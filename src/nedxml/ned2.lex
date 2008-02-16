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
L  [a-zA-Z_]
X  [0-9a-fA-F]
E  [Ee][+-]?{D}+
S  [ \t\v\n\r\f]

%x cplusplusbody
%x stringliteral

/* the following option keeps isatty() out */
%option never-interactive

%{
#include <string.h>
#include "nedyydefs.h"
#include "nedexception.h"
#include "ned2.tab.h"

#define yylloc ned2yylloc
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

// wrap symbols to allow several .lex files coexist
#define comment     ned2comment
#define count       ned2count
#define extendCount ned2extendCount

void comment();
void count();
void extendCount();

#define TEXTBUF_LEN 1024
static char textbuf[TEXTBUF_LEN];

USING_NAMESPACE

%}

%%
"@expr@"                 { return EXPRESSION_SELECTOR; /* must agree with nedparser.cc */ }

"//"                     { comment(); }

"import"                 { count(); return IMPORT; }
"package"                { count(); return PACKAGE; }
"property"               { count(); return PROPERTY; }

"module"                 { count(); return MODULE; }
"simple"                 { count(); return SIMPLE; }
"network"                { count(); return NETWORK; }
"channel"                { count(); return CHANNEL; }
"moduleinterface"        { count(); return MODULEINTERFACE; }
"channelinterface"       { count(); return CHANNELINTERFACE; }

"extends"                { count(); return EXTENDS; }
"like"                   { count(); return LIKE; }

"types"                  { count(); return TYPES; }
"parameters"             { count(); return PARAMETERS; }
"gates"                  { count(); return GATES; }
"submodules"             { count(); return SUBMODULES; }
"connections"            { count(); return CONNECTIONS; }
"allowunconnected"       { count(); return ALLOWUNCONNECTED; }

"double"                 { count(); return DOUBLETYPE; }
"int"                    { count(); return INTTYPE; }
"string"                 { count(); return STRINGTYPE; }
"bool"                   { count(); return BOOLTYPE; }
"xml"                    { count(); return XMLTYPE; }
"volatile"               { count(); return VOLATILE; }
"typename"               { count(); return TYPENAME; }

"input"                  { count(); return INPUT_; }
"output"                 { count(); return OUTPUT_; }
"inout"                  { count(); return INOUT_; }

"if"                     { count(); return IF; }
"for"                    { count(); return FOR; }

"-->"                    { count(); return RIGHTARROW; }
"<--"                    { count(); return LEFTARROW; }
"<-->"                   { count(); return DBLARROW; }
".."                     { count(); return TO; }

"true"                   { count(); return TRUE_; }
"false"                  { count(); return FALSE_; }
"this"                   { count(); return THIS_; }
"default"                { count(); return DEFAULT; }
"const"                  { count(); return CONST_; }
"sizeof"                 { count(); return SIZEOF; }
"index"                  { count(); return INDEX_; }
"xmldoc"                 { count(); return XMLDOC; }

{L}({L}|{D})*            { count(); return NAME; }
{D}+                     { count(); return INTCONSTANT; }
0[xX]{X}+                { count(); return INTCONSTANT; }
{D}+{E}                  { count(); return REALCONSTANT; }
{D}*"."{D}+({E})?        { count(); return REALCONSTANT; }

\"                       { count(); BEGIN(stringliteral); }
<stringliteral>{
      \n                 { BEGIN(INITIAL); throw NEDException("unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
      \\\n               { extendCount(); /* line continuation */ }
      \\\"               { extendCount(); /* qouted quote */ }
      \\[^\n\"]          { extendCount(); /* qouted char */ }
      [^\\\n\"]+         { extendCount(); /* character inside string literal */ }
      \"                 { extendCount(); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

"**"                     { count(); return DOUBLEASTERISK; }
"++"                     { count(); return PLUSPLUS; }

"$"                      { count(); return '$'; }
"@"                      { count(); return '@'; }
";"                      { count(); return ';'; }
","                      { count(); return ','; }
":"                      { count(); return ':'; }
"="                      { count(); return '='; }
"("                      { count(); return '('; }
")"                      { count(); return ')'; }
"["                      { count(); return '['; }
"]"                      { count(); return ']'; }
"{"                      { count(); return '{'; }
"}"                      { count(); return '}'; }
"."                      { count(); return '.'; }
"?"                      { count(); return '?'; }

"||"                     { count(); return OR; }
"&&"                     { count(); return AND; }
"##"                     { count(); return XOR; }
"!"                      { count(); return NOT; }

"|"                      { count(); return BIN_OR; }
"&"                      { count(); return BIN_AND; }
"#"                      { count(); return BIN_XOR; }
"~"                      { count(); return BIN_COMPL; }
"<<"                     { count(); return SHIFT_LEFT; }
">>"                     { count(); return SHIFT_RIGHT; }

"^"                      { count(); return '^'; }
"+"                      { count(); return '+'; }
"-"                      { count(); return '-'; }
"*"                      { count(); return '*'; }
"/"                      { count(); return '/'; }
"%"                      { count(); return '%'; }
"<"                      { count(); return '<'; }
">"                      { count(); return '>'; }

"=="                     { count(); return EQ; }
"!="                     { count(); return NE; }
"<="                     { count(); return LE; }
">="                     { count(); return GE; }

{S}                      { count(); }
.                        { count(); return INVALID_CHAR; }

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
 * - keeps a record of the complete current line in `textbuf[]'
 * - yytext[] is the current token passed by (f)lex
 */
static void _count(bool updateprevpos)
{
    static int textbuflen;
    int i;

    /* printf("DBG: count(): prev=%d,%d  pos=%d,%d yytext=>>%s<<\n",
           prevpos.li, prevpos.co, pos.li, pos.co, yytext);
    */

    /* init textbuf */
    if (pos.li==1 && pos.co==0) {
        textbuf[0]='\0'; textbuflen=0;
    }

    if (updateprevpos) {
        prevpos = pos;
    }
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            pos.co = 0;
            pos.li++;
            textbuflen=0; textbuf[0]='\0';
        } else if (yytext[i] == '\t')
            pos.co += 8 - (pos.co % 8);
        else
            pos.co++;
        if (yytext[i] != '\n') {
            if (textbuflen < TEXTBUF_LEN-5) {
                textbuf[textbuflen++]=yytext[i]; textbuf[textbuflen]='\0';
            }
            else if (textbuflen == TEXTBUF_LEN-5) {
                strcpy(textbuf+textbuflen, "...");
                textbuflen++;
            } else {
                /* line too long -- ignore */
            }
        }
    }
    /* printf("li=%d co=%d\n", pos.li, pos.co); good for debugging... */
    yylloc.first_line   = prevpos.li;
    yylloc.first_column = prevpos.co;
    yylloc.last_line    = pos.li;
    yylloc.last_column  = pos.co;
}

void count()
{
    _count(true);
}

void extendCount()
{
    _count(false);
}


