/*==================================================
 * File: ned2.lex
 *
 *  Lexical analyser for OMNeT++ NED-2.
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

%{
#include <cstring>
#include "yydefs.h"
#include "ned2.tab.h"
#include "exception.h"

using namespace omnetpp::nedxml;

static int parenDepth = 0;

#define YY_USER_INIT  {parenDepth = 0; pos = {1,0}; yylloc->first_line = yylloc->last_line = 1; yylloc->first_column = yylloc->last_column = 0;}

static void ned2count(const char *text, YYLTYPE *loc, bool extend);

#define COUNT()  ned2count(yytext, yylloc, false)
#define EXTEND() ned2count(yytext, yylloc, true)

#define P(x)  (x)
//#define P(x) (printf("lexer: token %s '%s' at %d:%d..%d.%d, parenDepth=%d\n", #x, yytext, yylloc->first_line, yylloc->first_column, yylloc->last_line, yylloc->last_column, parenDepth), (x))

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

%}

%%
"@expr@"                 { return P(EXPRESSION_SELECTOR); /* must agree with nedparser.cc */ }

"//"                     {
                            int c;
                            while ((c = yyinput(yyscanner))!='\n' && c!=0 && c!=EOF);
                            if (c=='\n') unput(c);
                         }

"import"                 { COUNT(); return P(IMPORT); }
"package"                { COUNT(); return P(PACKAGE); }
"property"               { COUNT(); return P(PROPERTY); }

"module"                 { COUNT(); return P(MODULE); }
"simple"                 { COUNT(); return P(SIMPLE); }
"network"                { COUNT(); return P(NETWORK); }
"channel"                { COUNT(); return P(CHANNEL); }
"moduleinterface"        { COUNT(); return P(MODULEINTERFACE); }
"channelinterface"       { COUNT(); return P(CHANNELINTERFACE); }

"extends"                { COUNT(); return P(EXTENDS); }
"like"                   { COUNT(); return P(LIKE); }

"types"                  { COUNT(); return P(TYPES); }
"parameters"             { COUNT(); return P(PARAMETERS); }
"gates"                  { COUNT(); return P(GATES); }
"submodules"             { COUNT(); return P(SUBMODULES); }
"connections"            { COUNT(); return P(CONNECTIONS); }
"allowunconnected"       { COUNT(); return P(ALLOWUNCONNECTED); }

"double"                 { COUNT(); return P(DOUBLE_); }
"int"                    { COUNT(); return P(INT_); }
"string"                 { COUNT(); return P(STRING_); }
"bool"                   { COUNT(); return P(BOOL_); }
"xml"                    { COUNT(); return P(XML_); }
"object"                 { COUNT(); return P(OBJECT_); }
"volatile"               { COUNT(); return P(VOLATILE_); }

"input"                  { COUNT(); return P(INPUT_); }
"output"                 { COUNT(); return P(OUTPUT_); }
"inout"                  { COUNT(); return P(INOUT_); }

"if"                     { COUNT(); return P(IF); }
"for"                    { COUNT(); return P(FOR); }

"-->"                    { COUNT(); return P(RIGHTARROW); }
"<--"                    { COUNT(); return P(LEFTARROW); }
"<-->"                   { COUNT(); return P(DBLARROW); }
".."                     { COUNT(); return P(TO); }
"::"                     { COUNT(); return P(DOUBLECOLON); }

"true"                   { COUNT(); return P(TRUE_); }
"false"                  { COUNT(); return P(FALSE_); }
"nan"                    { COUNT(); return P(NAN_); }
"inf"                    { COUNT(); return P(INF_); }
"undefined"              { COUNT(); return P(UNDEFINED_); }
"nullptr"                { COUNT(); return P(NULLPTR_); }
"null"                   { COUNT(); return P(NULL_); }
"this"                   { COUNT(); return P(THIS_); }
"parent"                 { COUNT(); return P(PARENT); }
"default"                { COUNT(); return P(DEFAULT); }
"ask"                    { COUNT(); return P(ASK); }
"const"                  { COUNT(); return P(CONST_); }
"sizeof"                 { COUNT(); return P(SIZEOF); }
"index"                  { COUNT(); return P(INDEX_); }
"exists"                 { COUNT(); return P(EXISTS); }
"typename"               { COUNT(); return P(TYPENAME); }
"xmldoc"                 { COUNT(); return P(XMLDOC); }

{L}({L}|{D})*            { COUNT(); return P(NAME); }
{D}+                     { COUNT(); return P(INTCONSTANT); }
0[xX]{X}+                { COUNT(); return P(INTCONSTANT); }
{D}+{E}                  { COUNT(); return P(REALCONSTANT); }
{D}*"."{D}+({E})?        { COUNT(); return P(REALCONSTANT); }

\"                       { COUNT(); BEGIN(stringliteral); }
<stringliteral>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral> state! */ }
    \\\n                 { EXTEND(); /* line continuation */ }
    \\\"                 { EXTEND(); /* quoted quote */ }
    \\[^\n\"]            { EXTEND(); /* quoted char */ }
    [^\\\n\"]+           { EXTEND(); /* character inside string literal */ }
    \"                   { EXTEND(); if (parenDepth==0) BEGIN(INITIAL); else BEGIN(propertyvalue); return P(STRINGCONSTANT); /* closing quote */ }
}

'                        { COUNT(); BEGIN(stringliteral2); }
<stringliteral2>{
    \n                   { BEGIN(INITIAL); parenDepth=0; throw NedException("Unterminated string literal (append backslash to line for multi-line strings)"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file (!) will start from the <stringliteral2> state! */ }
    \\\n                 { EXTEND(); /* line continuation */ }
    \\\'                 { EXTEND(); /* quoted apostrophe */ }
    \\[^\n\']            { EXTEND(); /* quoted char */ }
    [^\\\n\']+           { EXTEND(); /* character inside string literal */ }
    '                    { EXTEND(); if (parenDepth==0) BEGIN(INITIAL); else BEGIN(propertyvalue); return P(STRINGCONSTANT); /* closing quote */ }
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
    ({L}|{D}|[*?{}:.-])+ { COUNT(); return P(PROPNAME); }
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

"**"                     { COUNT(); return P(DOUBLEASTERISK); }
"++"                     { COUNT(); return P(PLUSPLUS); }

"$"                      { COUNT(); return P('$'); }
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
"<=>"                    { COUNT(); return P(SPACESHIP); }
"=~"                     { COUNT(); return P(MATCH); }

"\\xEF\\xBB\\xBF"        { COUNT(); /* UTF-8 BOM mark, ignore */ }
{S}                      { COUNT(); }
.                        { COUNT(); return P(INVALID_CHAR); }

%%

static void ned2count(const char *text, YYLTYPE *loc, bool extend)
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
