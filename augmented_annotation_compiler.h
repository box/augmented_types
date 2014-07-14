#ifndef PHP_AUGMENTED_ANNOTATION_COMPILER_H
#define PHP_AUGMENTED_ANNOTATION_COMPILER_H

#include "zend_compile.h"

int compile_new_user_function_annotations(TSRMLS_D);
zend_literal *get_enforcement_literal(zend_op_array *op_array);

#endif /* PHP_AUGMENTED_ANNOTATION_COMPILER_H */
