/*==================================================
 * File: msg2.lex
 *
 *  Lexical analyser for OMNeT++ MSG-2.
 *
 *  Author: Andras Varga
 *
 ==================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

%option noyywrap 8bit reentrant
%option bison-bridge bison-locations
%option never-interactive nounistd

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

%{
#include <cstring>
#include "yydefs.h"
#include "msg2.tab.h"
#include "exception.h"

using namespace omnetpp::nedxml;

static int parenDepth = 0;

#define YY_USER_INIT  {parenDepth = 0; pos = {1,0}; yylloc->first_line = yylloc->last_line = 1; yylloc->first_column = yylloc->last_column = 0;}

static void msg2count(const char *text, YYLTYPE *loc, bool extend);

#define COUNT()  msg2count(yytext, yylloc, false)
#define EXTEND() msg2count(yytext, yylloc, true)

#define P(x)  (x)
//#define P(x) (printf("lexer: token %s '%s' at %d:%d..%d.%d, parenDepth=%d\n", #x, yytext, yylloc->first_line, yylloc->first_column, yylloc->last_line, yylloc->last_column, parenDepth), (x))

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

%}

%%
"//"                     {
                            int c;
                            while ((c = yyinput(yyscanner))!='\n' && c!=0 && c!=EOF);
                            if (c=='\n') unput(c);
                         }

"namespace"              { COUNT(); return P(NAMESPACE); }
"using"                  { COUNT(); return P(USING); /*reserved for future use*/ }
"cplusplus"              { COUNT(); return P(CPLUSPLUS); }
"import"                 { COUNT(); return P(IMPORT); }
"struct"                 { COUNT(); return P(STRUCT); }
"message"                { COUNT(); return P(MESSAGE); }
"packet"                 { COUNT(); return P(PACKET); }
"class"                  { COUNT(); return P(CLASS); }
"noncobject"             { COUNT(); return P(NONCOBJECT); /*obsolete*/ }
"enum"                   { COUNT(); return P(ENUM); }
"extends"                { COUNT(); return P(EXTENDS); }
"abstract"               { COUNT(); return P(ABSTRACT); }

"bool"                   { COUNT(); return P(BOOL_); }
"char"                   { COUNT(); return P(CHAR_); }
"short"                  { COUNT(); return P(SHORT_); }
"int"                    { COUNT(); return P(INT_); }
"long"                   { COUNT(); return P(LONG_); }
"double"                 { COUNT(); return P(DOUBLE_); }
"unsigned"               { COUNT(); return P(UNSIGNED_); }
"const"                  { COUNT(); return P(CONST_); /*reserved for future use*/ }
"string"                 { COUNT(); return P(STRING_); }
"true"                   { COUNT(); return P(TRUE_); }
"false"                  { COUNT(); return P(FALSE_); }

{L}({L}|{D})*            { COUNT(); return P(NAME); }
{D}+                     { COUNT(); return P(INTCONSTANT); }
0[xX]{X}+                { COUNT(); return P(INTCONSTANT); }
{D}+{E}                  { COUNT(); return P(REALCONSTANT); }
{D}*"."{D}+({E})?        { COUNT(); return P(REALCONSTANT); }
\'[^\']\'                { COUNT(); return P(CHARCONSTANT); }

\"                       { COUNT(); BEGIN(stringliteral); }
<stringliteral>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
    \\\n                 { EXTEND(); /* line continuation */ }
    \\\"                 { EXTEND(); /* quoted quote */ }
    \\[^\n\"]            { EXTEND(); /* quoted char */ }
    [^\\\n\"]+           { EXTEND(); /* character inside string literal */ }
    \"                   { EXTEND(); if (parenDepth==0) BEGIN(INITIAL); else BEGIN(propertyvalue); return P(STRINGCONSTANT); /* closing quote */ }
}

"@"                      { COUNT(); BEGIN(propertyname); return P('@'); }
<propertyname>{
    ({L}|{D}|[:.-])+     { COUNT(); BEGIN(afterpropertyname); return P(PROPNAME); }
    {S}                  { COUNT(); }
    .                    { yyless(0); BEGIN(INITIAL); }
}

<afterpropertyname>{
    "["                  { COUNT(); BEGIN(propertyindex); return P('['); }
    "("                  { COUNT(); BEGIN(propertyvalue); parenDepth=1; return P('('); }
    {S}                  { COUNT(); }
    .                    { yyless(0); BEGIN(INITIAL); }
}

<propertyindex>{
    "]"                  { COUNT(); BEGIN(afterpropertyname); return P(']'); }
    ({L}|{D}|[:.-])+     { COUNT(); return P(PROPNAME); }
    {S}                  { COUNT(); }
    .                    { yyless(0); BEGIN(INITIAL); }
}

<propertyvalue>{
    [({[]                { COUNT(); ++parenDepth; return P(COMMONCHAR); }
    [)}\]]               { COUNT(); if (--parenDepth==0) {BEGIN(INITIAL); return P(yytext[0]);} else return P(COMMONCHAR); }
    "="                  { COUNT(); return parenDepth==1 ? P('=') : P(COMMONCHAR); }
    ","                  { COUNT(); return parenDepth==1 ? P(',') : P(COMMONCHAR); }
    ";"                  { COUNT(); return parenDepth==1 ? P(';') : P(COMMONCHAR); }
    \"                   { COUNT(); BEGIN(stringliteral); }
    "\n"                 { COUNT(); return P(COMMONCHAR); }
    .                    { COUNT(); return P(COMMONCHAR); }
}

"{{"                     { COUNT(); BEGIN(cplusplusbody); }
<cplusplusbody>{
    "}}"                 { EXTEND(); BEGIN(INITIAL); return P(CPLUSPLUSBODY); }
    {S}                  { EXTEND(); }
    .                    { EXTEND(); }
}

";"                      { COUNT(); return P(';'); }
","                      { COUNT(); return P(','); }
":"                      { COUNT(); return P(':'); }
"="                      { COUNT(); return P('='); }
"("                      { COUNT(); return P('('); }
")"                      { COUNT(); return P(')'); }
"["                      { COUNT(); return P('['); }
"]"                      { COUNT(); return P(']'); }
"{"                      { COUNT(); return P('{'); }
"}"                      { COUNT(); return P('}'); }
"."                      { COUNT(); return P('.'); }
"?"                      { COUNT(); return P('?'); }

   /* XXX are the next ones really needed? */

"::"                     { COUNT(); return P(DOUBLECOLON); }
"||"                     { COUNT(); return P(OR); }
"&&"                     { COUNT(); return P(AND); }
"##"                     { COUNT(); return P(XOR); }
"!"                      { COUNT(); return P('!'); }

"|"                      { COUNT(); return P('|'); }
"&"                      { COUNT(); return P('&'); }
"#"                      { COUNT(); return P('#'); }
"~"                      { COUNT(); return P('~'); }
"<<"                     { COUNT(); return P(SHIFT_LEFT); }
">>"                     { COUNT(); return P(SHIFT_RIGHT); }

"^"                      { COUNT(); return P('^'); }
"+"                      { COUNT(); return P('+'); }
"-"                      { COUNT(); return P('-'); }
"*"                      { COUNT(); return P('*'); }
"/"                      { COUNT(); return P('/'); }
"%"                      { COUNT(); return P('%'); }
"<"                      { COUNT(); return P('<'); }
">"                      { COUNT(); return P('>'); }

"=="                     { COUNT(); return P(EQ); }
"!="                     { COUNT(); return P(NE); }
"<="                     { COUNT(); return P(LE); }
">="                     { COUNT(); return P(GE); }

"\xEF\xBB\xBF"           { /* UTF-8 BOM mark, ignore */ }
{S}                      { COUNT(); }
.                        { COUNT(); return P(INVALID_CHAR); }

%%

static void msg2count(const char *text, YYLTYPE *loc, bool extend)
{
    for (const char *s = text; *s; s++) {
        if (*s == '\n') {
            pos.co = 0;
            pos.li++;
        } else if (*s == '\t')
            pos.co += 8 - (pos.co % 8);
        else
            pos.co++;
    }

    if (!extend) {
        loc->first_line = loc->last_line;
        loc->first_column = loc->last_column;
    }

    loc->last_line = pos.li;
    loc->last_column = pos.co;
}
