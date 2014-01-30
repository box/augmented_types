#ifndef PHP_AUGMENTED_UTIL_H
#define PHP_AUGMENTED_UTIL_H

#include "zend_compile.h"

void debug_print_op_array(zend_op_array *op_array);
void debug_print_arg_info(zend_arg_info *arg_info);
void debug_print_zend_literal(zend_literal *l);
void print_znode_op(znode_op op, zend_uchar type);
void print_op_code(zend_uchar opcode);

#endif /* PHP_AUGMENTED_UTIL_H */
