/*==================================================
 * File: expr.lex
 *
 *  Lexical analyser for OMNeT++ NED-2 expressions.
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
 *    inheritance:   extends like withcppclass
 *    sections:      types parameters gates submodules connections allowunconnected
 *    param types:   double int string bool xml function
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
%option nounistd

%{
#include <cstring>
#include "omnetpp/cexception.h"
#include "expryydefs.h"
#include "expr.tab.hh"

using omnetpp::opp_strdup;

#define yylval expryylval
extern YYSTYPE yylval;

// wrap symbols to allow several .lex files coexist
#define countChars  exprcount
#define extendCount exprextendCount

void countChars();
void extendCount();

#define TEXTBUF_LEN 1024
static char textbuf[TEXTBUF_LEN];

// buffer to collect characters during extendCount()
static std::string extendbuf;

#include "omnetpp/simutil.h"  // opp_strdup()

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

%}

%%
"double"                 { countChars(); return DOUBLETYPE; }
"int"                    { countChars(); return INTTYPE; }
"string"                 { countChars(); return STRINGTYPE; }
"bool"                   { countChars(); return BOOLTYPE; }
"xml"                    { countChars(); return XMLTYPE; }

"true"                   { countChars(); return TRUE_; }
"false"                  { countChars(); return FALSE_; }
"nan"                    { countChars(); return NAN_; }
"inf"                    { countChars(); return INF_; }
"this"                   { countChars(); return THIS_; }
"default"                { countChars(); return DEFAULT_; }
"ask"                    { countChars(); return ASK_; }
"const"                  { countChars(); return CONST_; }
"sizeof"                 { countChars(); return SIZEOF_; }
"index"                  { countChars(); return INDEX_; }
"exists"                 { countChars(); return EXISTS; }
"typename"               { countChars(); return TYPENAME; }
"xmldoc"                 { countChars(); return XMLDOC_; }

{L}({L}|{D})*            { countChars(); yylval = opp_strdup(yytext); return NAME; }
{D}+                     { countChars(); yylval = opp_strdup(yytext); return INTCONSTANT; }
0[xX]{X}+                { countChars(); yylval = opp_strdup(yytext); return INTCONSTANT; }
{D}+{E}                  { countChars(); yylval = opp_strdup(yytext); return REALCONSTANT; }
{D}*"."{D}+({E})?        { countChars(); yylval = opp_strdup(yytext); return REALCONSTANT; }

\"                       { BEGIN(stringliteral); countChars(); }
<stringliteral>{
      \n                 { BEGIN(INITIAL); throw std::runtime_error("Unterminated string literal"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file will start from the <stringliteral> state! */  }
      \\\n               { extendCount(); /* line continuation */ }
      \\\"               { extendCount(); /* qouted quote */ }
      \\[^\n\"]          { extendCount(); /* qouted char */ }
      [^\\\n\"]+         { extendCount(); /* character inside string literal */ }
      \"                 { extendCount(); yylval = opp_strdup(extendbuf.c_str()); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

","                      { countChars(); return ','; }
":"                      { countChars(); return ':'; }
"="                      { countChars(); return '='; }
"("                      { countChars(); return '('; }
")"                      { countChars(); return ')'; }
"["                      { countChars(); return '['; }
"]"                      { countChars(); return ']'; }
"."                      { countChars(); return '.'; }
"?"                      { countChars(); return '?'; }

"||"                     { countChars(); return OR_; }
"&&"                     { countChars(); return AND_; }
"##"                     { countChars(); return XOR_; }
"!"                      { countChars(); return NOT_; }

"|"                      { countChars(); return BINOR_; }
"&"                      { countChars(); return BINAND_; }
"#"                      { countChars(); return BINXOR_; }
"~"                      { countChars(); return BINCOMPL_; }
"<<"                     { countChars(); return SHIFTLEFT_; }
">>"                     { countChars(); return SHIFTRIGHT_; }

"^"                      { countChars(); return '^'; }
"+"                      { countChars(); return '+'; }
"-"                      { countChars(); return '-'; }
"*"                      { countChars(); return '*'; }
"/"                      { countChars(); return '/'; }
"%"                      { countChars(); return '%'; }
"<"                      { countChars(); return '<'; }
">"                      { countChars(); return '>'; }

"=="                     { countChars(); return EQ_; }
"!="                     { countChars(); return NE_; }
"<="                     { countChars(); return LE_; }
">="                     { countChars(); return GE_; }

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

/*
 * - counts the line and column number of the current token in `pos'
 * - keeps a record of the complete current line in `textbuf[]'
 * - yytext[] is the current token passed by (f)lex
 */
static void _count(bool updateprevpos)
{
    static int textbuflen;
    int i;

    /* printf("DBG: countChars(): prev=%d,%d  xpos=%d,%d yytext=>>%s<<\n",
           xprevpos.li, xprevpos.co, xpos.li, xpos.co, yytext);
    */

    /* init textbuf */
    if (xpos.li==1 && xpos.co==0) {
        textbuf[0]='\0'; textbuflen=0;
    }

    if (updateprevpos) {
        extendbuf = "";
        xprevpos = xpos;
    }
    extendbuf += yytext;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            xpos.co = 0;
            xpos.li++;
            textbuflen=0; textbuf[0]='\0';
        } else if (yytext[i] == '\t')
            xpos.co += 8 - (xpos.co % 8);
        else
            xpos.co++;
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
}

void countChars()
{
    _count(true);
}

void extendCount()
{
    _count(false);
}

