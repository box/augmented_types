#include "augmented_util.h"
#include "augmented_annotation_compiler.h"
#include "php_augmented_types.h"
#include "zend_types.h"
#include "PHPDoc_Bison_Compiler.h"

bool should_compile_file(const char* path TSRMLS_DC);

int get_num_whitelisted_op_refs_in_function_table(HashTable* function_table, int num_funcs_to_explore, zend_op *opcodes TSRMLS_DC)
{
	int count = 0;
	zend_function *func;
	HashPosition pos;
	for (zend_hash_internal_pointer_end_ex(function_table, &pos);
			zend_hash_get_current_data_ex(function_table, (void **) &func, &pos) == SUCCESS && num_funcs_to_explore > 0;
			zend_hash_move_backwards_ex(function_table, &pos), --num_funcs_to_explore) {
		if (func->type != ZEND_USER_FUNCTION) {
			continue;
		}
		DPRINTF("doing a sweep through func %s\n", func->op_array.function_name);
		if (func->op_array.opcodes == opcodes && should_compile_file(func->op_array.filename TSRMLS_CC)) {
			count++;
		}
	}
	return count;
}

int get_num_whitelisted_op_refs_in_class_table(int num_classes_to_explore, zend_op *opcodes TSRMLS_DC)
{
	int count = 0;
	zend_class_entry **pce;
	HashPosition pos;
	for (zend_hash_internal_pointer_end_ex(CG(class_table), &pos);
			zend_hash_get_current_data_ex(CG(class_table), (void **) &pce, &pos) == SUCCESS && num_classes_to_explore > 0;
			zend_hash_move_backwards_ex(CG(class_table), &pos), --num_classes_to_explore) {
		if ((*pce)->type != ZEND_USER_CLASS) {
			continue;
		}
		DPRINTF("doing a sweep through class %s\n", (*pce)->name);
		HashTable *func_table = &((*pce)->function_table);
		count += get_num_whitelisted_op_refs_in_function_table(func_table, zend_hash_num_elements(func_table), opcodes TSRMLS_CC);
	}
	return count;
}

/**
 * Helper method to get the number of functions in this compilation pass that
 * will be compiled that reference a particular array of opcodes. This is needed
 * to ensure that we will compile _all_ refrences to this op array so that we
 * can safely destroy the old literals array
 */
int get_num_whitelisted_op_array_refs_in_sweep(zend_op* opcodes TSRMLS_DC)
{
	int num_funcs_to_explore = zend_hash_num_elements(CG(function_table)) - ATCG(num_funcs_scanned);
	int num_classes_to_explore = zend_hash_num_elements(CG(class_table)) - ATCG(num_classes_scanned);
	return get_num_whitelisted_op_refs_in_class_table(num_classes_to_explore, opcodes TSRMLS_CC) +
		get_num_whitelisted_op_refs_in_function_table(CG(function_table), num_funcs_to_explore, opcodes TSRMLS_CC);
}

/**
 * Augments the function represented by func with additional type hint enforcement opcodes
 * Func must represent a function with a non-null doc_comment field
 * Returns 1 on success or 0 if it was a no-op
 */
int compile_user_function(zend_op_array *func TSRMLS_DC)
{
	// first, ensure that this function hasn't already been compiled
	// if it has, ensure everything is correctly assigned
	zend_literal *enforcer = get_enforcement_literal(func);
	if (enforcer) {
		assert(*func->refcount > 1);
		if (enforcer != func->literals) {
			func->literals = enforcer;
			func->last_literal++;
		}
		return 1;
	}

	// dump the new zend_op and zend_literal for annotations on the stack for now.
	zend_literal enforce_literal;
	int old_num_literals = func->last_literal;
	zend_literal *old_literals = func->literals;

	// if another op array references this function's op array, ensure that we will
	// compile that op array too on this pass, otherwise we must bail out. This
	// can happen when a superclass and subclass are whitelisted after the superclass
	// has been compiled, and it is not good - we must warn the user
	if (*func->refcount > 1) {
		if (*func->refcount != get_num_whitelisted_op_array_refs_in_sweep(func->opcodes TSRMLS_CC)) {
			zend_error(ATCG(compilation_error_level), "Compilation failed for function %s in file %s due to a duplicate reference, and its type information won't be enforced. This likely happened because the file was added to the whitelist after it was required during execution.\n",
					func->function_name, func->filename);
			return 0;
		}
	}

	// make a compiler object and compile this annotation!
	PHPDoc_Bison_Compiler compiler;
	if (compiler.compileFunction(func, &enforce_literal) == FAILURE) {
		return 0;
	}

	// realloc the literals array and
	// append the new annotation zend_literals onto the beginning of func's literals array
	zend_literal *new_literals = (zend_literal*) emalloc(sizeof(zend_literal) * (1 + old_num_literals));
	new_literals[0] = enforce_literal;
	memcpy(new_literals + 1, func->literals, sizeof(zend_literal) * old_num_literals);
	efree(func->literals);
	func->literals = new_literals;
	func->last_literal = old_num_literals + 1;

	// modify the original zend_ops to point to the literals in the new literals array
	zend_literal *old_literals_start = new_literals + 1;
	for (zend_uint i = 0; i < func->last; i++) {
		zend_op *cur = func->opcodes + i;

		if (cur->op1_type == IS_CONST) {
			cur->op1.literal = (zend_literal*)((char*)cur->op1.literal + ((char*)old_literals_start - (char*)old_literals));
		}
		if (cur->op2_type == IS_CONST) {
			cur->op2.literal = (zend_literal*)((char*)cur->op2.literal + ((char*)old_literals_start - (char*)old_literals));
		}
	}

	// finally, modify the last ZEND_RETURN opcode in the opcodes array to have a constant op2 field
	// pointing at the new enforce literal
	zend_op *ret_op = func->opcodes + (func->last - 1);
	ret_op->op2_type = IS_CONST;
	ret_op->op2.literal = new_literals;
	return 1;
}

/**
 * Helper method to get the enforcement literal from this op_array
 * If this op_array doesn't have an enforcement literal, this function
 * will return NULL
 */
zend_literal *get_enforcement_literal(zend_op_array *op_array)
{
	zend_op *last_opcode = op_array->opcodes + op_array->last -1;
	if (last_opcode->op2_type == IS_CONST && last_opcode->op2.literal) {
		return last_opcode->op2.literal;
	}
	return NULL;
}

/**
 * Helper method to check if a file is contained in a file list elem linked list
 * Returns the maximum length match, or 0 if nothing is found
 */
static int file_list_contains_path(file_list_elem *elem, const char* path)
{
	int longestMatch = 0;
	while (elem) {
		if (strncmp(elem->path, path, elem->pathlen) == 0 &&
				elem->pathlen > longestMatch) { // a longest match!
			longestMatch = elem->pathlen;
		}
		elem = elem->next;
	}
	return longestMatch;
}

/**
 * Helper method to determine if we should compile a particular file
 * Uses the whitelist and blacklist. It has to iterate through all of both lists,
 * so be sparing with calling thsi function
 */
bool should_compile_file(const char* path TSRMLS_DC)
{
	int longestWhitelist = file_list_contains_path(ATCG(whitelist_head), path);
	int longestBlacklist = file_list_contains_path(ATCG(blacklist_head), path);

	// if we are _not_ enforcing by default, compile files only if they have a strictly longer
	// whitelist match if we are, compile files that have an equal whitelist match (for files
	// with no match on either list, longestWhitelist == longestBlacklist == 0)
	return ((!ATCG(enforce_by_default) && longestWhitelist > longestBlacklist) ||
			(ATCG(enforce_by_default) && longestWhitelist >= longestBlacklist));
}

/**
 * Helper method to get the length of the namespace prefix from a classname or
 * function name. Returns 0 if this class/function lives in the global namespace
 */
int get_namespace_prefix_length(const char* entity_name, int entity_name_len)
{
	int idx = entity_name_len;
	for (; idx > 0; idx--) {
		if (entity_name[idx] == '\\') {
			return idx + 1;
		}
	}
	return 0;
}

/**
 * Goes through the global function table and parses any new userland functions
 * that happen to have doc comments.
 * Returns the number of functions that annotation op codes were added for
 */
int compile_new_global_user_functions(TSRMLS_D)
{
	HashTable *function_table = CG(function_table);
	zend_function *func;
	int num_funcs_added = 0;
	HashPosition temp_pos;
	int num_funcs_to_explore = zend_hash_num_elements(function_table) - ATCG(num_funcs_scanned);

	for (zend_hash_internal_pointer_end_ex(function_table, &temp_pos);
			zend_hash_get_current_data_ex(function_table, (void **) &func, &temp_pos) == SUCCESS && num_funcs_to_explore > 0;
			zend_hash_move_backwards_ex(function_table, &temp_pos), --num_funcs_to_explore) {

		// only compile valid functions in whitelisted files
		if (func->type != ZEND_USER_FUNCTION ||
				!should_compile_file(func->op_array.filename TSRMLS_CC)) {
			continue;
		}

		DPRINTF("ATTEMPTING TO COMPILE name = %s\n", func->common.function_name);

		// if this function lives in a namespace, be sure to set the namespace prefix so we
		// can use it later when compiling classname annotations
		int namespace_prefix_len =
			get_namespace_prefix_length(func->common.function_name, strlen(func->common.function_name));
		if ( namespace_prefix_len ) {
			ATCG(current_namespaced_entity) = func->common.function_name;
			ATCG(current_namespace_prefix_len) = namespace_prefix_len;
		}

		if (compile_user_function(&(func->op_array) TSRMLS_CC)) {
			num_funcs_added++;
		}

		ATCG(current_namespaced_entity) = NULL;
		ATCG(current_namespace_prefix_len) = 0;
	}

	assert(num_funcs_to_explore == 0);
	return num_funcs_added;
}

/**
 * Adds annotation enforcement codes to the relevant methods on a class
 * Returns the number of method annotations compiled, or -1 on failure
 */
int add_class_instance_methods(zend_class_entry *ce TSRMLS_DC)
{
	const char *class_file = ce->info.user.filename;
	int class_file_strlen = strlen(class_file);
	int num_methods_compiled = 0;
	HashPosition method_pos;
	zend_function *func;
	HashTable *func_table = &(ce->function_table);

	for (zend_hash_internal_pointer_reset_ex(func_table, &method_pos);
			zend_hash_get_current_data_ex(func_table, (void **) &func, &method_pos) == SUCCESS;
			zend_hash_move_forward_ex(func_table, &method_pos)) {

		// skip functions that aren't user functions
		if (func->type != ZEND_USER_FUNCTION) {
			continue;
		}

		DPRINTF("inspecting instance method %s at %p\n", func->common.function_name, func);
		DPRINTF("instance method filename = %s\n", func->op_array.filename);

		// we must once again ensure that we should compile this method using the
		// blacklist + whitelist NOTE: this normally would not be necessary, but
		// zend re-uses op_arrays for inherited instance methods
		// PS: To save CPU cycles, only check the whitelist + blacklist if this
		// function resides in a different file than the class
		if (strncmp(class_file, func->op_array.filename, class_file_strlen) == 0 ||
				should_compile_file(func->op_array.filename TSRMLS_CC)) {

			DPRINTF("ATTEMPTING TO COMPILE instance method %s\n", func->common.function_name);
			if (compile_user_function(&(func->op_array) TSRMLS_CC)) {
				num_methods_compiled++;
			}
		}
	}
	return num_methods_compiled;
}

/**
 * Compiles new function annotations from new entries in the global class table
 * Returns the number of instance methods that annotation op codes were added for
 */
int compile_new_instance_method_annotations(TSRMLS_D)
{
	HashTable *class_table = CG(class_table);
	zend_class_entry **pce;
	int num_methods_added = 0;
	int num_instance_methods_added = 0;
	HashPosition temp_pos;
	int num_classes_to_explore = zend_hash_num_elements(class_table) - ATCG(num_classes_scanned);

	for (zend_hash_internal_pointer_end_ex(class_table, &temp_pos);
			zend_hash_get_current_data_ex(class_table, (void **) &pce, &temp_pos) == SUCCESS && num_classes_to_explore > 0;
			zend_hash_move_backwards_ex(class_table, &temp_pos), --num_classes_to_explore) {

		// only compile valid classes declared in whitelisted files
		if ((*pce)->type != ZEND_USER_CLASS ||
				!should_compile_file((*pce)->info.user.filename TSRMLS_CC)) {
			continue;
		}

		DPRINTF("\t\t\tADDING new type enforcement info for class %s\n", (*pce)->name);

		int namespace_prefix_len = get_namespace_prefix_length((*pce)->name, (int) (*pce)->name_length);
		if (namespace_prefix_len) {
			ATCG(current_namespaced_entity) = (*pce)->name;
			ATCG(current_namespace_prefix_len) = namespace_prefix_len;
		}

		// now, we must iterate through its methods!
		num_instance_methods_added = add_class_instance_methods(*pce TSRMLS_CC);
		if (num_instance_methods_added == -1) {
			DPRINTF("ERROR ADDING ANNOTATIONS FOR CLASS %s\n", (*pce)->name);
		} else {
			num_methods_added += num_instance_methods_added;
		}

		// always reset the current namespaced entity..
		ATCG(current_namespaced_entity) = NULL;
		ATCG(current_namespace_prefix_len) = 0;
	}

	assert(num_classes_to_explore == 0);
	return num_methods_added;
}

/**
 * Compiles all new user-land functions with annotations. Returns the
 * number of functions compiled. Compiles annotations on both instance
 * methods and standalone user functions
 */
int compile_new_user_function_annotations(TSRMLS_D)
{
	return compile_new_instance_method_annotations(TSRMLS_C) +
		compile_new_global_user_functions(TSRMLS_C);
}


/**
 * Function to prepare the compiler's state before each compilation
 * pass so we can judge what new functions+classes to compile
 */
void prepare_compiler_state(TSRMLS_D)
{
	ATCG(num_classes_scanned) = zend_hash_num_elements(CG(class_table));
	ATCG(num_funcs_scanned) = zend_hash_num_elements(CG(function_table));
}
