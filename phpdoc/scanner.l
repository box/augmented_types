%{
#include <stdio.h>
#include "phpdoc.h"
#include "parser.yacc.hpp"

#define yyterminate() \
do { phpdoc_initial_state_set = false; \
    return YY_NULL; \
    } while (0)

%}

%option noyywrap
%option prefix="phpdoc"

%x PHPDOC_COMMENT
%x PHPDOC_DECL
%s PHPDOC_START_EXPR
%x PHPDOC_END_EXPR

    bool phpdoc_initial_state_set = false;
%%

    /* Immediately start in the PHPDOC_COMMENT start state. All php doc PHPDOC_COMMENTs are English
    * until proven otherwise. */
    if (!phpdoc_initial_state_set) {
        BEGIN(PHPDOC_COMMENT);
        phpdoc_initial_state_set = true;
    }


    /* First we define possible phpdoc declarations. These can appear anywhere during the course
    * of parsing a regular php doc PHPDOC_COMMENT. */


<PHPDOC_END_EXPR,PHPDOC_COMMENT>@p(aram)?                    BEGIN(PHPDOC_DECL); return ANN_PARAM;
<PHPDOC_END_EXPR,PHPDOC_COMMENT>@r(eturn)?                   BEGIN(PHPDOC_DECL); return ANN_RETURN;
    /* <PHPDOC_END_EXPR,PHPDOC_COMMENT>@t(hrows)?            BEGIN(PHPDOC_DECL); return ANN_THROWS;*/
<PHPDOC_END_EXPR,PHPDOC_COMMENT>@v(ar)?                      BEGIN(PHPDOC_DECL); return ANN_VAR;
<PHPDOC_END_EXPR,PHPDOC_COMMENT>@m(ethod)?                   BEGIN(PHPDOC_DECL); return ANN_SIM_METHOD;
 
    /* We need to rely on whitespace in order to know when phpdoc expressions have terminated.
     * The first whitespace following the initial declaration moves the condition into the `PHPDOC_START_EXPR`
     * state. Any whitespace encountered during `PHPDOC_START_EXPR` change the state to `PHPDOC_END_EXPR`.
     * This implies that phpdoc expressions may not contain whitespace themselves.
     */

<PHPDOC_DECL>[ \t\r\n]+                           BEGIN(PHPDOC_START_EXPR);
<PHPDOC_START_EXPR>[ \t\r\n]+                     BEGIN(PHPDOC_END_EXPR);

    /* phpdoc expressions can only be matched when the PHPDOC_START_EXPR has begun*/

int(eger)?                                        return ANN_INT;
float|double|real                                 return ANN_FLOAT;
uint|unsigned_integer                             return ANN_UINT;
bool(ean)?                                        return ANN_BOOL;
callable                                          return ANN_CALLABLE;
numeric                                           return ANN_NUMERIC;
mixed                                             return ANN_MIXED;
array                                             return ANN_ARRAY;
null                                              return ANN_NULL;
void                                              return ANN_VOID;
object                                            return ANN_OBJECT;
string|varchar                                    return ANN_STRING;
resource                                          return ANN_RESOURCE;
\(                                                return OPAREN;
\)                                                return CPAREN;
\|                                                return VBAR;
\?                                                return OPTIONAL;
\[\]                                              return ARR_OF;
\[                                                return OBRACKET;
\]                                                return CBRACKET;
\{                                                return OBRACE;
\}                                                return CBRACE;
\<                                                return OGEN_OF;
\>                                                return CGEN_OF;
[a-zA-Z\_\\][a-zA-Z\_0-9\\]*                      phpdoclval.sval=phpdoctext; return NAME;
\*                                                return VARIA;
[^a-zA-Z]                                         { BEGIN(PHPDOC_COMMENT); return UNKNOWN; }

<PHPDOC_END_EXPR,PHPDOC_START_EXPR>\$[a-zA-Z\_][a-zA-Z\_0-9]*        {
                                                    BEGIN(PHPDOC_COMMENT);
                                                    phpdoclval.sval = phpdoctext;
                                                    return VAR_NAME;
                                                }
<PHPDOC_END_EXPR>[^\$][^ \t\n\r]*                BEGIN(PHPDOC_COMMENT);

    /* When in the `PHPDOC_COMMENT` state, we don't care about anything that doesn't look like the
     * beginning of a param declaration. Flex matches tokens by finding the pattern that
     * matches the largest amount of text. Ties are broken by the order they appear in this
     * file. That means we have to define individual rules for the `PHPDOC_COMMENT` state that never
     * exceed the possible match length of the phpdoc declaration rules above. So long as the
     * match length is a tie, param declarations will win, which is what we want. 
     */

<PHPDOC_COMMENT>[ \t\r\n]+                        /* ignore whitespace */
<PHPDOC_COMMENT>[^ \t\r\n]+                       /* ignore any contiguous non-ws text. */

%%
