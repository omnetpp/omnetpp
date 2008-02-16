/*==================================================
 * File: ned1.lex
 *
 *  Lexical analyser for OMNeT++ NED-1.
 *
 *  Author: Andras Varga
 *
 ==================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


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
#include "ned1.tab.h"

#define yylloc ned1yylloc
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

// wrap symbols to allow several .lex files coexist
#define comment     ned1comment
#define count       ned1count
#define extendCount ned1extendCount

void comment();
void count();
void extendCount();

#define TEXTBUF_LEN 1024
static char textbuf[TEXTBUF_LEN];

USING_NAMESPACE

%}

%%
"//"                    { comment(); }
"--"                    { comment(); }

"import"                { count(); return INCLUDE; }
"network"               { count(); return NETWORK; }
"module"                { count(); return MODULE; }
"simple"                { count(); return SIMPLE; }
"channel"               { count(); return CHANNEL; }
"for"                   { count(); return FOR; }
"do"                    { count(); return DO; }
"true"                  { count(); return TRUE_; }
"false"                 { count(); return FALSE_; }
"ref"                   { count(); return REF; }
"ancestor"              { count(); return ANCESTOR; }
"input"                 { count(); return INPUT_; }
"const"                 { count(); return CONSTDECL; }
"sizeof"                { count(); return SIZEOF; }
"endsimple"             { count(); return ENDSIMPLE; }
"endmodule"             { count(); return ENDMODULE; }
"endchannel"            { count(); return ENDCHANNEL; }
"endnetwork"            { count(); return ENDNETWORK; }
"endfor"                { count(); return ENDFOR; }
"parameters"            { count(); return PARAMETERS; }
"gates"                 { count(); return GATES; }
"gatesizes"             { count(); return GATESIZES; }
"in:"                   { count(); return IN; }
"out:"                  { count(); return OUT; }
"submodules"            { count(); return SUBMODULES; }
"connections"           { count(); return CONNECTIONS; }
"display"               { count(); return DISPLAY; }
"on"                    { count(); return ON; }
"like"                  { count(); return LIKE; }
"-->"                   { count(); return RIGHT_ARROW; }
"<--"                   { count(); return LEFT_ARROW; }
".."                    { count(); return TO; }
"..."                   { count(); return TO; }
"to"                    { count(); return TO; }
"if"                    { count(); return IF; }
"index"                 { count(); return SUBMODINDEX; }
"nocheck"               { count(); return NOCHECK; }
"xmldoc"                { count(); return XMLDOC; }

"numeric"               { count(); return NUMERICTYPE; }
"string"                { count(); return STRINGTYPE; }
"bool"                  { count(); return BOOLTYPE; }
"xml"                   { count(); return XMLTYPE; }
"anytype"               { count(); return ANYTYPE; }

"cplusplus"             { count(); return CPLUSPLUS; }
"struct"                { count(); return STRUCT; }
"message"               { count(); return MESSAGE; }
"class"                 { count(); return CLASS; }
"noncobject"            { count(); return NONCOBJECT; }
"enum"                  { count(); return ENUM; }
"extends"               { count(); return EXTENDS; }
"fields"                { count(); return FIELDS; }
"properties"            { count(); return PROPERTIES; }
"abstract"              { count(); return ABSTRACT; }
"readonly"              { count(); return READONLY; }

"char"                  { count(); return CHARTYPE; }
"short"                 { count(); return SHORTTYPE; }
"int"                   { count(); return INTTYPE; }
"long"                  { count(); return LONGTYPE; }
"double"                { count(); return DOUBLETYPE; }
"unsigned"              { count(); return UNSIGNED_; }

"delay"                 { count(); return CHANATTRNAME; }
"error"                 { count(); return CHANATTRNAME; }
"datarate"              { count(); return CHANATTRNAME; }

{L}({L}|{D})*           { count(); return NAME; }
{D}+                    { count(); return INTCONSTANT; }
0[xX]{X}+               { count(); return INTCONSTANT; }
{D}+{E}                 { count(); return REALCONSTANT; }
{D}*"."{D}+({E})?       { count(); return REALCONSTANT; }
\'[^\']\'               { count(); return CHARCONSTANT; }

\"                      { count(); BEGIN(stringliteral); }
<stringliteral>{
      \n                { BEGIN(INITIAL); throw NEDException("unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
      \\\n              { extendCount(); /* line continuation */ }
      \\\"              { extendCount(); /* qouted quote */ }
      \\[^\n\"]         { extendCount(); /* qouted char */ }
      [^\\\n\"]+        { extendCount(); /* character inside string literal */ }
      \"                { extendCount(); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

"{{"                    { count(); BEGIN(cplusplusbody); }
<cplusplusbody>"}}"     { extendCount(); BEGIN(INITIAL); return CPLUSPLUSBODY; }
<cplusplusbody>{S}      { extendCount(); }
<cplusplusbody>.        { extendCount(); }

"++"                    { count(); return PLUSPLUS; }

";"                     { count(); return ';'; }
","                     { count(); return ','; }
":"                     { count(); return ':'; }
"="                     { count(); return '='; }
"("                     { count(); return '('; }
")"                     { count(); return ')'; }
"["                     { count(); return '['; }
"]"                     { count(); return ']'; }
"{"                     { count(); return '{'; }
"}"                     { count(); return '}'; }
"."                     { count(); return '.'; }
"?"                     { count(); return '?'; }

"||"                    { count(); return OR; }
"&&"                    { count(); return AND; }
"##"                    { count(); return XOR; }
"!"                     { count(); return NOT; }

"|"                     { count(); return BIN_OR; }
"&"                     { count(); return BIN_AND; }
"#"                     { count(); return BIN_XOR; }
"~"                     { count(); return BIN_COMPL; }
"<<"                    { count(); return SHIFT_LEFT; }
">>"                    { count(); return SHIFT_RIGHT; }

"^"                     { count(); return '^'; }
"+"                     { count(); return '+'; }
"-"                     { count(); return '-'; }
"*"                     { count(); return '*'; }
"/"                     { count(); return '/'; }
"%"                     { count(); return '%'; }

"=="                    { count(); return EQ; }
"!="                    { count(); return NE; }
"<"                     { count(); return LS; }
"<="                    { count(); return LE; }
">"                     { count(); return GT; }
">="                    { count(); return GE; }

{S}                     { count(); }
.                       { count(); return INVALID_CHAR; }

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


