#include "augmented_util.h"
#include "augmented_annotation_compiler.h"
#include "php_augmented_types.h"
#include "zend_types.h"
#include "phpdoc/phpdoc.h"

void print_op_code(zend_uchar opcode)
{
	switch (opcode) {
	case ZEND_INCLUDE_OR_EVAL:
		DPRINTF("ZEND_INCLUDE_OR_EVAL\n");
		break;
	case ZEND_NOP:
		DPRINTF("ZEND_NOP\n");
		break;
	case ZEND_ADD:
		DPRINTF("ZEND_ADD\n");
		break;
	case ZEND_RECV:
		DPRINTF("ZEND_RECV\n");
		break;
	case ZEND_DO_FCALL:
		DPRINTF("ZEND_DO_FCALL\n");
		break;
	case ZEND_DO_FCALL_BY_NAME:
		DPRINTF("ZEND_DO_FCALL_BY_NAME\n");
		break;
	case ZEND_RETURN:
		DPRINTF("ZEND_RETURN\n");
		break;
	case ZEND_SEND_VAL:
		DPRINTF("ZEND_SEND_VAL\n");
		break;
	case ZEND_SEND_VAR:
		DPRINTF("ZEND_SEND_VAR\n");
		break;
	case ZEND_INIT_FCALL_BY_NAME:
		DPRINTF("ZEND_INIT_FCALL_BY_NAME\n");
		break;
	case ZEND_ASSIGN:
		DPRINTF("ZEND_ASSIGN\n");
		break;
	case ZEND_ECHO:
		DPRINTF("ZEND_ECHO\n");
		break;
	default:
		DPRINTF("opcode = %u\n", opcode);
	}
}

void print_znode_op(znode_op op, zend_uchar type)
{
	switch (type) {
	case IS_CONST:
		DPRINTF("constant! const = %u\n", op.constant);
		break;
	case IS_TMP_VAR:
		DPRINTF("is tmp var?\n");
		break;
	case IS_VAR:
		DPRINTF("is var?\n");
		break;
	case IS_UNUSED:
		DPRINTF("is unused?\n");
		break;
	case IS_CV:
		DPRINTF("is compiled var?\n");
		break;
	case EXT_TYPE_UNUSED:
		DPRINTF("is ext_type_unused?\n");
		break;
	case 0:
		DPRINTF("is 0?\n");
		break;
	default:
		DPRINTF("is of unknown type %u\n", type);
		break;
	}
}

void debug_print_op_array(zend_op_array *op_array)
{
	DPRINTF("\nBEGIN OPARRAY \n");
	DPRINTF("Memory address is %p\n", op_array);
	DPRINTF("The number of opcodes is %u\n", op_array->last);
	DPRINTF("The function name is %s\n", op_array->function_name);
	DPRINTF("The number of args is %u\n", op_array->num_args);
	DPRINTF("The filename is %s\n", op_array->filename);
	DPRINTF("Line start is %u\n", op_array->line_start);
	DPRINTF("Line end is %u\n", op_array->line_end);
	DPRINTF("prototype pointer= %p\n", op_array->prototype);

	// loop through opcodes, print them
	for (uint i = 0; i < op_array->last; i++) {
		zend_op op = op_array->opcodes[i];
		DPRINTF("line number = %u\n", op.lineno);
		print_op_code(op.opcode);
		DPRINTF("OP1 =\t\t");
		print_znode_op(op.op1, op.op1_type);
		DPRINTF("OP2 =\t\t");
		print_znode_op(op.op2, op.op2_type);
		DPRINTF("RESULT =\t");
		print_znode_op(op.result, op.result_type);
		DPRINTF("\n\n");
	}

	DPRINTF("END OPARRAY \n\n");
}

void debug_print_arg_info(zend_arg_info *arg_info)
{
	DPRINTF("ArgInfo name: %s\n", arg_info->name);
	DPRINTF("ArgInfo class name: %s\n", arg_info->class_name);
	DPRINTF("ArgInfo Type hint: %u\n\n", arg_info->type_hint);
}

void debug_print_zend_literal(zend_literal *l)
{
	DPRINTF("Zend Literal type %u\n", l->constant.type);
	DPRINTF("Zend literal str len %u\n", l->constant.value.str.len);
}
