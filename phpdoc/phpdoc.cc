#include <iostream>
#include "phpdoc.h"
#include <sstream>
#include <assert.h>
#include "../php_augmented_types.h"

#define PRIMITIVE_TYPE 1
#define NUMERIC_TYPE 2
#define CALLABLE_TYPE 3
#define VOID_TYPE 4
#define CLASSNAME_TYPE 5
#define MIXED_TYPE 6
#define DISJUNCTIVE_TYPE 7
#define VARIADIC_TYPE 8
#define ARRAYOF_TYPE 9
#define UNSIGNEDINT_TYPE 10

using namespace std;

/**
 * This sure as hell aint pretty, but its the best way I've found to
 * declare all PHPDoc_Type objects on the stack and still get a little
 * bit of (fake) polymorphism. Theres no getting around having a single function
 * that makes each subtype on the stack, so why not just do it in as
 * few lines as possible with macros?
 */
#define GENERATE_FUNC_NAME(TYPECLASS) TYPECLASS ## _deserialize_matcher
#define GENERATE_DESERIALIZE_TYPE_MATCHER(TYPECLASS) 			\
static bool GENERATE_FUNC_NAME(TYPECLASS)(zend_uchar *serial_t, zval **val, char *tstr, int tstr_len TSRMLS_DC)		\
{ 										\
	TYPECLASS t; 						\
	bool retval;						\
	t.deserialize(serial_t);			\
	retval = t.matches(val TSRMLS_CC);	\
	if (!retval) {						\
		snprintf(tstr, tstr_len, "%s", t.typestr().c_str());		\
	}									\
	return retval;						\
}

#define CALL_DESERIALIZED_TYPE_MATCHER(TYPECLASS, serial_t, val, tstr, tstr_len) GENERATE_FUNC_NAME(TYPECLASS)(serial_t, val, tstr, tstr_len TSRMLS_CC)

GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Primitive_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Numeric_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Callable_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Void_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Classname_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Mixed_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Disjunctive_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_UnsignedInt_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_ArrayOf_Type)
GENERATE_DESERIALIZE_TYPE_MATCHER(PHPDoc_Variadic_Type)


/**
 * Helper method to pull out the passed in serialized type and check
 * the zval** val against it. Returns whether the val passed or not
 * NOTE: this is 'fast' because it doesn't allocate anything on the heap
 */
static bool fast_match_type(zend_uchar *serialized_type, zval **val, char *expected_type, int expected_type_len TSRMLS_DC)
{
	switch (serialized_type[0]) {
		case PRIMITIVE_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Primitive_Type, serialized_type, val, expected_type, expected_type_len);
		case NUMERIC_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Numeric_Type, serialized_type, val, expected_type, expected_type_len);
		case CALLABLE_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Callable_Type, serialized_type, val, expected_type, expected_type_len);
		case VOID_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Void_Type, serialized_type, val, expected_type, expected_type_len);
		case CLASSNAME_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Classname_Type, serialized_type, val, expected_type, expected_type_len);
		case MIXED_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Mixed_Type, serialized_type, val, expected_type, expected_type_len);
		case DISJUNCTIVE_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Disjunctive_Type, serialized_type, val, expected_type, expected_type_len);
		case VARIADIC_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Variadic_Type, serialized_type, val, expected_type, expected_type_len);
		case ARRAYOF_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_ArrayOf_Type, serialized_type, val, expected_type, expected_type_len);
		case UNSIGNEDINT_TYPE:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_UnsignedInt_Type, serialized_type, val, expected_type, expected_type_len);
		default:
			return CALL_DESERIALIZED_TYPE_MATCHER(PHPDoc_Void_Type, serialized_type, val, expected_type, expected_type_len);
	}
}

/**
 * Helper method to get a heap allocated PHPDoc_Type from the passed in
 * buffer. This handles deserializing it from the passed-in serialized
 * PHPDoc_Type buffer
 */
static PHPDoc_Type *get_heap_alloced_type_from_buffer(zend_uchar* type_buf)
{
	PHPDoc_Type *t;
	zend_uchar type = type_buf[0];
	switch (type) {
		case PRIMITIVE_TYPE:
			t = new PHPDoc_Primitive_Type();
			break;
		case NUMERIC_TYPE:
			t = new PHPDoc_Numeric_Type();
			break;
		case CALLABLE_TYPE:
			t = new PHPDoc_Callable_Type();
			break;
		case VOID_TYPE:
			t = new PHPDoc_Void_Type();
			break;
		case CLASSNAME_TYPE:
			t = new PHPDoc_Classname_Type();
			break;
		case MIXED_TYPE:
			t = new PHPDoc_Mixed_Type();
			break;
		case DISJUNCTIVE_TYPE:
			t = new PHPDoc_Disjunctive_Type();
			break;
		case VARIADIC_TYPE:
			t = new PHPDoc_Variadic_Type();
			break;
		case ARRAYOF_TYPE:
			t = new PHPDoc_ArrayOf_Type();
			break;
		case UNSIGNEDINT_TYPE:
			t = new PHPDoc_UnsignedInt_Type();
			break;
		default:
			t = new PHPDoc_Void_Type();
			break;
	}

	t->deserialize(type_buf);
	return t;
}

static const char* get_str_for_type(zend_uchar type)
{
	switch (type) {
		case IS_NULL:
			return "null";
		case IS_BOOL:
			return "boolean";
		case IS_STRING:
			return "string";
		case IS_LONG:
			return "integer";
		case IS_DOUBLE:
			return "float";
		case IS_ARRAY:
			return "array";
		case IS_OBJECT:
			return "object";
		case IS_RESOURCE:
			return "resource";
		case 255:
			return "void";
		case 254:
			return "mixed";
		default:
			return "unknown";
	}
}


static void get_zval_typestr(zval **z, char *buf, int buflen TSRMLS_DC)
{
	if (!z) {
		snprintf(buf, buflen, "void");
		return;
	}

	switch (Z_TYPE_PP(z)) {
		case IS_NULL:
			snprintf(buf, buflen, "null");
			return;
		case IS_BOOL:
			if (Z_LVAL_PP(z)) {
				snprintf(buf, buflen, "(bool) true");
			} else {
				snprintf(buf, buflen, "(bool) false");
			}
			return;
		case IS_STRING:
			snprintf(buf, buflen, "(string) '%s'", Z_STRVAL_PP(z));
			return;
		case IS_LONG:
			snprintf(buf, buflen, "(int) %ld", Z_LVAL_PP(z));
			return;
		case IS_DOUBLE:
			snprintf(buf, buflen, "(float) %f", Z_DVAL_PP(z));
			return;
		case IS_ARRAY:
			snprintf(buf, buflen, "array");
			return;
		case IS_OBJECT:
			if (Z_OBJCE_P(*z)) {
				snprintf(buf, buflen, "%s", Z_OBJCE_P(*z)->name);
				return;
			} else {
				snprintf(buf, buflen, "unknown class");
				return;
			}
		case IS_RESOURCE:
			snprintf(buf, buflen, "resource");
			return;
		default:
			snprintf(buf, buflen, "unknown");
			return;
	}
}


PHPDoc_Type::PHPDoc_Type()
{
	next = NULL;
}

//////////////////// Begin PHPDoc_Primitive_Type ///////////////////////

PHPDoc_Primitive_Type::PHPDoc_Primitive_Type()
{
	primitive_type = 0;
	next = NULL;
}

PHPDoc_Primitive_Type::PHPDoc_Primitive_Type(zend_uchar type)
{
	primitive_type = type;
	next = NULL;
}

bool PHPDoc_Primitive_Type::matches(zval **z TSRMLS_DC)
{
	if (!z || !(*z)) {
		return false;
	}
	return Z_TYPE_PP(z) == primitive_type;
}

string PHPDoc_Primitive_Type::typestr()
{
	//DPRINTF("primitive type is %u\n", primitive_type);
	return get_str_for_type(primitive_type);
}

zend_uchar PHPDoc_Primitive_Type::get_type_uchar()
{
	return PRIMITIVE_TYPE;
}

int PHPDoc_Primitive_Type::serialize(zend_uchar *buf, bool writeMemory)
{
	if (writeMemory) {
		buf[0] = get_type_uchar();
		buf[1] = primitive_type;
	}
	return 2;
}

void PHPDoc_Primitive_Type::deserialize(zend_uchar *buf)
{
	primitive_type = buf[1];
}

//////////////////// Begin PHPDoc_Numeric_Type ////////////////////////

bool PHPDoc_Numeric_Type::matches(zval **z TSRMLS_DC)
{
	switch (Z_TYPE_PP(z)) {
	case IS_LONG:
	case IS_DOUBLE:
		return true;

	case IS_STRING:
		if (is_numeric_string(Z_STRVAL_PP(z), Z_STRLEN_PP(z), NULL, NULL, 0)) {
			return true;
		} else {
			return false;
		}
	default:
		return false;
	}
}

string PHPDoc_Numeric_Type::typestr()
{
	return "numeric";
}

zend_uchar PHPDoc_Numeric_Type::get_type_uchar()
{
	return NUMERIC_TYPE;
}

//////////////////// Begin PHPDoc_Callable_Type /////////////////////////

bool PHPDoc_Callable_Type::matches(zval **z TSRMLS_DC)
{
	char *error;
	zend_bool retval;

	retval = zend_is_callable_ex(*z, NULL, 0, NULL, NULL, NULL, &error TSRMLS_CC);
	if (error) {
		/* ignore errors */
		efree(error);
	}

	return retval;
}

string PHPDoc_Callable_Type::typestr()
{
	return "callable";
}

zend_uchar PHPDoc_Callable_Type::get_type_uchar()
{
	return CALLABLE_TYPE;
}


//////////////////// Begin PHPDoc_Void_Type /////////////////////////

bool PHPDoc_Void_Type::matches(zval **z TSRMLS_DC)
{
	return (!z);
}

string PHPDoc_Void_Type::typestr()
{
	return "void";
}

zend_uchar PHPDoc_Void_Type::get_type_uchar()
{
	return VOID_TYPE;
}

bool PHPDoc_Void_Type::contains_void()
{
	return true;
}

//////////////////// Begin PHPDoc_Classname_Type ///////////////////////

PHPDoc_Classname_Type::PHPDoc_Classname_Type()
{
	uses_serialized_resources = false;
	name = NULL;
}

PHPDoc_Classname_Type::PHPDoc_Classname_Type(char *cn)
{
	TSRMLS_FETCH(); // :(
	uses_serialized_resources = false;

	// we have to a bit of trickery here to resolve namespaces properly, as the
	// normal zend compiler handles them in a wonky way. Note that this doesn't
	// handle 'use' statements at all, since we can't retrospectively look at global
	// compiler state that has been wiped out. Bad stuff...

	// If this is a relative path that exists in a namespace, append the rest of the namespace to
	// its name. If this isn't a relative path or not in a (non-global) namespace, just use the name
	if ( ATCG(current_namespaced_entity) && cn[0] != '\\') {
		name = (char*) emalloc( ATCG(current_namespace_prefix_len) + strlen(cn) + 1 );
		strncpy(name, ATCG(current_namespaced_entity), ATCG(current_namespace_prefix_len));
		strcpy(name + ATCG(current_namespace_prefix_len), cn);
	} else {
		name = (char*) emalloc(strlen(cn) + 1);
		strcpy(name, cn);
	}
}

PHPDoc_Classname_Type::~PHPDoc_Classname_Type()
{
	if (!uses_serialized_resources) {
		efree((void*)name);
	}
}

bool PHPDoc_Classname_Type::matches(zval **val TSRMLS_DC)
{
	if (!val || !(*val)) {
		return false;
	}
	if (Z_TYPE_PP(val) != IS_OBJECT) {
		return false;
	}

	zend_class_entry **ce;
	if (zend_lookup_class(name, strlen(name), &ce TSRMLS_CC) == SUCCESS) {
		if (HAS_CLASS_ENTRY(**val) && instanceof_function(Z_OBJCE_PP(val), *ce TSRMLS_CC)) {
			return true;
		}
	}
	return false;
}

string PHPDoc_Classname_Type::typestr()
{
	return name;
}

zend_uchar PHPDoc_Classname_Type::get_type_uchar()
{
	return CLASSNAME_TYPE;
}

int PHPDoc_Classname_Type::serialize(zend_uchar *buf, bool writeMemory)
{
	if (writeMemory) {
		buf[0] = get_type_uchar();
		strcpy((char*)(buf + 1), name);
	}
	return strlen(name) + 2;
}

void PHPDoc_Classname_Type::deserialize(zend_uchar *buf)
{
	uses_serialized_resources = true;
	name = (char*)(buf + 1);
}

//////////////////// Begin PHPDoc_Mixed_Type ///////////////////////

bool PHPDoc_Mixed_Type::matches(zval **z TSRMLS_DC)
{
	// mixed cant be null or void
	if (!z || !(*z)) {
		return false;
	}
	return (Z_TYPE_PP(z) != IS_NULL);
}

string PHPDoc_Mixed_Type::typestr()
{
	return "mixed";
}

zend_uchar PHPDoc_Mixed_Type::get_type_uchar()
{
	return MIXED_TYPE;
}

//////////////////// Begin PHPDoc_Disjunctive_Type //////////////////////

PHPDoc_Disjunctive_Type::PHPDoc_Disjunctive_Type()
{
	left = NULL;
	right = NULL;
	uses_serialized_resources = false;
}

PHPDoc_Disjunctive_Type::PHPDoc_Disjunctive_Type(PHPDoc_Type * l, PHPDoc_Type * r)
{
	left = l;
	right = r;
	uses_serialized_resources = false;
}

PHPDoc_Disjunctive_Type::~PHPDoc_Disjunctive_Type()
{
	if (!uses_serialized_resources) {
		delete left;
		delete right;
	}
}

bool PHPDoc_Disjunctive_Type::contains_void()
{
	assert(!uses_serialized_resources);
	return (left->contains_void() || right->contains_void());
}

bool PHPDoc_Disjunctive_Type::verify_type(string* err_msg)
{
	assert(!uses_serialized_resources);

	// check left before right, because that likely will produce
	// the most intuitive error message
	if ( !left->verify_type(err_msg) ) {
		return false;
	}
	return right->verify_type(err_msg);
}

/**
 * We have to special case the deserialized case here in order to avoid
 * declaring cruft on the heap
 */
bool PHPDoc_Disjunctive_Type::matches(zval **z TSRMLS_DC)
{
	if (!uses_serialized_resources) {
		return left->matches(z TSRMLS_CC) || right->matches(z TSRMLS_CC);
	}

	char unused_buf[1];
	return fast_match_type((zend_uchar*) left, z, unused_buf, 1 TSRMLS_CC) ||
		fast_match_type((zend_uchar*) right, z, unused_buf, 1 TSRMLS_CC);
}

/**
 * Also have to special case the deserialized case here
 */
string PHPDoc_Disjunctive_Type::typestr()
{
	if (!uses_serialized_resources) {
		return left->typestr() + "|" + right->typestr();
	}

	// we don't need the typestr method to be super quick in the deserialized case,
	// because this isn't called in the critical checking phase except when an error
	// is thrown
	PHPDoc_Type *l = get_heap_alloced_type_from_buffer((zend_uchar*)left);
	PHPDoc_Type *r = get_heap_alloced_type_from_buffer((zend_uchar*)right);

	string typestr = l->typestr() + "|" + r->typestr();

	delete l;
	delete r;
	return typestr;
}

zend_uchar PHPDoc_Disjunctive_Type::get_type_uchar()
{
	return DISJUNCTIVE_TYPE;
}

/**
 * This is a fun case. A disjunctive type's serialization
 * looks like this:
 * [type_uchar | (int) offset to r | <serializaiton of l> | <serialization of r>]
 */
int PHPDoc_Disjunctive_Type::serialize(zend_uchar *buf, bool writeMemory)
{
	int l_offset = 1 + sizeof(int);
	if (writeMemory) {
		buf[0] = get_type_uchar();
	}

	int l_len = left->serialize(buf + l_offset, writeMemory);
	int r_offset = l_len + sizeof(int);
	int r_len = right->serialize(buf + l_offset + l_len, writeMemory);
	if (writeMemory) {
		memcpy((void*)(buf + 1), &r_offset, sizeof(int));
	}

	return 1 + sizeof(int) + l_len + r_len;
}

/**
 * This is another fun case. Set is deserialized to be true and
 * point left and right at the right things. The 'matches' instance
 * method will have to special case things too to avoid declaring
 * heap memory
 */
void PHPDoc_Disjunctive_Type::deserialize(zend_uchar *buf)
{
	uses_serialized_resources = true;
	left = (PHPDoc_Type *) (buf + 1 + sizeof(int));
	int r_offset = *((int*)(buf + 1));
	right = (PHPDoc_Type *) (buf + 1 + r_offset);
}

///////////////////// Begin PHPDoc_Variadic_Type /////////////////////

PHPDoc_Variadic_Type::PHPDoc_Variadic_Type()
{
	elemental_type = NULL;
	uses_serialized_resources = false;
}

PHPDoc_Variadic_Type::PHPDoc_Variadic_Type(PHPDoc_Type * e)
{
	elemental_type = e;
	uses_serialized_resources = false;
}

PHPDoc_Variadic_Type::~PHPDoc_Variadic_Type()
{
	if (!uses_serialized_resources) {
		delete elemental_type;
	}
}

bool PHPDoc_Variadic_Type::matches(zval **z TSRMLS_DC)
{
	if (!uses_serialized_resources) {
		// delegate directly to elemental
		return elemental_type->matches(z TSRMLS_CC);
	}

	char unused_buf[1];
	return fast_match_type((zend_uchar*) elemental_type, z, unused_buf, 1 TSRMLS_CC);
}

string PHPDoc_Variadic_Type::typestr()
{
	if (!uses_serialized_resources) {
		return "*" + elemental_type->typestr();
	}

	// we don't need the typestr method to be super quick in the deserialized case,
	// because this isn't called in the critical checking phase except when an error
	// is thrown. Put in parens around the contained type too for disambiguation
	PHPDoc_Type *elem = get_heap_alloced_type_from_buffer((zend_uchar*)elemental_type);
	string typestr = "*(" + elem->typestr() + ")";
	delete elem;
	return typestr;
}

zend_uchar PHPDoc_Variadic_Type::get_type_uchar()
{
	return VARIADIC_TYPE;
}

void PHPDoc_Variadic_Type::set_next(PHPDoc_Type * cannot_set)
{
	throw "Variadic arguments cannot be followed by more parameters.";
}

bool PHPDoc_Variadic_Type::is_variadic()
{
	return true;
}

bool PHPDoc_Variadic_Type::contains_void()
{
	assert(!uses_serialized_resources);
	return elemental_type->contains_void();
}

bool PHPDoc_Variadic_Type::verify_type(string* err_msg)
{
	assert(!uses_serialized_resources);
	return elemental_type->verify_type(err_msg);
}

/**
 * Serializes the variadiac type. Its memory looks like this:
 * [ type_uchar | <serialized elemental_type ]
 */
int PHPDoc_Variadic_Type::serialize(zend_uchar *buf, bool writeMemory)
{
	if (writeMemory) {
		buf[0] = get_type_uchar();
	}
	int type_len = elemental_type->serialize(buf + 1, writeMemory);
	return type_len + 1;
}

void PHPDoc_Variadic_Type::deserialize(zend_uchar *buf)
{
	uses_serialized_resources = true;
	elemental_type = (PHPDoc_Type *)(buf + 1);
}

//////////////////// Begin PHPDoc_ArrayOf_Type ///////////////////////

PHPDoc_ArrayOf_Type::PHPDoc_ArrayOf_Type()
{
	elemental_type = NULL;
	uses_serialized_resources = false;
}

PHPDoc_ArrayOf_Type::PHPDoc_ArrayOf_Type(PHPDoc_Type * e)
{
	elemental_type = e;
	uses_serialized_resources = false;
}

PHPDoc_ArrayOf_Type::~PHPDoc_ArrayOf_Type()
{
	if (!uses_serialized_resources) {
		delete elemental_type;
	}
}

bool PHPDoc_ArrayOf_Type::matches(zval **z TSRMLS_DC)
{
	if (!z || !(*z) || Z_TYPE_PP(z) != IS_ARRAY) {
		// no to nulls, voids and non-arrays
		DPRINTF("failed the null / void / array test\n");
		return false;
	}

	HashPosition pos;
	zval **entry;
	char unused_buf[1];

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(z), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(z), (void **)&entry, &pos) == SUCCESS) {

		if (!uses_serialized_resources && !elemental_type->matches(entry TSRMLS_CC)) {
			return false;
		}
		if (uses_serialized_resources && !fast_match_type((zend_uchar*) elemental_type, entry, unused_buf, 1 TSRMLS_CC)) {
			return false;
		}

		zend_hash_move_forward_ex(Z_ARRVAL_PP(z), &pos);
	}
	return true;
}

string PHPDoc_ArrayOf_Type::typestr()
{
	if (!uses_serialized_resources) {
		return elemental_type->typestr() + "[]";
	}

	// we don't need the typestr method to be super quick in the deserialized case,
	// because this isn't called in the critical checking phase except when an error
	// is thrown. Put in parens around the contained type too for disambiguation
	PHPDoc_Type *elem = get_heap_alloced_type_from_buffer((zend_uchar*)elemental_type);
	string typestr = "(" + elem->typestr() + ")[]";
	delete elem;
	return typestr;
}

zend_uchar PHPDoc_ArrayOf_Type::get_type_uchar()
{
	return ARRAYOF_TYPE;
}

bool PHPDoc_ArrayOf_Type::contains_void()
{
	assert(!uses_serialized_resources);
	return elemental_type->contains_void();
}

/**
 * The type contained in the Array must not contain void types!
 * Having an array of voids does not make sense
 */
bool PHPDoc_ArrayOf_Type::verify_type(string* err_msg)
{
	assert(!uses_serialized_resources);
	if (elemental_type->contains_void()) {
		(*err_msg) = "ArrayOf types cannot contain void!";
		return false;
	}
	return elemental_type->verify_type(err_msg);
}

/**
 * Serializes the arrayof type. Its memory looks like this:
 * [ type_uchar | <serialized elemental_type ]
 */
int PHPDoc_ArrayOf_Type::serialize(zend_uchar *buf, bool writeMemory)
{
	if (writeMemory) {
		buf[0] = get_type_uchar();
	}
	int type_len = elemental_type->serialize(buf + 1, writeMemory);
	return type_len + 1;
}

void PHPDoc_ArrayOf_Type::deserialize(zend_uchar *buf)
{
	uses_serialized_resources = true;
	elemental_type = (PHPDoc_Type *)(buf + 1);
}

//////////////////// Begin PHPDoc_UnsignedInt_Type ////////////////////////

bool PHPDoc_UnsignedInt_Type::matches(zval **z TSRMLS_DC)
{
	if (!z || !(*z)) {
		return false;
	}
	return Z_TYPE_PP(z) == IS_LONG && Z_LVAL_PP(z) >= 0;
}

string PHPDoc_UnsignedInt_Type::typestr()
{
	return "uint";
}

zend_uchar PHPDoc_UnsignedInt_Type::get_type_uchar()
{
	return UNSIGNEDINT_TYPE;
}

//////////////////// Begin PHPDoc_Function ///////////////////////

PHPDoc_Function::PHPDoc_Function(zend_literal *serialized_type_info)
{
	init_resources();
	deserialize(serialized_type_info);
}

PHPDoc_Function::PHPDoc_Function()
{
	init_resources();
}

void PHPDoc_Function::init_resources()
{
	first_parameter_type = NULL;
	return_type = NULL;
	error_str = "";
	uses_serialized_resources = false;
}

PHPDoc_Function::~PHPDoc_Function()
{
	if (first_parameter_type) {
		delete first_parameter_type;
	}
	if (return_type) {
		delete return_type;
	}
}

void PHPDoc_Function::set_error_string(string err)
{
	error_str = err;
}

string PHPDoc_Function::get_error_string()
{
	return error_str;
}

void PHPDoc_Function::set_parameters(PHPDoc_Type * first_parameter)
{
	first_parameter_type = first_parameter;
}

void PHPDoc_Function::set_return(PHPDoc_Type * ret_type)
{
	return_type = ret_type;
}

PHPDoc_Type *PHPDoc_Function::get_return()
{
	return return_type;
}

void PHPDoc_Function::enforce_argument_types(zval** params, uint nparams, const char *func_name TSRMLS_DC)
{
	// enforce_argument_types must _only_ be called on serialized functions
	assert( uses_serialized_resources );

	char *type_buffer_ptr = serialized_arg_types;
	char type_string_buf[64];
	char zval_typestr_buf[64];

	for (uint i = 0; i < nparams; i++) {
		zval **param = params + i;

		// if we have passed more arguments than doc specifications
		if (type_buffer_ptr == NULL) {
			get_zval_typestr(param, zval_typestr_buf, 64 TSRMLS_CC);
			zend_error(E_ERROR, "Too many arguments provided for function %s, was expecting nothing but got a %s\n",
					func_name, zval_typestr_buf);
		}
		// otherwise, ensure that the param's type matches our expectations.
		else if (!fast_match_type((zend_uchar *)(type_buffer_ptr + sizeof(int)), param, type_string_buf, 64 TSRMLS_CC)) {
			get_zval_typestr(param, zval_typestr_buf, 64 TSRMLS_CC);
			zend_error(E_ERROR, "Wrong type encountered for argument %u of function %s, was expecting a %s but got a %s\n",
					(i + 1), func_name, type_string_buf, zval_typestr_buf);
		}

		type_buffer_ptr = advance_type_pointer(type_buffer_ptr);
	}

	// This loop handles the case when no parameter was passed but
	// more types are declared in the phpdoc. This has the effect of enforcing
	// that |void is set on all optional arguments.
	// NOTE: the loop's conditional has the effect of ensuring that type_buffer_ptr is not null
	// and the type it points to is not variadic. You can't have optional args in a variadic type
	uint arg_num = nparams + 1;
	while (type_buffer_ptr && ( *((zend_uchar*)(type_buffer_ptr + sizeof(int))) != VARIADIC_TYPE )) {

		// only a void type will match a null pointer
		if (!fast_match_type((zend_uchar *)(type_buffer_ptr + sizeof(int)), NULL, type_string_buf, 64 TSRMLS_CC)) {
			zend_error(E_ERROR, "Wrong type encountered for argument %u of function %s, was expecting a %s but no argument was passed\n",
					arg_num, func_name, type_string_buf);
		}
		type_buffer_ptr = advance_type_pointer(type_buffer_ptr);
		arg_num++;
	}
}

char *PHPDoc_Function::advance_type_pointer(char *current)
{
	int next_offset = *((int*)current);
	zend_uchar this_type = *((zend_uchar*)(current + sizeof(int)));

	if (this_type == VARIADIC_TYPE) { // we don't move forward for variadic types
		return current;
	} else if (next_offset == 0) { // the end-of-list
		return NULL;
	}

	return (current + next_offset);
}

void PHPDoc_Function::enforce_return_type(zval** returned_value, const char *func_name TSRMLS_DC)
{
	char type_string_buf[64];
	char zval_typestr_buf[64];

	// enforce_return_type must _only_ be called on serialized functions
	assert( uses_serialized_resources );

	// if returned_value is null, that means the callee of this function
	// is not using the return value of the function. In this case, just bail
	// because nothing can be enforced
	if (!returned_value) {
		return;
	} else if (!(*returned_value)) {
		// TODO - understand this case better
		DPRINTF("*returned_value is NULL\n");
		return;
	}

	if (!fast_match_type((zend_uchar *)(serialized_return_type + sizeof(int)), returned_value, type_string_buf, 64 TSRMLS_CC)) {
		// Unfortunately, we have to special case something here. If the retval was supposed to be
		// void but someone in user-land php somehow used this function as if it was returning a
		// value, PHP will return a NULL-type zval. returned_value != NULL signifies this condition.
		// If the retval is used, the PHPDoc specified a void retval, and the actual zval returned is
		// null, return true
		// TODO should we even allow this? seems more appropriate to tell people not to do this..
		if (*(zend_uchar *)(serialized_return_type + sizeof(int)) == VOID_TYPE &&
				returned_value && *returned_value && Z_TYPE_PP(returned_value) == IS_NULL) {
			return;
		}

		get_zval_typestr(returned_value, zval_typestr_buf, 64 TSRMLS_CC);
		zend_error(E_ERROR, "Wrong type returned by function %s, was expecting a %s but got a %s\n",
				func_name, type_string_buf, zval_typestr_buf);
	}
}

void PHPDoc_Function::debug()
{
	PHPDoc_Type * para_type = first_parameter_type;

	while (para_type) {
		DPRINTF("@param %s\n", para_type->typestr().c_str());
		para_type = para_type->get_next();
	}
	PHPDoc_Type * ret_type = return_type;
	while (ret_type != NULL) {
		DPRINTF("@return %s\n", ret_type->typestr().c_str());
		ret_type = ret_type->get_next();
	}
}

zend_literal PHPDoc_Function::serialize()
{
	zend_literal l;
	int total_buf_len = 0;
	int metadata_size = sizeof(int);
	PHPDoc_Type *cur_type = first_parameter_type;

	// STAGE 1: Calculate the total length of the buffer
	total_buf_len += (metadata_size + return_type->serialize(NULL, false));
	while (cur_type) {
		total_buf_len += (metadata_size + cur_type->serialize(NULL, false));
		cur_type = cur_type->get_next();
	}

	// STAGE 2: emalloc the buffer and dump the serialized types in there
	zend_uchar *buf = (zend_uchar*) emalloc(total_buf_len + 1); // +1 for the trailing null terminator
	buf[total_buf_len] = 0;
	zend_uchar *loc = buf;
	int entry_len;

	// dump the retval in first
	entry_len = metadata_size + return_type->serialize(loc + metadata_size, true);
	if (!first_parameter_type) {
		// if there are no argument annotations, set the next pointer to 0 to signify that
		// there are no more entries
		entry_len = 0;
	}
	memcpy((void*) loc, (void*) &entry_len, metadata_size);
	loc += entry_len;

	// dump the arg vals in there now
	cur_type = first_parameter_type;
	while (cur_type) {
		entry_len = metadata_size + cur_type->serialize(loc + metadata_size, true);

		// populate this entry's pointer forwards. if there isn't a next entry,
		// zero out this pointer forwards
		if (cur_type->get_next() != NULL) {
			memcpy((void*) loc, (void*) &entry_len, metadata_size);
		} else {
			memset((void*) loc, 0, metadata_size);
		}
		loc += entry_len;
		cur_type = cur_type->get_next();
	}

	// STAGE 3: fixup the zend_literal
	// make the literal's zval into a string zval that points to buf
	l.constant.value.str.val = (char *)buf;
	l.constant.value.str.len = total_buf_len;
	l.constant.type = IS_STRING;
	l.constant.is_ref__gc = 0; // ?
	l.constant.refcount__gc = 1; // ?

	// set the remaining fields on the literal
	l.hash_value = 0; // TODO: not sure how to populate these two fields
	l.cache_slot = 0;
	return l;
}

void PHPDoc_Function::deserialize(zend_literal *literal)
{
	// this function now points to serialized type information!
	uses_serialized_resources = true;

	// pull out relevant info
	char* buf = literal->constant.value.str.val;
	int next_ptr = *((int*)buf);

	// the retval type pointer always points to the start of the zval's array
	serialized_return_type = buf;
	if (next_ptr == 0) {
		serialized_arg_types = NULL;
	} else {
		serialized_arg_types = buf + next_ptr;
	}
}

/**
 * This is general purpose function to verify the types of this function. If any type
 * is deemed unsuitable, then this will return false and dump an informative error
 * to the object's error string
 */
bool PHPDoc_Function::verify_types()
{
	assert(!uses_serialized_resources);
	string type_err_str;

	// all phpdoc functions _must_ have return types!
	if (!return_type) {
		error_str = "Return types must always be specified.";
		return false;
	}

	// enforce that return types be valid and not mix void and non-void values
	if (!return_type->verify_type(&type_err_str)) {
		error_str = "Error in return type: " + type_err_str;
		return false;
	}
	if (return_type->get_type_uchar() != VOID_TYPE && return_type->contains_void()) {
		error_str = "Return types may not mix void and non-void types.";
		return false;
	}

	// enforce that all argument types are valid
	int current_arg_num = 1;
	PHPDoc_Type *current_arg = first_parameter_type;
	while (current_arg) {
		if (!current_arg->verify_type(&type_err_str)) {
			stringstream ss;
			ss << "Error in type for argument " << current_arg_num << ": " << type_err_str;
			error_str = ss.str();
			return false;
		}

		current_arg = current_arg->get_next();
		current_arg_num++;
	}

	return true;
}
