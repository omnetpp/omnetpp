/*===============================================================
 * File: expr.y
 *
 *  Grammar for OMNeT++ NED-2 expressions.
 *
 *  Author: Andras Varga
 *
 *=============================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/* Reserved words */
%token DOUBLETYPE INTTYPE STRINGTYPE BOOLTYPE XMLTYPE
%token TRUE_ FALSE_ THIS_ DEFAULT_ CONST_ SIZEOF_ INDEX_ XMLDOC_

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

#include <stdio.h>
#include <stdlib.h>
#include "expryydefs.h"

#define YYDEBUG 1           /* allow debugging */
#define YYDEBUGGING_ON 0    /* turn on/off debugging */

#if YYDEBUG != 0
#define YYERROR_VERBOSE     /* more detailed error messages */
#include <string.h>         /* YYVERBOSE needs it */
#endif

#define yyin expryyin
#define yyout expryyout
#define yyrestart expryyrestart
#define yy_scan_string expryy_scan_string
#define yy_delete_buffer expryy_delete_buffer
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);
void yyrestart(FILE *);
int yylex();
void yyerror (const char *s);

LineColumn xpos, xprevpos;


#include "cdynamicexpression.h"
#include "cpar.h"
#include "cxmlelement.h"
#include "cexception.h"
#include "globals.h"
#include "cfunction.h"
#include "cnedfunction.h"
#include "nedsupport.h"
#include "stringutil.h"
#include "unitconversion.h"

USING_NAMESPACE

static cDynamicExpression::Elem *e;

static char *expryyconcat(char *s1, char *s2, char *s3=NULL)
{
    char *d = new char[strlen(s1)+strlen(s2)+strlen(s3?s3:"")+4];
    strcpy(d, s1);
    strcat(d, " ");
    strcat(d, s2);
    if (s3) {strcat(d, " "); strcat(d, s3);}
    delete [] s1; delete [] s2; delete [] s3;
    return d;
}

static void addFunction(const char *funcname, int argcount)
{
    cMathFunction *f = cMathFunction::find(funcname, argcount);
    if (f) {
        *e++ = f;
        return;
    }
    cNEDFunction *nf = cNEDFunction::find(funcname, argcount);
    if (nf) {
        (e++)->set(nf,argcount);
        return;
    }
    yyerror(opp_stringf("function `%s' with %d args not found (Define_NED_Function() missing from C++ code?)", funcname, argcount).c_str());
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
        | xmldocvalue
        | DEFAULT_ '(' expr ')'
                { yyerror("default() is not supported here"); }
        ;

xmldocvalue
        : XMLDOC_ '(' expr ')'
                { *e++ = new NEDSupport::XMLDoc(false); }
        | XMLDOC_ '(' expr ',' expr ')'
                { *e++ = new NEDSupport::XMLDoc(true); }
        ;

expr
        : simple_expr
        | '(' expr ')'
        | CONST_ '(' expr ')'
                { yyerror("const() is not supported here"); }

        | expr '+' expr
                { *e++ = cDynamicExpression::ADD; }
        | expr '-' expr
                { *e++ = cDynamicExpression::SUB; }
        | expr '*' expr
                { *e++ = cDynamicExpression::MUL; }
        | expr '/' expr
                { *e++ = cDynamicExpression::DIV; }
        | expr '%' expr
                { *e++ = cDynamicExpression::MOD; }
        | expr '^' expr
                { *e++ = cDynamicExpression::POW; }

        | '-' expr
                %prec UMIN_
                { *e++ = cDynamicExpression::NEG; }

        | expr EQ_ expr
                { *e++ = cDynamicExpression::EQ; }
        | expr NE_ expr
                { *e++ = cDynamicExpression::NE; }
        | expr '>' expr
                { *e++ = cDynamicExpression::GT; }
        | expr GE_ expr
                { *e++ = cDynamicExpression::GE; }
        | expr '<' expr
                { *e++ = cDynamicExpression::LT; }
        | expr LE_ expr
                { *e++ = cDynamicExpression::LE; }

        | expr AND_ expr
                { *e++ = cDynamicExpression::AND; }
        | expr OR_ expr
                { *e++ = cDynamicExpression::OR; }
        | expr XOR_ expr
                { *e++ = cDynamicExpression::XOR; }

        | NOT_ expr
                %prec UMIN_
                { *e++ = cDynamicExpression::NOT; }

        | expr BINAND_ expr
                { *e++ = cDynamicExpression::BIN_AND; }
        | expr BINOR_ expr
                { *e++ = cDynamicExpression::BIN_OR; }
        | expr BINXOR_ expr
                { *e++ = cDynamicExpression::BIN_XOR; }

        | BINCOMPL_ expr
                %prec UMIN_
                { *e++ = cDynamicExpression::BIN_NOT; }
        | expr SHIFTLEFT_ expr
                { *e++ = cDynamicExpression::LSHIFT; }
        | expr SHIFTRIGHT_ expr
                { *e++ = cDynamicExpression::RSHIFT; }
        | expr '?' expr ':' expr
                { *e++ = cDynamicExpression::IIF; }

        | INTTYPE '(' expr ')'
                { addFunction("int",1); }
        | DOUBLETYPE '(' expr ')'
                { addFunction("double",1); }
        | STRINGTYPE '(' expr ')'
                { addFunction("string",1); }

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
        : identifier
        | special_expr
        | literal
        ;

identifier
        : NAME
                { *e++ = new NEDSupport::ParameterRef($1, true, false); delete [] $1; }
        | THIS_ '.' NAME
                { *e++ = new NEDSupport::ParameterRef($3, false, true); delete [] $3; }
        | NAME '.' NAME
                { *e++ = new NEDSupport::SiblingModuleParameterRef($1, $3, true, false); delete [] $1; delete [] $3; }
        | NAME '[' expression ']' '.' NAME
                { *e++ = new NEDSupport::SiblingModuleParameterRef($1, $6, true, true); delete [] $1; delete [] $6; }
        ;

special_expr
        : INDEX_
                { *e++ = new NEDSupport::ModuleIndex(); }
        | INDEX_ '(' ')'
                { *e++ = new NEDSupport::ModuleIndex(); }
        | SIZEOF_ '(' NAME ')'
                { *e++ = new NEDSupport::Sizeof($3, true, false); delete [] $3; }
        | SIZEOF_ '(' THIS_ '.' NAME ')'
                { *e++ = new NEDSupport::Sizeof($5, false, false); delete [] $5; }
        | SIZEOF_ '(' NAME '.' NAME ')'
                { delete [] $3; delete [] $5; yyerror("sizeof(submodule.gate) notation not supported here"); }
        | SIZEOF_ '(' NAME '[' expression ']' '.' NAME ')'
                { delete [] $3; delete [] $8; yyerror("sizeof(submodule[index].gate) notation not supported here"); }
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
                { *e++ = strtol($1,NULL,10); delete [] $1; }
        | REALCONSTANT
                { *e++ = strtod($1,NULL); delete [] $1; }
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
                { $$ = expryyconcat($1,$2,$3); }
        | quantity REALCONSTANT NAME
                { $$ = expryyconcat($1,$2,$3); }
        | INTCONSTANT NAME
                { $$ = expryyconcat($1,$2); }
        | REALCONSTANT NAME
                { $$ = expryyconcat($1,$2); }
        ;

%%

//----------------------------------------------------------------------

void doParseExpression(const char *nedtext, cDynamicExpression::Elem *&elems, int& nelems)
{
    elems = NULL;
    nelems = 0;

    // reset the lexer
    xpos.co = 0;
    xpos.li = 1;
    xprevpos = xpos;

    yyin = NULL;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        throw std::runtime_error("parser is unable to allocate work memory");

    cDynamicExpression::Elem *v = new cDynamicExpression::Elem[100]; // overestimate for now; XXX danger of overrun
    e = v;

    // parse
    int ret;
    try
    {
        ret = yyparse();
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
    elems = new cDynamicExpression::Elem[nelems];
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

