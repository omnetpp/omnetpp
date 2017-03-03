/*===============================================================
 * File: expression.y
 *
 *  Grammar for generic arithmetic expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* number of expected shift-reduce conflicts */
/*TODO this needs to be checked why: %expect 46 */

/* Reserved words */
%token DOUBLETYPE INTTYPE STRINGTYPE BOOLTYPE
%token TRUE_ FALSE_

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT
%token EQ_ NE_ GE_ LE_
%token AND_ OR_ XOR_ NOT_
%token BINAND_ BINOR_ BINXOR_ BINCOMPL_
%token SHIFTLEFT_ SHIFTRIGHT_

%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND_ OR_ XOR_
%left EQ_ NE_ '>' GE_ '<' LE_
%left BINAND_ BINOR_ BINXOR_
%left SHIFTLEFT_ SHIFTRIGHT_
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN_ NOT_ BINCOMPL_

%start expression

%{

#include <cstdio>
#include <cstdlib>
#include "commonutil.h"
#include "expressionyydefs.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <cstring>         /* YYVERBOSE needs it */
#endif

#include "expression.h"
#include "exception.h"
#include "stringutil.h"
#include "unitconversion.h"

using namespace omnetpp;
using namespace omnetpp::common;

#define yyin expressionyyin
#define yyout expressionyyout
#define yyrestart expressionyyrestart
#define yy_scan_string expressionyy_scan_string
#define yy_delete_buffer expressionyy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (const char *s);

LineColumn xpos, xprevpos;

static Expression::Elem *e;
static Expression::Resolver *resolver;

static char *join(const char *s1, const char *s2, const char *s3=nullptr)
{
    char *d = new char[strlen(s1) + strlen(s2) + strlen(s3?s3:"") + 4];
    strcpy(d, s1);
    strcat(d, " ");
    strcat(d, s2);
    if (s3) {strcat(d, " "); strcat(d, s3);}
    return d;
}

static char *concat(const char *s1, const char *s2, const char *s3=nullptr, const char *s4=nullptr)
{
    char *d = new char[strlen(s1) + strlen(s2) + strlen(s3?s3:"") + strlen(s4?s4:"") + 1];
    strcpy(d, s1);
    strcat(d, s2);
    if (s3) strcat(d, s3);
    if (s4) strcat(d, s4);
    return d;
}

static void addFunction(const char *funcname, int numargs)
{
    try {
        if (!resolver)
            throw opp_runtime_error("Cannot resolve function '%s': No resolver provided", funcname);
        Expression::Functor *f = resolver->resolveFunction(funcname, numargs);
        if (!f)
            throw opp_runtime_error("Cannot resolve function name '%s'", funcname);
        if (f->getNumArgs() != numargs)
            throw opp_runtime_error("Function %s() takes %d argument(s) and not %d", funcname, f->getNumArgs(), numargs);
        *e++ = f;
    }
    catch (std::exception& e) {
        yyerror(e.what());
    }
}

static void addVariableRef(const char *varname)
{
    try {
        if (!resolver)
            throw opp_runtime_error("Cannot resolve variable '%s': No resolver provided", varname);
        Expression::Functor *f = resolver->resolveVariable(varname);
        if (!f)
            throw opp_runtime_error("Cannot resolve variable '%s'", varname);
        if (f->getNumArgs() != 0)
            throw opp_runtime_error("Internal error: functor representing a variable cannot expect arguments: %s", varname);
        *e++ = f;
    }
    catch (std::exception& e) {
        yyerror(e.what());
    }
}

static double parseQuantity(const char *text, std::string& unit)
{
    try {
        // evaluate quantities like "5s 230ms"
        return UnitConversion::parseQuantity(text, unit);
    }
    catch (std::exception& e) {
        yyerror(e.what());
        return 0;
    }
}

%}

%%

expression
        : expr
        ;

expr
        : simple_expr
        | '(' expr ')'

        | expr '+' expr
                { *e++ = Expression::ADD; }
        | expr '-' expr
                { *e++ = Expression::SUB; }
        | expr '*' expr
                { *e++ = Expression::MUL; }
        | expr '/' expr
                { *e++ = Expression::DIV; }
        | expr '%' expr
                { *e++ = Expression::MOD; }
        | expr '^' expr
                { *e++ = Expression::POW; }

        | '-' expr
                %prec UMIN_
                { *e++ = Expression::NEG; }

        | expr '=' expr
                { *e++ = Expression::EQ; }
        | expr EQ_ expr
                { *e++ = Expression::EQ; }
        | expr NE_ expr
                { *e++ = Expression::NE; }
        | expr '>' expr
                { *e++ = Expression::GT; }
        | expr GE_ expr
                { *e++ = Expression::GE; }
        | expr '<' expr
                { *e++ = Expression::LT; }
        | expr LE_ expr
                { *e++ = Expression::LE; }

        | expr AND_ expr
                { *e++ = Expression::AND; }
        | expr OR_ expr
                { *e++ = Expression::OR; }
        | expr XOR_ expr
                { *e++ = Expression::XOR; }

        | NOT_ expr
                %prec UMIN_
                { *e++ = Expression::NOT; }

        | expr BINAND_ expr
                { *e++ = Expression::BIN_AND; }
        | expr BINOR_ expr
                { *e++ = Expression::BIN_OR; }
        | expr BINXOR_ expr
                { *e++ = Expression::BIN_XOR; }

        | BINCOMPL_ expr
                %prec UMIN_
                { *e++ = Expression::BIN_NOT; }
        | expr SHIFTLEFT_ expr
                { *e++ = Expression::LSHIFT; }
        | expr SHIFTRIGHT_ expr
                { *e++ = Expression::RSHIFT; }
        | expr '?' expr ':' expr
                { *e++ = Expression::IIF; }

        | NAME '(' ')'
                { addFunction($1,0); delete [] $1; }
        | NAME '(' expr ')'
                { addFunction($1,1); delete [] $1; }
        | NAME '(' expr ',' expr ')'
                { addFunction($1,2); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ')'
                { addFunction($1,3); delete [] $1; }
        | NAME '(' expr ',' expr ',' expr ',' expr ')'
                { addFunction($1,4); delete [] $1; }
         ;

simple_expr
        : variable
        | literal
        ;

variable
        : variable2
                { addVariableRef($1); delete [] $1; }
        ;

variable2
        : variable2 '.' segment
                { $$ = concat($1, ".", $3); delete [] $1; delete [] $3; }
        | segment
        ;

segment
        : NAME
        | NAME '[' INTCONSTANT ']'
                { $$ = concat($1, "[", $3, "]"); delete [] $1; delete [] $3; }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { *e++ = opp_parsequotedstr($1).c_str(); delete [] $1; }
        ;

boolliteral
        : TRUE_
                { *e++ = true; }
        | FALSE_
                { *e++ = false; }
        ;

numliteral
        : INTCONSTANT
                { *e++ = opp_atol($1); delete [] $1; }
        | REALCONSTANT
                { *e++ = opp_atof($1); delete [] $1; }
        | quantity
                {
                  std::string unit;
                  *e++ = parseQuantity($1, unit);
                  if (!unit.empty())
                      (e-1)->setUnit(unit.c_str());
                  delete [] $1;
                }
        ;

quantity
        : quantity INTCONSTANT NAME
                { $$ = join($1, $2, $3); delete [] $1; delete [] $2; delete [] $3; }
        | quantity REALCONSTANT NAME
                { $$ = join($1, $2, $3); delete [] $1; delete [] $2; delete [] $3; }
        | INTCONSTANT NAME
                { $$ = join($1, $2); delete [] $1; delete [] $2; }
        | REALCONSTANT NAME
                { $$ = join($1, $2); delete [] $1; delete [] $2; }
        ;

%%

//----------------------------------------------------------------------

void doParseExpression(const char *text, Expression::Resolver *res, Expression::Elem *&elems, int& nelems)
{
    NONREENTRANT_PARSER();

    elems = nullptr;
    nelems = 0;

    // reset the lexer
    xpos.co = 0;
    xpos.li = 1;
    xprevpos = xpos;

    yyin = nullptr;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(text);
    if (!handle)
        throw std::runtime_error("Parser is unable to allocate work memory");

    Expression::Elem *v = new Expression::Elem[100]; // overestimate for now; XXX danger of overrun
    e = v;
    resolver = res;

    // parse
    try
    {
        yyparse();
    }
    catch (std::exception& e)
    {
        yy_delete_buffer(handle);
        delete [] v;
        throw;
    }
    yy_delete_buffer(handle);

    // copy v[] and return
    nelems = e - v;
    elems = new Expression::Elem[nelems];
    for (int i=0; i<nelems; i++)
        elems[i] = v[i];
    delete [] v;
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw std::runtime_error(buf);
}
