#include "php.h"
#include "php_augmented_types.h"
#include "augmented_types_module.h"
#include "phpdoc/phpdoc.h" // would be nice not to have to include this because of DPRINTF
#include "zend_ini.h"

#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_ini.h"

/* Parse out the relevant ini values */
ZEND_INI_BEGIN()
	STD_PHP_INI_ENTRY("augmented_types.enforce_by_default", "0", PHP_INI_ALL, OnUpdateBool,
			enforce_by_default, augmented_types_globals, at_globals)
	STD_PHP_INI_ENTRY("augmented_types.compilation_error_level", "1" /* 1 == E_ERROR */, PHP_INI_ALL, OnUpdateLong,
			compilation_error_level, augmented_types_globals, at_globals)
ZEND_INI_END()

ZEND_BEGIN_ARG_INFO_EX(arginfo_augmented_types_blacklist, 0, 0, 1)
	ZEND_ARG_INFO(0, blacklist_file_array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_augmented_types_whitelist, 0, 0, 1)
	ZEND_ARG_INFO(0, whitelist_file_array)
ZEND_END_ARG_INFO()

/**
 * Helper function to add filenames in array 'arr' to the passed in file linked list
 */
static int prepend_paths_to_file_linked_list(HashTable *arr, file_list_elem **head TSRMLS_DC)
{
	HashPosition pos;
	zval **entry;
	file_list_elem *new_elem;
	char *filename;
	int num_added = 0;

	for (zend_hash_internal_pointer_reset_ex(arr, &pos);
			zend_hash_get_current_data_ex(arr, (void **) &entry, &pos) == SUCCESS;
			zend_hash_move_forward_ex(arr, &pos)) {

		if (Z_TYPE_PP(entry) == IS_STRING) {

			// filename is emalloc-ed
			filename = zend_resolve_path(Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) TSRMLS_CC);
			DPRINTF("path %s resolved to filename %s\n", Z_STRVAL_PP(entry), filename);
			if (filename) {
				new_elem = (file_list_elem*) emalloc(sizeof(file_list_elem));
				new_elem->path = filename;
				new_elem->pathlen = strlen(filename);
				new_elem->next = *head;
				*head = new_elem;
				num_added++;
			}
		}
	}

	return num_added;
}

ZEND_FUNCTION(augmented_types_blacklist)
{
	zval *arr;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr) == FAILURE) {
		RETURN_NULL();
	}

	// ensure that zarray is in fact an array
	if (Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception(spl_ce_InvalidArgumentException,
				"augmented_types_blacklist only takes an array of filenames!", 0 TSRMLS_CC);
		return;
	}

	// return the number of paths added to the blacklist head
	RETURN_LONG( prepend_paths_to_file_linked_list(Z_ARRVAL_P(arr), &(ATCG(blacklist_head)) TSRMLS_CC) );
}

ZEND_FUNCTION(augmented_types_whitelist)
{
	zval *arr;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr) == FAILURE) {
		RETURN_NULL();
	}

	// ensure that zarray is in fact an array
	if (Z_TYPE_P(arr) != IS_ARRAY) {
		zend_throw_exception(spl_ce_InvalidArgumentException,
				"augmented_types_whitelist only takes an array of filenames!", 0 TSRMLS_CC);
		return;
	}

	RETURN_LONG( prepend_paths_to_file_linked_list(Z_ARRVAL_P(arr), &(ATCG(whitelist_head)) TSRMLS_CC) );
}

ZEND_MINIT_FUNCTION(augmented_types)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

static zend_function_entry augmented_types_functions[] = {
	ZEND_FE(augmented_types_blacklist, arginfo_augmented_types_blacklist)
	ZEND_FE(augmented_types_whitelist, arginfo_augmented_types_whitelist)
	{NULL, NULL, NULL}
};

zend_module_entry augmented_types_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_AUGMENTED_TYPES_EXTNAME,
	augmented_types_functions, /* functions */
	ZEND_MINIT(augmented_types), /* minit */
	NULL, /* mshutdown */
	NULL, /* rinit */
	NULL, /* rshutdown */
	NULL, /* minfo */
#if ZEND_MODULE_API_NO >= 20010901
	PHP_AUGMENTED_TYPES_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

int start_augmented_types_module(void)
{
	return zend_startup_module(&augmented_types_module_entry);
}
