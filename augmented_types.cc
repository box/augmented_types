#include "php_augmented_types.h"
#include "augmented_util.h"
#include "augmented_annotation_compiler.h"
#include "augmented_types_module.h"
#include "phpdoc/phpdoc.h"

#include "zend_extensions.h"
#include "zend_execute.h"
#include "zend_compile.h"

/* Global storage */
#ifdef ZTS
int at_globals_id;
#else
augmented_types_globals at_globals;
#endif /* ZTS */

/* compilation redirection functions */
zend_op_array* (*old_compile_file)(zend_file_handle* file_handle, int type TSRMLS_DC);
zend_op_array* augmented_types_compile_file(zend_file_handle*, int type TSRMLS_DC);

zend_op_array* (*old_compile_string)(zval* source_string, char *filename TSRMLS_DC);
zend_op_array* augmented_types_compile_string(zval* source_string, char *filename TSRMLS_DC);

/* execution redirection functions */
#if PHP_VERSION_ID < 50500
void (*old_execute)(zend_op_array *op_array TSRMLS_DC);
void augmented_types_execute(zend_op_array *op_array TSRMLS_DC);
#else
void (*old_execute_ex)(zend_execute_data *execute_data TSRMLS_DC);
void augmented_types_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
#endif /* PHP_VERSION_ID < 50500 */

static void at_globals_ctor(augmented_types_globals *globals TSRMLS_DC)
{
	// set all of the things to null
	memset((void*) globals, 0, sizeof(augmented_types_globals));
}

/**
 * Helper method to free file linked lists used by the global AT struct
 */
static void free_file_list(file_list_elem *elem)
{
	if (!elem) {
		return;
	}
	file_list_elem *next = elem->next;
	efree(elem->path);
	efree(elem);
	free_file_list(next); // horray tail recursion!!
}

static int augmented_types_start(zend_extension *extension)
{
	/* init thread-safe augmented types global struct */
#ifdef ZTS
	ts_allocate_id(&at_globals_id, sizeof(augmented_types_globals), (ts_allocate_ctor) at_globals_ctor, NULL);
#else
	at_globals_ctor(&at_globals TSRMLS_CC);
#endif

	/* start up the normal php module */
	if (start_augmented_types_module() == FAILURE) {
		zend_error(E_ERROR, "augmented types module startup failed!");
		return FAILURE;
	}

	/* Redirect compile and execute functions to our own */
	old_compile_file = zend_compile_file;
	zend_compile_file = augmented_types_compile_file;

	old_compile_string = zend_compile_string;
	zend_compile_string = augmented_types_compile_string;

#if PHP_VERSION_ID < 50500
	old_execute = zend_execute;
	zend_execute = augmented_types_execute;
#else
	old_execute_ex = zend_execute_ex;
	zend_execute_ex = augmented_types_execute_ex;
#endif

	return SUCCESS;
}

zend_op_array *augmented_types_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	// must be called before old_compile_file to ensure we can accurately judge
	// what functions and classes old_compile_file adds
	prepare_compiler_state(TSRMLS_C);

	zend_op_array *op_array = old_compile_file(file_handle, type TSRMLS_CC);

	DPRINTF("FROM COMPILE::::\n");
	DPRINTF("\t\t\tFor filename = %s\n", file_handle->filename);

	if (should_compile_file(file_handle->filename TSRMLS_CC)) {
		int num_annotations_compiled = compile_new_user_function_annotations(TSRMLS_C);

		if (AT_DEBUG) {
			if (num_annotations_compiled > 0) {
				DPRINTF("success compiling annotations! %d new annotations compiled\n\n", num_annotations_compiled);
			} else if (num_annotations_compiled == 0) {
				DPRINTF("no new annotations compiled\n\n");
			} else {
				DPRINTF("failure compiling annotations\n\n");
			}
		}
	}

	return op_array;
}

zend_op_array* augmented_types_compile_string(zval* source_string, char *filename TSRMLS_DC)
{
	prepare_compiler_state(TSRMLS_C);
	zend_op_array *res = old_compile_string(source_string, filename TSRMLS_CC);

	// check if we should compile new functions produced by this eval-ed string
	if (should_compile_file(filename TSRMLS_CC)) {
		compile_new_user_function_annotations(TSRMLS_C);
	}

	return res;
}

#if PHP_VERSION_ID < 50500
void augmented_types_execute(zend_op_array *op_array TSRMLS_DC) {
#else
void augmented_types_execute_ex(zend_execute_data *execute_data TSRMLS_DC) {
	zend_op_array *op_array = execute_data->op_array;
#endif /* PHP_VERSION_ID < 50500 */

	zend_literal *enforcement_literal = get_enforcement_literal(op_array);
	if (enforcement_literal) {

#if PHP_VERSION_ID < 50500
		int num_args = (int)(zend_uintptr_t) (*(EG(argument_stack)->top - 1));
		zval **args = (zval **)(EG(argument_stack)->top - num_args - 1);
#else
		int num_args = zend_vm_stack_get_args_count_ex(execute_data->prev_execute_data);
		zval **args = zend_vm_stack_get_arg_ex(execute_data->prev_execute_data, 1);
#endif /* PHP_VERSION_ID < 50500 */

		DPRINTF("ENFORCING FROM EXECUTE::::");
		DPRINTF("for function %s\n", op_array->function_name);
		PHPDoc_Function func(enforcement_literal);
		func.enforce_argument_types(args, num_args, op_array TSRMLS_CC);

#if PHP_VERSION_ID < 50500
		old_execute(op_array TSRMLS_CC);
#else
		old_execute_ex(execute_data TSRMLS_CC);
#endif /* PHP_VERSION_ID < 50500 */

		func.enforce_return_type(EG(return_value_ptr_ptr), op_array TSRMLS_CC);
		return;
	}

#if PHP_VERSION_ID < 50500
	old_execute(op_array TSRMLS_CC);
#else
	old_execute_ex(execute_data TSRMLS_CC);
#endif /* PHP_VERSION_ID < 50500 */
}

static void augmented_types_deactivate(void)
{
	/* We always have to free the whitelist and blacklist file lists at the
	 * end of each request. Unfortunately, we can't do this in the dtor for
	 * the globals struct, because it gets called too late for efree */
	TSRMLS_FETCH();
	free_file_list(ATCG(whitelist_head));
	free_file_list(ATCG(blacklist_head));
	ATCG(whitelist_head) = NULL;
	ATCG(blacklist_head) = NULL;
	if (ATCG(error_callback_name)) {
		efree((void *) ATCG(error_callback_name));
	}
}

//// BEGIN BOILERPLATE //////

#ifndef ZEND_EXT_API
#define ZEND_EXT_API    ZEND_DLEXPORT
#endif

ZEND_EXT_API zend_extension zend_extension_entry = {
	PHP_AUGMENTED_TYPES_EXTNAME,			/* name */
	PHP_AUGMENTED_TYPES_VERSION,			/* version */
	"jmarrama@box.com, abishopric@box.com",	/* author */
	"http://www.box.com/",					/* URL */
	"Copyright (c) 2013",					/* copyright */
	augmented_types_start,					/* startup */
	NULL,									/* shutdown */
	NULL,									/* per-script activation */
	augmented_types_deactivate,				/* per-script deactivation */
	NULL,									/* message handler */
	NULL,									/* op_array handler */
	NULL,									/* extended statement handler */
	NULL,									/* extended fcall begin handler */
	NULL,									/* extended fcall end handler */
	NULL,									/* op_array ctor */
	NULL,									/* op_array dtor */
	STANDARD_ZEND_EXTENSION_PROPERTIES
};

ZEND_EXTENSION();

//// END BOILERPLATE //////
