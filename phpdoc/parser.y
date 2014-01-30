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
%type <ptype> phpdoc_expression param_declaration param_declarations return_declaration void_expression phpdoc_expressions_including_void phpdoc_expressions_excluding_void
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
        param_declarations return_declaration param_declarations
        {
                delete $1;
                delete $2;
                delete $3;
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
                                YYERROR;
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
                char errbuf[128];
                snprintf(errbuf, 128, "You MUST specify a type for variable %s", (char*)$2);
                yyerror(result, errbuf);
                YYERROR;
        }
        |
        ANN_PARAM phpdoc_expressions_including_void
        {
                $$ = $2;
        }
        |
        ANN_PARAM phpdoc_expressions_including_void VAR_NAME
        {
                $$ = $2;
        }
        |
        ANN_PARAM VARIA phpdoc_expressions_including_void
        {
                $$ = new PHPDoc_Variadic_Type($3);
        }
        |
        ANN_PARAM VARIA phpdoc_expressions_including_void VAR_NAME
        {
                $$ = new PHPDoc_Variadic_Type($3);
                // do something with the VAR_NAME
        }
        ;


return_declaration:
        ANN_RETURN void_expression
        {
                $$ = $2;
        }
        |
        ANN_RETURN phpdoc_expressions_excluding_void VAR_NAME
        {
                $$ = $2;
        }
        |
        ANN_RETURN phpdoc_expressions_excluding_void
        {
                $$ = $2;
        }
        ;

phpdoc_expressions_including_void:
        OPAREN phpdoc_expressions_including_void CPAREN
        {
                $$ = $2;
        }
        |
        phpdoc_expressions_including_void VBAR phpdoc_expressions_including_void
        {
                $$ = new PHPDoc_Disjunctive_Type($1, $3);
        }
        |
        phpdoc_expressions_including_void ARR_OF
        {
                $$ = new PHPDoc_ArrayOf_Type($1);
        }
        |
        void_expression
        |
        phpdoc_expression
        ;

phpdoc_expressions_excluding_void:
        OPAREN phpdoc_expressions_excluding_void CPAREN
        {
                $$ = $2;
        }
        |
        phpdoc_expressions_excluding_void VBAR phpdoc_expressions_excluding_void
        {
                $$ = new PHPDoc_Disjunctive_Type($1, $3);
        }
        |
        phpdoc_expressions_excluding_void ARR_OF
        {
                $$ = new PHPDoc_ArrayOf_Type($1);
        }
        |
        phpdoc_expression
        |
        void_expression
        {
                yyerror(result, "@return declarations may not mix void and values.");
                YYERROR;
        }
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

