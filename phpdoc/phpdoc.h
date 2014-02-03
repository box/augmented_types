
#include <vector>
extern "C" {
#include "php.h"
}
#include <string>

using namespace std;

// http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml#The__define_Guard
#ifndef AUGMENTED_TYPES_PHPDOC_PHPDOC_H_
#define AUGMENTED_TYPES_PHPDOC_PHPDOC_H_

#define AT_DEBUG 0
#define DPRINTF(fmt, ...) if(AT_DEBUG) {fprintf(stderr, fmt, ##__VA_ARGS__);}

/**
 * Represents a generic notion of an augmented type in PHP. Instances
 * of this class are able to answe whether passed zvals comply with their
 * respective types.
 */
class PHPDoc_Type
{

protected:
	/**
	 * In a sequence of types, this is the type that follows
	 * the current one. If this is the last type in the sequence,
	 * next is set to null.
	 */
	PHPDoc_Type * next;

public:

	PHPDoc_Type();

	virtual ~PHPDoc_Type() {
		if (next != NULL) {
			delete next;
		}
	};

	/**
	 * Determines whether or not the given zval matches
	 * the type specified by this instance of PHPDoc_Type.
	 * Matching is determined by sublcasses of PHPDoc_Type.
	 */
	virtual bool matches(zval **z TSRMLS_DC) = 0;

	/**
	 * In a sequence of types, this is the type that follows
	 * the current one. If this is the last type in the sequence,
	 * next is set to null.
	 */
	virtual void set_next(PHPDoc_Type * type) {
		next = type;
	};

	/**
	 * Gets the type that is currently assigned to next. May be NULL.
	 */
	virtual PHPDoc_Type * get_next() {
		return next;
	}

	/**
	 * Dumps an representation of what the expected
	 * compatible PHP type is and any relevant debugging
	 * information.
	 */
	virtual void debug() {
		DPRINTF("%s", typestr().c_str());
	};

	/**
	* Emits a friendly string representation of what this type expects
	* from userland.
	*/
	virtual string typestr() = 0;

	/**
	* Describes whether or not this argument is variadic.
	*/
	virtual bool is_variadic() {
		return false;
	}

	/**
	 * Describes whether this type contains the 'void' type or not. All types which
	 * do not contain other types and are not the void type itself should return false
	 */
	virtual bool contains_void() {
		return false;
	}

	/**
	 * Does any type-specific validation that should be checked at compile-time.
	 * Returns true if the type is valid. This will likely be a no-op for most types,
	 * but certain types need to perform specific post-parse-time validation (for example,
	 * the array_of type needs to verify that it doesn't contain a void type, because
	 * void types _cannot_ be in arrays).
	 */
	virtual bool verify_type(string* err_msg) {
		return true;
	}

	/**
	 * Returns this types identifying uchar. Used to identify subclasses of serialized
	 * phpdoc_types. This MUST be unique - see the defining macros in phpdoc.cc
	 */
	virtual zend_uchar get_type_uchar() = 0;

	/**
	 * Serializes this type. Must dump the following things in buf:
	 * [ type_uchar | any_arbitrary_thing]
	 * Also, this must return the amount of memory written to in buf
	 * In the case of writeMemory == true, actually commit this to memory,
	 * Otherwise, just return the number of bytes that would've been written
	 *
	 * Implemented below is the default serialize method of PHPDoc_Type subclasses
	 * that don't contain any state (instance vars) other than their subclass itself
	 */
	virtual int serialize(zend_uchar *buf, bool writeMemory) {
		if (writeMemory) {
			buf[0] = get_type_uchar();
		}
		return 1;
	}

	/**
	 * Deserializes this type. It will be passed the exact same piece
	 * of memory it dumped when serialize was called. Note that this method doesn't
	 * handle switching on the leading type_uchar
	 *
	 * Implemented below is the default deserialize method of PHPDoc_Type subclasses
	 * which don't have any state, which is a no-op
	 */
	virtual void deserialize(zend_uchar *buf) {
		return;
	}
};

class PHPDoc_Primitive_Type : public PHPDoc_Type
{
private:
	zend_uchar primitive_type;

public:
	PHPDoc_Primitive_Type(zend_uchar type);
	PHPDoc_Primitive_Type();
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
	int serialize(zend_uchar *buf, bool writeMemory);
	void deserialize(zend_uchar *buf);
};

class PHPDoc_Numeric_Type : public PHPDoc_Type
{
public:
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
};

class PHPDoc_Callable_Type : public PHPDoc_Type
{
public:
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
};

class PHPDoc_Void_Type : public PHPDoc_Type
{
public:
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
	bool contains_void();
};

class PHPDoc_Classname_Type : public PHPDoc_Type
{
private:
	char* name;
	bool uses_serialized_resources;

public:
	PHPDoc_Classname_Type(char *);
	PHPDoc_Classname_Type();
	~PHPDoc_Classname_Type();
	bool matches(zval **val TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
	int serialize(zend_uchar *buf, bool writeMemory);
	void deserialize(zend_uchar *buf);
};

class PHPDoc_Mixed_Type : public PHPDoc_Type
{
public:
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
};

class PHPDoc_Disjunctive_Type : public PHPDoc_Type
{
private:
	PHPDoc_Type * left;
	PHPDoc_Type * right;
	bool uses_serialized_resources;

public:
	PHPDoc_Disjunctive_Type(PHPDoc_Type * l, PHPDoc_Type * r);
	PHPDoc_Disjunctive_Type();
	~PHPDoc_Disjunctive_Type();
	bool matches(zval **z TSRMLS_DC);
	bool contains_void();
	bool verify_type(string* err_msg);
	string typestr();
	zend_uchar get_type_uchar();
	int serialize(zend_uchar *buf, bool writeMemory);
	void deserialize(zend_uchar *buf);
};

class PHPDoc_Variadic_Type : public PHPDoc_Type
{
private:
	PHPDoc_Type * elemental_type;
	bool uses_serialized_resources;

public:
	PHPDoc_Variadic_Type(PHPDoc_Type * e);
	PHPDoc_Variadic_Type();
	~PHPDoc_Variadic_Type();
	bool matches(zval **z TSRMLS_DC);
	bool contains_void();
	bool verify_type(string* err_msg);
	string typestr();
	zend_uchar get_type_uchar();
	void set_next(PHPDoc_Type * cannot_set);
	bool is_variadic();
	int serialize(zend_uchar *buf, bool writeMemory);
	void deserialize(zend_uchar *buf);
};

class PHPDoc_ArrayOf_Type : public PHPDoc_Type
{
private:
	PHPDoc_Type * elemental_type;
	bool uses_serialized_resources;

public:
	PHPDoc_ArrayOf_Type(PHPDoc_Type * e);
	PHPDoc_ArrayOf_Type();
	~PHPDoc_ArrayOf_Type();
	bool matches(zval **z TSRMLS_DC);
	bool contains_void();
	bool verify_type(string* err_msg);
	string typestr();
	zend_uchar get_type_uchar();
	int serialize(zend_uchar *buf, bool writeMemory);
	void deserialize(zend_uchar *buf);
};

class PHPDoc_UnsignedInt_Type : public PHPDoc_Type
{
public:
	bool matches(zval **z TSRMLS_DC);
	string typestr();
	zend_uchar get_type_uchar();
};

class PHPDoc_Function
{
public:

	PHPDoc_Function();
	PHPDoc_Function(zend_literal *serialized_type_info);
	~PHPDoc_Function();

	void set_parameters(PHPDoc_Type * first_parameter_type);
	void set_return(PHPDoc_Type * return_type);
	PHPDoc_Type *get_return();
	bool verify_types();

	void enforce_argument_types(zval** params, uint nparams, const char *func_name TSRMLS_DC);
	void enforce_return_type(zval** returned_value, const char *func_name TSRMLS_DC);
	void debug();
	void set_error_string(string err);
	string get_error_string();
	zend_literal serialize();
	void deserialize(zend_literal * literal);

private:
	PHPDoc_Type* first_parameter_type;
	PHPDoc_Type* return_type;

	char *serialized_return_type;
	char *serialized_arg_types;

	string error_str;
	bool uses_serialized_resources;
	char *advance_type_pointer(char *current);
	void init_resources();
};

#endif /* AUGMENTED_TYPES_PHPDOC_PHPDOC_H_ */
