/*==================================================
 * File: msg2.lex
 *
 *  Lexical analyser for OMNeT++ MSG-2.
 *
 *  Author: Andras Varga
 *
 ==================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/*
 * MSG-2 reserved words:
 *    non-component:   cplusplus import namespace using
 *    component:       struct message class noncobject enum
 *    inheritance:     extends
 *    field types:     char short int long double unsigned string const true false
 *    field modifiers: abstract
 */

D  [0-9]
L  [a-zA-Z_]
X  [0-9a-fA-F]
E  [Ee][+-]?{D}+
S  [ \t\v\n\r\f]

%x cplusplusbody
%x stringliteral
%x propertyname
%x afterpropertyname
%x propertyindex
%x propertyvalue

/* the following option keeps isatty() out */
%option never-interactive
%option nounistd

/*%option debug*/

%{
#include <cstring>
#include "yydefs.h"
#include "exception.h"
#include "msg2.tab.hh"

#define yylloc msg2yylloc
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

// wrap symbols to allow several .lex files coexist
#define comment     msgcomment
#define countChars  msgcount
#define extendCount msgextendCount
#define debugPrint  msgdebugPrint

void comment();
void countChars();
void extendCount();
int debugPrint(int c);

static int parenDepth = 0;

#define P(x)  (x)
//#define P(x)  debugPrint(x)  /*for debugging*/

using namespace omnetpp::nedxml;

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

%}

%%
"//"                     { comment(); }

"namespace"              { countChars(); return NAMESPACE; }
"using"                  { countChars(); return USING; /*reserved for future use*/ }
"cplusplus"              { countChars(); return CPLUSPLUS; }
"import"                 { countChars(); return IMPORT; }
"struct"                 { countChars(); return STRUCT; }
"message"                { countChars(); return MESSAGE; }
"packet"                 { countChars(); return PACKET; }
"class"                  { countChars(); return CLASS; }
"noncobject"             { countChars(); return NONCOBJECT; /*obsolete*/ }
"enum"                   { countChars(); return ENUM; }
"extends"                { countChars(); return EXTENDS; }
"abstract"               { countChars(); return ABSTRACT; }

"bool"                   { countChars(); return BOOL_; }
"char"                   { countChars(); return CHAR_; }
"short"                  { countChars(); return SHORT_; }
"int"                    { countChars(); return INT_; }
"long"                   { countChars(); return LONG_; }
"double"                 { countChars(); return DOUBLE_; }
"unsigned"               { countChars(); return UNSIGNED_; }
"const"                  { countChars(); return CONST_; /*reserved for future use*/ }
"string"                 { countChars(); return STRING_; }
"true"                   { countChars(); return TRUE_; }
"false"                  { countChars(); return FALSE_; }

{L}({L}|{D})*            { countChars(); return NAME; }
{D}+                     { countChars(); return INTCONSTANT; }
0[xX]{X}+                { countChars(); return INTCONSTANT; }
{D}+{E}                  { countChars(); return REALCONSTANT; }
{D}*"."{D}+({E})?        { countChars(); return REALCONSTANT; }
\'[^\']\'                { countChars(); return CHARCONSTANT; }

\"                       { countChars(); BEGIN(stringliteral); }
<stringliteral>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal"); }
    \\\n                 { extendCount(); /* line continuation */ }
    \\\"                 { extendCount(); /* qouted quote */ }
    \\[^\n\"]            { extendCount(); /* qouted char */ }
    [^\\\n\"]+           { extendCount(); /* character inside string literal */ }
    \"                   { extendCount(); if (parenDepth==0) BEGIN(INITIAL);else BEGIN(propertyvalue); return STRINGCONSTANT; /* closing quote */ }
}

"@"                      { countChars(); BEGIN(propertyname); return '@'; }
<propertyname>{
    ({L}|{D}|[:.-])+     { countChars(); BEGIN(afterpropertyname); return PROPNAME; }
    {S}                  { countChars(); }
    .                    { BEGIN(INITIAL); yyless(0); }
}

<afterpropertyname>{
    "["                  { countChars(); BEGIN(propertyindex); return P('['); }
    "("                  { countChars(); BEGIN(propertyvalue); parenDepth=1; return P('('); }
    {S}                  { countChars(); }
    .                    { BEGIN(INITIAL); yyless(0); }
}

<propertyindex>{
    "]"                  { countChars(); BEGIN(afterpropertyname); return P(']'); }
    ({L}|{D}|[:.-])+     { countChars(); return PROPNAME; }
    {S}                  { countChars(); }
    .                    { BEGIN(INITIAL); yyless(0); }
}

<propertyvalue>{
    [({[]                { countChars(); ++parenDepth; return P(COMMONCHAR); }
    [)}\]]               { countChars(); if (--parenDepth==0) {BEGIN(INITIAL); return P(yytext[0]);} else return P(COMMONCHAR); }
    "="                  { countChars(); return P(parenDepth==1 ? '=' : COMMONCHAR); }
    ","                  { countChars(); return P(parenDepth==1 ? ',' : COMMONCHAR); }
    ";"                  { countChars(); return P(parenDepth==1 ? ';' : COMMONCHAR); }
    \"                   { countChars(); BEGIN(stringliteral); }
    "\n"                 { countChars(); return P(COMMONCHAR); }
    .                    { countChars(); return P(COMMONCHAR); }
}

"{{"                     { countChars(); BEGIN(cplusplusbody); }
<cplusplusbody>{
    "}}"                 { extendCount(); BEGIN(INITIAL); return CPLUSPLUSBODY; }
    {S}                  { extendCount(); }
    .                    { extendCount(); }
}

";"                      { countChars(); return ';'; }
","                      { countChars(); return ','; }
":"                      { countChars(); return ':'; }
"="                      { countChars(); return '='; }
"("                      { countChars(); return '('; }
")"                      { countChars(); return ')'; }
"["                      { countChars(); return '['; }
"]"                      { countChars(); return ']'; }
"{"                      { countChars(); return '{'; }
"}"                      { countChars(); return '}'; }
"."                      { countChars(); return '.'; }
"?"                      { countChars(); return '?'; }

   /* XXX are the next ones really needed? */

"::"                     { countChars(); return DOUBLECOLON; }
"||"                     { countChars(); return OR; }
"&&"                     { countChars(); return AND; }
"##"                     { countChars(); return XOR; }
"!"                      { countChars(); return '!'; }

"|"                      { countChars(); return '|'; }
"&"                      { countChars(); return '&'; }
"#"                      { countChars(); return '#'; }
"~"                      { countChars(); return '~'; }
"<<"                     { countChars(); return SHIFT_LEFT; }
">>"                     { countChars(); return SHIFT_RIGHT; }

"^"                      { countChars(); return '^'; }
"+"                      { countChars(); return '+'; }
"-"                      { countChars(); return '-'; }
"*"                      { countChars(); return '*'; }
"/"                      { countChars(); return '/'; }
"%"                      { countChars(); return '%'; }
"<"                      { countChars(); return '<'; }
">"                      { countChars(); return '>'; }

"=="                     { countChars(); return EQ; }
"!="                     { countChars(); return NE; }
"<="                     { countChars(); return LE; }
">="                     { countChars(); return GE; }

"\xEF\xBB\xBF"           { /* UTF-8 BOM mark, ignore */ }
{S}                      { countChars(); }
.                        { countChars(); return INVALID_CHAR; }

%%

int yywrap()
{
     return 1;
}

/*
 * - discards all remaining characters of a line of
 *   text from the inputstream.
 * - the characters are read with the input() and
 *   unput() functions.
 * - input() is sometimes called yyinput()...
 */
#ifdef __cplusplus
#define input  yyinput
#endif

/* the following #define is needed for broken flex versions */
#define yytext_ptr yytext

void comment()
{
    int c;
    while ((c = input())!='\n' && c!=0 && c!=EOF);
    if (c=='\n') unput(c);
}

/*
 * - counts the line and column number of the current token in `pos'
 * - yytext[] is the current token passed by (f)lex
 */
static void _count(bool updateprevpos)
{
    int i;

    // printf("DBG: countChars(): prev=%d,%d  pos=%d,%d yytext=>>%s<<\n", prevpos.li, prevpos.co, pos.li, pos.co, yytext);

    if (pos.li==1 && pos.co==0)
        parenDepth = 0;

    if (updateprevpos)
        prevpos = pos;

    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            pos.co = 0;
            pos.li++;
        } else if (yytext[i] == '\t')
            pos.co += 8 - (pos.co % 8);
        else
            pos.co++;
    }

    // printf("count '%s': %d:%d-%d:%d\n", strcmp(yytext,"\n")==0?"\\n":yytext, prevpos.li, prevpos.co, pos.li, pos.co);
    yylloc.first_line   = prevpos.li;
    yylloc.first_column = prevpos.co;
    yylloc.last_line    = pos.li;
    yylloc.last_column  = pos.co;
}

void countChars()
{
    _count(true);
}

void extendCount()
{
    _count(false);
}

int debugPrint(int c)
{
    if (c==COMMONCHAR) fprintf(stderr, " ret=COMMONCHAR\n");
    else if (c==STRINGCONSTANT) fprintf(stderr, " ret=STRINGCONSTANT\n");
    else if (c>0 && c<=255) fprintf(stderr, " ret='%c'\n", c);
    else fprintf(stderr, " ret=%d\n", c);
    return c;
}
