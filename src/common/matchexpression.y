/*===============================================================
 * File: matchexpression.y
 *
 *  Grammar for match expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* Tokens */
%token STRINGLITERAL
%token OR_ AND_ NOT_   /* note: cannot use %left/%right because of implicit "or" operator */

// for bison 2.3
%pure_parser

// for bison 3.x
%lex-param {void *statePtr}
%parse-param {void *statePtr}

%start expression

%{

#include <stdio.h>
#include <stdlib.h>

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif

#include "matchexpression.h"
#include "matchexpressionlexer.h"
#include "patternmatcher.h"
#include "exception.h"

NAMESPACE_BEGIN

void yyerror (void *statePtr, const char *s);  // used by bison 3+
void yyerror (const char *s);  // used by bison 2.x

#define YYSTYPE  char *

typedef struct _MatchExpressionParserState {
    std::vector<MatchExpression::Elem> *elemsp;
    bool dottedpath;
    bool fullstring;
    bool casesensitive;
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
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::OR)); }
        | or_expr /*implicit-OR*/ and_expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::OR)); }
        | and_expr
        ;

and_expr
        : and_expr AND_ not_expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::AND)); }
        | not_expr
        ;

not_expr
        : NOT_ term
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::NOT)); }
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
                    PatternMatcher *p = new PatternMatcher();
                    p->setPattern($1, state.dottedpath, state.fullstring, state.casesensitive);
                    state.elemsp->push_back(MatchExpression::Elem(p));
                    delete [] $1;
                }
        | STRINGLITERAL '(' STRINGLITERAL ')'
                {
                    MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                    PatternMatcher *p = new PatternMatcher();
                    p->setPattern($3, state.dottedpath, state.fullstring, state.casesensitive);
                    state.elemsp->push_back(MatchExpression::Elem(p, $1));
                    delete [] $1;
                    delete [] $3;
                }
        ;

%%

//----------------------------------------------------------------------

void MatchExpression::parsePattern(std::vector<MatchExpression::Elem>& elems, const char *pattern,
                                   bool dottedpath, bool fullstring, bool casesensitive)
{
    MatchExpressionLexer *lexer = new MatchExpressionLexer(pattern);

    // store options
    MatchExpressionParserState state;
    state.elemsp = &elems;
    state.dottedpath = dottedpath;
    state.fullstring = fullstring;
    state.casesensitive = casesensitive;
    state.lexer = lexer;

    // parse
    yyparse(&state);
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

    throw opp_runtime_error("Error parsing match expression: %s", buf);
}

NAMESPACE_END