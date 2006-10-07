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
%token TRUE_ FALSE_ THIS_ DEFAULT CONST_ SIZEOF INDEX_ XMLDOC

/* Other tokens: identifiers, numeric literals, operators etc */
%token NAME INTCONSTANT REALCONSTANT STRINGCONSTANT
%token EQ NE GE LE
%token AND OR XOR NOT
%token BIN_AND BIN_OR BIN_XOR BIN_COMPL
%token SHIFT_LEFT SHIFT_RIGHT

%token INVALID_CHAR   /* just to generate parse error */

/* Operator precedences (low to high) and associativity */
%left '?' ':'
%left AND OR XOR
%left EQ NE '>' GE '<' LE
%left BIN_AND BIN_OR BIN_XOR
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-'
%left '*' '/' '%'
%right '^'
%left UMIN NOT BIN_COMPL

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

#define yylloc expryylloc
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

#include "cdynamicexpression.h"
#include "cpar.h"
#include "cxmlelement.h"
#include "cexception.h"
#include "globals.h"
#include "cfunction.h"
#include "cnedfunction.h"

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

static void addFunction(const char *funcname, int numargs)
{
    cMathFunction *f = cMathFunction::find(funcname, numargs);
    if (f)
    {
        *e++ = f;
        return;
    }
    cNEDFunction *af = cNEDFunction::find(funcname, numargs);
    if (af)
    {
        *e++ = af;
        return;
    }
    yyerror((std::string("function `")+funcname+"' not found (Define_Function() missing from C++ code?)").c_str());
}

%}

%%

expression
        : expr
        | xmldocvalue
        | DEFAULT '(' expr ')'
                { yyerror("default() is not supported here"); }
        ;

xmldocvalue
        : XMLDOC '(' stringliteral ',' stringliteral ')'
                { *e++ = 1; /*FIXME TBD*/ }
        | XMLDOC '(' stringliteral ')'
                { *e++ = 1; /*FIXME TBD*/ }
        ;

expr
        : simple_expr
        | '(' expr ')'
        | CONST_ '(' expr ')'
                { yyerror("const() is not supported here"); }

        | expr '+' expr
                { *e++ = '+'; }
        | expr '-' expr
                { *e++ = '-'; }
        | expr '*' expr
                { *e++ = '*'; }
        | expr '/' expr
                { *e++ = '/'; }
        | expr '%' expr
                { *e++ = '%'; }
        | expr '^' expr
                { *e++ = '^'; }

        | '-' expr
                %prec UMIN
                { *e++ = 'M'; }

        | expr EQ expr
                { *e++ = '='; }
        | expr NE expr
                { *e++ = '!'; }
        | expr '>' expr
                { *e++ = '>'; }
        | expr GE expr
                { *e++ = '}'; }
        | expr '<' expr
                { *e++ = '<'; }
        | expr LE expr
                { *e++ = '{'; }

        | expr AND expr
                { *e++ = 'A'; }
        | expr OR expr
                { *e++ = 'O'; }
        | expr XOR expr
                { *e++ = 'X'; }

        | NOT expr
                %prec UMIN
                { *e++ = 'N'; }

        | expr BIN_AND expr
                { *e++ = '&'; }
        | expr BIN_OR expr
                { *e++ = '|'; }
        | expr BIN_XOR expr
                { *e++ = '#'; }

        | BIN_COMPL expr
                %prec UMIN
                { *e++ = '~'; }
        | expr SHIFT_LEFT expr
                { *e++ = 'L'; }
        | expr SHIFT_RIGHT expr
                { *e++ = 'R'; }
        | expr '?' expr ':' expr
                { *e++ = '?'; }

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
                { *e++ = 0; /*FIXME simulation.contextModule()->par($1)*/; delete [] $1; }
        | THIS_ '.' NAME
                { *e++ = 0; /*FIXME simulation.contextModule()->par($1)*/; delete [] $3; }
        | NAME '.' NAME
                { *e++ = 0; /*FIXME simulation.contextModule()->parentModule()->submodule($1)->par($1)*/; delete [] $1; delete [] $3; /*FIXME check for NULLs!!! */ }
        | NAME '[' expression ']' '.' NAME
                { delete [] $1; delete [] $6; yyerror("submodule[index] notation not supported here"); }
        ;

special_expr
        : INDEX_
                { *e++ = 0; /*FIXME simulation.contextModule()->index()*/ }
        | INDEX_ '(' ')'
                { *e++ = 0; /*FIXME simulation.contextModule()->index()*/ }
        | SIZEOF '(' identifier ')'
                { *e++ = 0; /*FIXME simulation.contextModule()->size()*/ }
        ;

literal
        : stringliteral
        | boolliteral
        | numliteral
        ;

stringliteral
        : STRINGCONSTANT
                { char *d; char *s = opp_parsequotedstr($1,d); *e++ = s; delete [] s; delete [] $1; }
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
                { *e++ = strToSimtime($1); delete [] $1; }
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
    pos.co = 0;
    pos.li = 1;
    prevpos = pos;

    yyin = NULL;
    yyout = stderr; // not used anyway

    // alloc buffer
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        throw new cRuntimeError("Error during expression parsing: unable to allocate work memory");

    cDynamicExpression::Elem *v = new cDynamicExpression::Elem[100]; // overestimate for now; XXX danger of overrun
    e = v;

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

    // copy v[] and return
    nelems = e - v;
    elems = new cDynamicExpression::Elem[nelems];
    for (int i=0; i<nelems; i++)
        elems[i] = v[i];
}

void yyerror(const char *s)
{
    // chop newline
    char buf[250];
    strcpy(buf, s);
    if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

    throw new cRuntimeError("Error parsing expression: %s", buf);
}

