#ifndef PHPDOC_COMPILER_H
#define PHPDOC_COMPILER_H

#include "zend_compile.h"
#include "zend_types.h"

/**
 * Base interface for objects that compile PHPDoc comments
 */
class PHPDoc_Compiler
{

public:
	// populates the zend_literal used for enforcement of this function in the 'literal' pointer
	// returns SUCCESS or FAILURE
	virtual int compileFunction(zend_op_array* op_array, zend_literal *literal) = 0;
};

#endif /* PHPDOC_COMPILER_H */
