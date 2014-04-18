%{
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "phpdoc.h"
using namespace std;

#define YYDEBUG 1

extern int phpdoclex();
extern FILE *phpdocin;

void yyerror(PHPDoc_Function** res, const char *str)
{
	//fprintf(stderr,"error: %s\n",str);
	(*res)->set_error_string(str);
}

int yywrap()
{
        return 1;
}

%}

%union {
        char *sval;
        PHPDoc_Type *ptype;
        PHPDoc_Function *pfunc;
}

        /*%define parse.error verbose*/
%error-verbose
        /*%define parse.lac full*/

%name-prefix = "phpdoc"
%parse-param {PHPDoc_Function ** result}

%token ANN_PARAM ANN_INT ANN_FLOAT ANN_UINT ANN_BOOL ANN_CALLABLE ANN_NUMERIC
%token ANN_MIXED ANN_ARRAY ANN_NULL ANN_VOID ANN_OBJECT ANN_STRING ANN_RESOURCE
%token ANN_VAR ANN_SIM_METHOD ANN_RETURN ANN_THROWS OPAREN CPAREN VBAR
%token NAME OPTIONAL ARR_OF OGEN_OF CGEN_OF VAR_NAME VARIA
%token OBRACE CBRACE OBRACKET CBRACKET UNKNOWN

%type <sval> NAME VAR_NAME
%type <ptype> phpdoc_expression param_declaration param_declarations return_declaration void_expression phpdoc_compound_expression
%type <pfunc> function_annotation

%%

function_annotation:
        param_declarations return_declaration
        {
                (*result)->set_parameters($1);
                (*result)->set_return($2);
        }
        |
        param_declarations
        {
                (*result)->set_parameters($1);
                (*result)->set_return(NULL);
        }
        |
        param_declarations return_declaration ANN_PARAM
        {
                delete $1;
                delete $2;
                yyerror(result, "@param declarations may not appear after the @return declaration.");
                YYERROR;
        }
        ;

param_declarations:
        {
                $$ = NULL;
        }
        |
        param_declarations param_declaration
        {
                if ($1) {
                        // could be better with saved PHPDoc_Type ** last
                        // pointer.
                        PHPDoc_Type *right_most = $1;
                        while (right_most->get_next()) {
                                right_most = right_most->get_next();
                        }
                        if (!right_most->is_variadic()) {
                                right_most->set_next($2);
                                $$ = $1;
                        }
                        else {
                                yyerror(result, "variadic arguments cannot be followed by regular arguments");
                                $$ = $1;
                        }
                }
                else {
                        $$ = $2;
                }

        }
        ;

param_declaration:
        ANN_PARAM VAR_NAME
        {
                // NOTE: Upon encountering invalid PHPDoc, we just return void types on error and
                // set the error string. This simplifies cleaning up memory after parsing fails
                yyerror(result, "You MUST specify a type for all parameters");
                $$ = new PHPDoc_Void_Type();
        }
        |
        ANN_PARAM UNKNOWN
        {
                yyerror(result, "Invalid parameter type - unknown character encountered");
                $$ = new PHPDoc_Void_Type();
        }
        |
        ANN_PARAM phpdoc_compound_expression
        {
                $$ = $2;
        }
        |
        ANN_PARAM phpdoc_compound_expression VAR_NAME
        {
                $$ = $2;
        }
        |
        ANN_PARAM VARIA phpdoc_compound_expression
        {
                $$ = new PHPDoc_Variadic_Type($3);
        }
        |
        ANN_PARAM VARIA phpdoc_compound_expression VAR_NAME
        {
                $$ = new PHPDoc_Variadic_Type($3);
        }
        ;

return_declaration:
        ANN_RETURN UNKNOWN
        {
                yyerror(result, "Invalid return type - unknown character encountered");
                $$ = new PHPDoc_Void_Type();
        }
        |
        ANN_RETURN phpdoc_compound_expression VAR_NAME
        {
                $$ = $2;
        }
        |
        ANN_RETURN phpdoc_compound_expression
        {
                $$ = $2;
        }
        ;

phpdoc_compound_expression:
        OPAREN phpdoc_compound_expression CPAREN
        {
                $$ = $2;
        }
        |
        phpdoc_compound_expression VBAR phpdoc_compound_expression
        {
                $$ = new PHPDoc_Disjunctive_Type($1, $3);
        }
        |
        phpdoc_compound_expression ARR_OF
        {
                $$ = new PHPDoc_ArrayOf_Type($1);
        }
        |
        void_expression
        |
        phpdoc_expression
        ;

phpdoc_expression:
        ANN_INT
        {
                $$ = new PHPDoc_Primitive_Type(IS_LONG);
        }
        |
        ANN_BOOL
        {
                $$ = new PHPDoc_Primitive_Type(IS_BOOL);
        }
        |
        ANN_STRING
        {
                $$ = new PHPDoc_Primitive_Type(IS_STRING);
        }
        |
        ANN_NULL
        {
                $$ = new PHPDoc_Primitive_Type(IS_NULL);
        }
        |
        ANN_ARRAY
        {
                $$ = new PHPDoc_Primitive_Type(IS_ARRAY);
        }
        |
        ANN_OBJECT
        {
                $$ = new PHPDoc_Primitive_Type(IS_OBJECT);
        }
        |
        ANN_FLOAT
        {
                $$ = new PHPDoc_Primitive_Type(IS_DOUBLE);
        }
        |
        NAME
        {
                $$ = new PHPDoc_Classname_Type($1);
        }
        |
        ANN_RESOURCE
        {
                $$ = new PHPDoc_Primitive_Type(IS_RESOURCE);
        }
        |
        ANN_MIXED
        {
                $$ = new PHPDoc_Mixed_Type();
        }
        |
        ANN_NUMERIC
        {
                $$ = new PHPDoc_Numeric_Type();
        }
        |
        ANN_CALLABLE
        {
                $$ = new PHPDoc_Callable_Type();
        }
        |
        ANN_UINT
        {
                $$ = new PHPDoc_UnsignedInt_Type();
        }
        ;

void_expression:
        ANN_VOID
        {
                $$ = new PHPDoc_Void_Type();
        }
        ;

%%

