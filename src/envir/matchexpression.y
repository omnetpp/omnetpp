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

#define YYSTYPE  char *

#define yyin matchexpressionyyin
#define yyout matchexpressionyyout
#define yyrestart matchexpressionyyrestart
#define yy_scan_string matchexpressionyy_scan_string
#define yy_delete_buffer matchexpressionyy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (const char *s);

//#include "expryydefs.h"
#include "matchexpression.h"
#include "patternmatcher.h"
#include "cexception.h"

static struct MatchExpressionParserVars {
    std::vector<MatchExpression::Elem> *elemsp;
    bool dottedpath;
    bool fullstring;
    bool casesensitive;
} state;


%}

%%

expression
        : expr
        ;

expr
        : fieldpattern
        | '(' expr ')'
        | NOT_ expr
                { state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::NOT)); }
        | expr AND_ expr
                { state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::AND)); }
        | expr OR_ expr
                { state.elemsp->push_back(MatchExpression::Elem(MatchExpression::Elem::OR)); }
        ;

fieldpattern
        : STRINGLITERAL
                {
                    PatternMatcher *p = new PatternMatcher();
                    p->setPattern($1, state.dottedpath, state.fullstring, state.casesensitive);
                    state.elemsp->push_back(MatchExpression::Elem(p));
                }
        | STRINGLITERAL '(' STRINGLITERAL ')'
                {
                    PatternMatcher *p = new PatternMatcher();
                    p->setPattern($3, state.dottedpath, state.fullstring, state.casesensitive);
                    state.elemsp->push_back(MatchExpression::Elem(p, $1));
                }
        ;

%%

//----------------------------------------------------------------------

void MatchExpression::parsePattern(std::vector<MatchExpression::Elem>& elems, const char *pattern,
                                   bool dottedpath, bool fullstring, bool casesensitive)
{
    yyin = NULL;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(pattern);
    if (!handle)
        throw new cException("Error during match expression parsing: unable to allocate work memory");

    // store options
    state.elemsp = &elems;
    state.dottedpath = dottedpath;
    state.fullstring = fullstring;
    state.casesensitive = casesensitive;

    // parse
    int ret;
    try
    {
        ret = yyparse();
    }
    catch (cException *e)
    {
        yy_delete_buffer(handle);
        throw;
    }
    yy_delete_buffer(handle);
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw new cException("Error parsing match expression: %s", buf);
}

