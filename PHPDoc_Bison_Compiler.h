#ifndef PHPDOC_BISON_COMPILER_H
#define PHPDOC_BISON_COMPILER_H

#include "PHPDoc_Compiler.h"
#include "phpdoc/phpdoc.h"

class PHPDoc_Bison_Compiler : public PHPDoc_Compiler
{
public:
	int compileFunction(zend_op_array* op_array, zend_literal *literal);
private:
	void throw_compilation_exception(zend_op_array* op_array, PHPDoc_Function *fn);
	bool isFuncNameClosure(const char *name);
};

#endif /* PHPDOC_BISON_COMPILER_H */
