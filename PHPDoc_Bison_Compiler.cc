#include "PHPDoc_Bison_Compiler.h"
#include "php_augmented_types.h"
#include "augmented_annotation_compiler.h"

#include "zend_exceptions.h"
#include "zend_execute.h"
#include "zend_compile.h"
#include "ext/spl/spl_exceptions.h"

extern int phpdocparse(PHPDoc_Function** f);

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern yy_buffer_state* phpdoc_scan_string(const char *);
extern void phpdoc_delete_buffer(yy_buffer_state* st);

/**
 * Single entrypoint for compiling a function's annotations with flex + bison
 * populates the zend_literal used for enforcement of this function in the 'literal' pointer
 * returns SUCCESS or FAILURE
 */
int PHPDoc_Bison_Compiler::compileFunction(zend_op_array* op_array, zend_literal *literal)
{
	DPRINTF("Compiling function %s\n", op_array->function_name);
	//DPRINTF("PHPDoc_Bison_Compiler doc_comment: %s\n", op_array->doc_comment);

	PHPDoc_Function fn;
	PHPDoc_Function *fnptr = &fn;
	bool is_closure = strcmp(op_array->function_name, "{closure}") == 0;
	bool is_constructor = strncmp(op_array->function_name, "__construct", 12) == 0;

	// NOTE: we _only_ allow functions to _not_ specify valid PHPDoc when either
	// the function is a closure OR the function is a constructor that has no arguments
	// also, note that closures sometime erratically pick up random PHPDoc in code, so
	// we just skip phpdoc on closures altogether now
	bool function_should_be_enforced = !(is_closure || (is_constructor && op_array->num_args == 0));

	// bail out early if this function doesn't need phpdoc,
	// throw an appropriate error message otherwise
	if (!function_should_be_enforced) {
		return FAILURE;
	} else if (!op_array->doc_comment) {
		// Add an appropriate error string
		if (is_constructor) {
			fn.set_error_string("No phpdoc specified - all constructors with arguments require phpdoc. (Did you remember to use /** ?)");
		} else {
			fn.set_error_string("No phpdoc specified - all named functions require phpdoc. (Did you remember to use /** ?)");
		}
		throw_compilation_exception(op_array, fnptr);
		return FAILURE;
	}

	YY_BUFFER_STATE state = phpdoc_scan_string(op_array->doc_comment);
	int result = phpdocparse(&fnptr);
	DPRINTF("phpdocparse returned %d\n", result);

	// if the compilation failed, cleanup, throw an exception, and bail!
	if (result != 0) {
		phpdoc_delete_buffer(state);
		throw_compilation_exception(op_array, &fn);
		return FAILURE;
	}

	// enforce that all compiled functions other than __construct methods
	// have return types.
	if (is_constructor && !fn.get_return()) {
		fn.set_return(new PHPDoc_Void_Type());
	} else if (!fn.get_return()) {
		fn.set_error_string("Return types must always be specified.");
		phpdoc_delete_buffer(state);
		throw_compilation_exception(op_array, &fn);
		return FAILURE;
	}

	// dump all of the PHPDoc_Function into the zend_literal
	*literal = fn.serialize();

	// delete the buffer once we're done
	phpdoc_delete_buffer(state);
	return SUCCESS;
}

/**
 * Throws an informative compilation fatal error
 */
void PHPDoc_Bison_Compiler::throw_compilation_exception(zend_op_array* op_array, PHPDoc_Function *fn)
{
	TSRMLS_FETCH();
	zend_error(ATCG(compilation_error_level), "Function annotation compilation failed for function %s in file %s at line %u, error message: %s\n",
			op_array->function_name, op_array->filename, op_array->line_start, fn->get_error_string());
}
