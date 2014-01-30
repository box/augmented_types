#ifndef PHP_AUGMENTED_ANNOTATION_COMPILER_H
#define PHP_AUGMENTED_ANNOTATION_COMPILER_H

#include "zend_compile.h"

#define ENFORCE_OPCODE 159

void prepare_compiler_state(TSRMLS_D);
int compile_new_user_function_annotations(TSRMLS_D);
int compile_user_function(zend_op_array *func);
bool should_compile_file(const char* path TSRMLS_DC);
zend_literal *get_enforcement_literal(zend_op_array *op_array);

#endif /* PHP_AUGMENTED_ANNOTATION_COMPILER_H */
