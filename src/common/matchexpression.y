/*===============================================================
 * File: matchexpression.y
 *
 *  Grammar for match expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* Tokens */
%token STRINGLITERAL

/* Operator precedences (low to high) and associativity */
%left OR_
%left AND_
%left NOT_

%pure_parser

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

void yyerror (const char *s);

#include "matchexpression.h"
#include "matchexpressionlexer.h"
#include "patternmatcher.h"
#include "exception.h"

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


using OPP::MatchExpression;
using OPP::PatternMatcher;
using OPP::opp_runtime_error;

%}

%%

expression
        : expr
        ;

expr
        : fieldpattern
        | '(' expr ')'
        | NOT_ expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::NOT)); }
        | expr AND_ expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::AND)); }
        | expr OR_ expr
                { MatchExpressionParserState &state = *(MatchExpressionParserState*)statePtr;
                  state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::OR)); }
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

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw opp_runtime_error("Error parsing match expression: %s", buf);
}

