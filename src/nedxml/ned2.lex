/*==================================================
 * File: ned2.lex
 *
 *  Lexical analyser for OMNeT++ NED-2.
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
 * NED-2 reserved words:
 *    non-component: import package property
 *    component:     module simple network channel interface
 *    inheritance:   extends like
 *    sections:      types parameters gates submodules connections allowunconnected
 *    param types:   double int string bool xml volatile
 *    gate types:    input output inout
 *    conditional:   if
 *    connections:   --> <-- <--> while ..
 *    expressions:   true false default const sizeof index xmldoc
 */

D  [0-9]
L  [a-zA-Z_\x80-\xff]
X  [0-9a-fA-F]
E  [Ee][+-]?{D}+
S  [ \t\v\n\r\f]

%x stringliteral
%x stringliteral2
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
#include "ned2.tab.hh"

#define yylloc ned2yylloc
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

// wrap symbols to allow several .lex files coexist
#define comment     ned2comment
#define countChars  ned2count
#define extendCount ned2extendCount
#define debugPrint  ned2debugPrint

void comment();
void countChars();
void extendCount();
int debugPrint(int c);

static int parenDepth = 0;

#define P(x)  (x)
//#define P(x)  debugPrint(x)  /*for debugging*/

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

using namespace omnetpp::nedxml;

%}

%%
"@expr@"                 { return EXPRESSION_SELECTOR; /* must agree with nedparser.cc */ }

"//"                     { comment(); }

"import"                 { countChars(); return IMPORT; }
"package"                { countChars(); return PACKAGE; }
"property"               { countChars(); return PROPERTY; }

"module"                 { countChars(); return MODULE; }
"simple"                 { countChars(); return SIMPLE; }
"network"                { countChars(); return NETWORK; }
"channel"                { countChars(); return CHANNEL; }
"moduleinterface"        { countChars(); return MODULEINTERFACE; }
"channelinterface"       { countChars(); return CHANNELINTERFACE; }

"extends"                { countChars(); return EXTENDS; }
"like"                   { countChars(); return LIKE; }

"types"                  { countChars(); return TYPES; }
"parameters"             { countChars(); return PARAMETERS; }
"gates"                  { countChars(); return GATES; }
"submodules"             { countChars(); return SUBMODULES; }
"connections"            { countChars(); return CONNECTIONS; }
"allowunconnected"       { countChars(); return ALLOWUNCONNECTED; }

"double"                 { countChars(); return DOUBLE_; }
"int"                    { countChars(); return INT_; }
"string"                 { countChars(); return STRING_; }
"bool"                   { countChars(); return BOOL_; }
"xml"                    { countChars(); return XML_; }
"object"                 { countChars(); return OBJECT_; }
"volatile"               { countChars(); return VOLATILE_; }

"input"                  { countChars(); return INPUT_; }
"output"                 { countChars(); return OUTPUT_; }
"inout"                  { countChars(); return INOUT_; }

"if"                     { countChars(); return IF; }
"for"                    { countChars(); return FOR; }

"-->"                    { countChars(); return RIGHTARROW; }
"<--"                    { countChars(); return LEFTARROW; }
"<-->"                   { countChars(); return DBLARROW; }
".."                     { countChars(); return TO; }
"::"                     { countChars(); return DOUBLECOLON; }

"true"                   { countChars(); return TRUE_; }
"false"                  { countChars(); return FALSE_; }
"nan"                    { countChars(); return NAN_; }
"inf"                    { countChars(); return INF_; }
"undefined"              { countChars(); return UNDEFINED_; }
"nullptr"                { countChars(); return NULLPTR_; }
"null"                   { countChars(); return NULL_; }
"this"                   { countChars(); return THIS_; }
"parent"                 { countChars(); return PARENT; }
"default"                { countChars(); return DEFAULT; }
"ask"                    { countChars(); return ASK; }
"const"                  { countChars(); return CONST_; }
"sizeof"                 { countChars(); return SIZEOF; }
"index"                  { countChars(); return INDEX_; }
"exists"                 { countChars(); return EXISTS; }
"typename"               { countChars(); return TYPENAME; }
"xmldoc"                 { countChars(); return XMLDOC; }

{L}({L}|{D})*            { countChars(); return NAME; }
{D}+                     { countChars(); return INTCONSTANT; }
0[xX]{X}+                { countChars(); return INTCONSTANT; }
{D}+{E}                  { countChars(); return REALCONSTANT; }
{D}*"."{D}+({E})?        { countChars(); return REALCONSTANT; }

\"                       { countChars(); BEGIN(stringliteral); }
<stringliteral>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
    \\\n                 { extendCount(); /* line continuation */ }
    \\\"                 { extendCount(); /* quoted quote */ }
    \\[^\n\"]            { extendCount(); /* quoted char */ }
    [^\\\n\"]+           { extendCount(); /* character inside string literal */ }
    \"                   { extendCount(); if (parenDepth==0) BEGIN(INITIAL);else BEGIN(propertyvalue); return STRINGCONSTANT; /* closing quote */ }
}

'                        { BEGIN(stringliteral2); countChars(); }
<stringliteral2>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral2> state! */ }
    \\\n                 { extendCount(); /* line continuation */ }
    \\\'                 { extendCount(); /* quoted apostrophe */ }
    \\[^\n\']            { extendCount(); /* quoted char */ }
    [^\\\n\']+           { extendCount(); /* character inside string literal */ }
    '                    { extendCount(); if (parenDepth==0) BEGIN(INITIAL);else BEGIN(propertyvalue); return STRINGCONSTANT; /* closing quote */ }
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
    ({L}|{D}|[*?{}:.-])+ { countChars(); return PROPNAME; }
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

"**"                     { countChars(); return DOUBLEASTERISK; }
"++"                     { countChars(); return PLUSPLUS; }

"$"                      { countChars(); return '$'; }
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
"<=>"                    { countChars(); return SPACESHIP; }
"=~"                     { countChars(); return MATCH; }

"\\xEF\\xBB\\xBF"           { /* UTF-8 BOM mark, ignore */ }
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
