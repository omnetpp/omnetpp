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
 *    non-component:   cplusplus
 *    component:       struct message class noncobject enum
 *    inheritance:     extends
 *    field types:     char short int long double unsigned
 *    field modifiers: abstract readonly
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
#include "msg2.tab.hh"

#define yylloc msg2yylloc
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

// wrap symbols to allow several .lex files coexist
#define comment     msgcomment
#define countChars  msgcount
#define extendCount msgextendCount

void comment();
void countChars();
void extendCount();

#define TEXTBUF_LEN 1024
static char textbuf[TEXTBUF_LEN];

USING_NAMESPACE

%}

%%
"//"                    { comment(); }

"namespace"             { countChars(); return NAMESPACE; }
"cplusplus"             { countChars(); return CPLUSPLUS; }
"struct"                { countChars(); return STRUCT; }
"message"               { countChars(); return MESSAGE; }
"class"                 { countChars(); return CLASS; }
"noncobject"            { countChars(); return NONCOBJECT; }
"enum"                  { countChars(); return ENUM; }
"extends"               { countChars(); return EXTENDS; }
"abstract"              { countChars(); return ABSTRACT; }
"readonly"              { countChars(); return READONLY; }
"properties"            { countChars(); return PROPERTIES; }
"fields"                { countChars(); return FIELDS; }

"bool"                  { countChars(); return BOOLTYPE; }
"char"                  { countChars(); return CHARTYPE; }
"short"                 { countChars(); return SHORTTYPE; }
"int"                   { countChars(); return INTTYPE; }
"long"                  { countChars(); return LONGTYPE; }
"double"                { countChars(); return DOUBLETYPE; }
"unsigned"              { countChars(); return UNSIGNED_; }
"string"                { countChars(); return STRINGTYPE; }
"true"                  { countChars(); return TRUE_; }
"false"                 { countChars(); return FALSE_; }

{L}({L}|{D})*           { countChars(); return NAME; }
{D}+                    { countChars(); return INTCONSTANT; }
0[xX]{X}+               { countChars(); return INTCONSTANT; }
{D}+{E}                 { countChars(); return REALCONSTANT; }
{D}*"."{D}+({E})?       { countChars(); return REALCONSTANT; }
\'[^\']\'               { countChars(); return CHARCONSTANT; }

\"                      { countChars(); BEGIN(stringliteral); }
<stringliteral>{
      \n                { BEGIN(INITIAL); throw NEDException("unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
      \\\n              { extendCount(); /* line continuation */ }
      \\\"              { extendCount(); /* qouted quote */ }
      \\[^\n\"]         { extendCount(); /* qouted char */ }
      [^\\\n\"]+        { extendCount(); /* character inside string literal */ }
      \"                { extendCount(); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

"{{"                    { countChars(); BEGIN(cplusplusbody); }
<cplusplusbody>"}}"     { extendCount(); BEGIN(INITIAL); return CPLUSPLUSBODY; }
<cplusplusbody>{S}      { extendCount(); }
<cplusplusbody>.        { extendCount(); }

";"                     { countChars(); return ';'; }
","                     { countChars(); return ','; }
":"                     { countChars(); return ':'; }
"="                     { countChars(); return '='; }
"("                     { countChars(); return '('; }
")"                     { countChars(); return ')'; }
"["                     { countChars(); return '['; }
"]"                     { countChars(); return ']'; }
"{"                     { countChars(); return '{'; }
"}"                     { countChars(); return '}'; }
"."                     { countChars(); return '.'; }
"?"                     { countChars(); return '?'; }
"@"                     { countChars(); return '@'; }

   /* FIXME are the next ones really needed? */

"||"                    { countChars(); return OR; }
"&&"                    { countChars(); return AND; }
"##"                    { countChars(); return XOR; }
"!"                     { countChars(); return NOT; }

"|"                     { countChars(); return BIN_OR; }
"&"                     { countChars(); return BIN_AND; }
"#"                     { countChars(); return BIN_XOR; }
"~"                     { countChars(); return BIN_COMPL; }
"<<"                    { countChars(); return SHIFT_LEFT; }
">>"                    { countChars(); return SHIFT_RIGHT; }

"^"                     { countChars(); return '^'; }
"+"                     { countChars(); return '+'; }
"-"                     { countChars(); return '-'; }
"*"                     { countChars(); return '*'; }
"/"                     { countChars(); return '/'; }
"%"                     { countChars(); return '%'; }
"<"                     { countChars(); return '<'; }
">"                     { countChars(); return '>'; }

"=="                    { countChars(); return EQ; }
"!="                    { countChars(); return NE; }
"<="                    { countChars(); return LE; }
">="                    { countChars(); return GE; }

"\ef\bb\bf"             { /* UTF-8 BOM mark, ignore */ }
{S}                     { countChars(); }
.                       { countChars(); return INVALID_CHAR; }

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

    /* printf("DBG: countChars(): prev=%d,%d  pos=%d,%d yytext=>>%s<<\n",
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

void countChars()
{
    _count(true);
}

void extendCount()
{
    _count(false);
}


