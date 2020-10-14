/*===============================================================
 * File: matchexpression.y
 *
 *  Grammar for match expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* Tokens */
%token STRINGLITERAL 
%token MATCHES
%token OR_ AND_ NOT_   /* note: cannot use %left/%right because of implicit "or" operator */

// for bison 3.x
%define api.pure
%lex-param {void *statePtr}
%parse-param {void *statePtr}

%start expression

%{

#include <cstdio>
#include <cstdlib>

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>         /* YYVERBOSE needs it */
#endif

#include "matchexpression.h"
#include "matchexpressionlexer.h"
#include "patternmatcher.h"
#include "exception.h"

namespace omnetpp {
namespace common {

namespace expression { class ExprNode; }

void yyerror (void *statePtr, const char *s);  // used by bison 3+
void yyerror (const char *s);  // used by bison 2.x

#define YYSTYPE  char *

typedef struct _MatchExpressionParserState {
    std::vector<MatchExpression::Elem> elems;
    MatchExpressionLexer *lexer;
} MatchExpressionParserState;

#define YYPARSE_PARAM statePtr
#define YYLEX_PARAM statePtr
inline int matchexpressionyylex (YYSTYPE *yylval, void *statePtr)
{
    return ((MatchExpressionParserState*)statePtr)->lexer->getNextToken(yylval);
}

%}

%%

expression
        : or_expr
        ;

or_expr
        : or_expr OR_ and_expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elems.push_back(MatchExpression::Elem(MatchExpression::Elem::OR)); }
        | and_expr
        ;

and_expr
        : and_expr AND_ not_expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elems.push_back(MatchExpression::Elem(MatchExpression::Elem::AND)); }
        | not_expr
        ;

not_expr
        : NOT_ term
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elems.push_back(MatchExpression::Elem(MatchExpression::Elem::NOT)); }
        | term
        ;

term
        : '(' or_expr ')'
        | fieldpattern
        ;

fieldpattern
        : STRINGLITERAL
                {
                    MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                    state.elems.push_back(MatchExpression::Elem($1));
                    delete [] $1;
                }
        | STRINGLITERAL MATCHES STRINGLITERAL
                {
                    MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                    state.elems.push_back(MatchExpression::Elem($3, $1));
                    delete [] $1;
                    delete [] $3;
                }
        ;

%%

//----------------------------------------------------------------------

std::vector<MatchExpression::Elem>  MatchExpression::parsePattern(const char *pattern)
{
    MatchExpressionLexer lexer(pattern);

    // store options
    MatchExpressionParserState state;
    state.lexer = &lexer;

    // parse
    yyparse(&state);

    return state.elems;
}

// for bison 3.x
void yyerror(void *statePtr, const char *s)
{
    yyerror(s);
}

// for bison 2.x
void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw opp_runtime_error("Parse error in match expression: %s", buf);
}

} // namespace common
}  // namespace omnetpp
