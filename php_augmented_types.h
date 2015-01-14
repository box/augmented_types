#ifndef PHP_AUGMENTED_TYPES_H
#define PHP_AUGMENTED_TYPES_H

#define PHP_AUGMENTED_TYPES_VERSION "0.6.6"
#define PHP_AUGMENTED_TYPES_EXTNAME "augmented_types"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
}

#include "zend_extensions.h"
#include "zend_compile.h"

typedef struct _file_list_elem {
	char* path;
	int pathlen;
	_file_list_elem *next;
} file_list_elem;

typedef struct _augmented_types_globals {
	int				num_funcs_scanned;		// The number of functions and classes scanned for compilation.
	int				num_classes_scanned;	// Used for judging how far to go on each compilation pass
	file_list_elem	*whitelist_head;		// Head of the file whitelist
	file_list_elem	*blacklist_head;		// Head of the file blacklist
	zend_bool		enforce_by_default;
	long			compilation_error_level;
	const char*		current_namespaced_entity;		// The current namespaced entity that we are compiling annotations for.
	int 			current_namespace_prefix_len;	// It is either a classname or a global function name.
	const char*		error_callback_name;			// The name of the callback we call on runtime type errors. Init-ed to NULL
	char*			whitelist_ini_list;
	char*			blacklist_ini_list;
} augmented_types_globals;

#ifdef ZTS
#define ATCG(v) TSRMG(at_globals_id, augmented_types_globals *, v)
extern int at_globals_id;
#else
#define ATCG(v) (at_globals.v)
extern augmented_types_globals at_globals;
#endif /* ZTS */

#endif /* PHP_AUGMENTED_TYPES_H */
