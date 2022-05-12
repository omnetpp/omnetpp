/*==================================================
 * File: expression.lex
 *
 *  Lexical analyser for generic arithmetic expressions.
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
%option bison-bridge
%option never-interactive nounistd

/*
 * Note: since the Expression class is used to parse NED expressions as well, the Expression
 * lexer (and parser) must accept everything that may occur in NED expressions. For example,
 * the local definition of L ('letter', see below) allows a superset of NED's definition.
 */

D  [0-9]
L  [$@a-zA-Z_\x80-\xff]
X  [0-9a-fA-F]
E  [Ee][+-]?{D}+
S  [ \t\v\n\r\f]

%x stringliteral
%x stringliteral2

%{
#include "stringutil.h"  // opp_strdup()
#include "expression.h"
#include "expression.tab.h"
#include "exception.h"

using namespace omnetpp;
using namespace omnetpp::common;

typedef Expression::AstNode AstNode;

static std::string extendbuf;

#define P(x)  (x)
//#define P(x) (printf("lexer: token %s '%s'\n", #x, yytext), (x))

#pragma GCC diagnostic ignored "-Wsign-compare"  // suppress warning in bison template code

%}

%%
"//"                     {  /* needed for NED */
                            int c;
                            while ((c = yyinput(yyscanner))!='\n' && c!=0 && c!=EOF);
                            if (c=='\n') unput(c);
                         }

"true"                   { return TRUE_; }
"false"                  { return FALSE_; }
"nan"                    { return NAN_; }
"inf"                    { return INF_; }
"undefined"              { return UNDEFINED_; }
"nullptr"                { return NULLPTR_; }
"null"                   { return NULL_; /* JSON-compatible notation */ }

{L}({L}|{D})*(:{L}({L}|{D})*)*  { yylval->str = opp_strdup(yytext); return NAME; }
{D}+                     { yylval->str = opp_strdup(yytext); return INTCONSTANT; }
0[xX]{X}+                { yylval->str = opp_strdup(yytext); return INTCONSTANT; }
{D}+{E}                  { yylval->str = opp_strdup(yytext); return REALCONSTANT; }
{D}*"."{D}+({E})?        { yylval->str = opp_strdup(yytext); return REALCONSTANT; }

\"                       { BEGIN(stringliteral); extendbuf = yytext; }
<stringliteral>{
      \n                 { BEGIN(INITIAL); throw std::runtime_error("Unterminated string literal"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file will start from the <stringliteral> state! */  }
      \\\n               { extendbuf += yytext; /* line continuation */ }
      \\\"               { extendbuf += yytext; /* quoted quote */ }
      \\[^\n\"]          { extendbuf += yytext; /* quoted char */ }
      [^\\\n\"]+         { extendbuf += yytext; /* character inside string literal */ }
      \"                 { extendbuf += yytext; yylval->str = opp_strdup(extendbuf.c_str()); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

'                        { BEGIN(stringliteral2); extendbuf = yytext; }
<stringliteral2>{
      \n                 { BEGIN(INITIAL); throw std::runtime_error("Unterminated string literal"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file will start from the <stringliteral2> state! */  }
      \\\n               { extendbuf += yytext; /* line continuation */ }
      \\\'               { extendbuf += yytext; /* quoted apostrophe */ }
      \\[^\n\']          { extendbuf += yytext; /* quoted char */ }
      [^\\\n\']+         { extendbuf += yytext; /* character inside string literal */ }
      '                  { extendbuf += yytext; yylval->str = opp_strdup(extendbuf.c_str()); BEGIN(INITIAL); return STRINGCONSTANT; /* closing quote */ }
}

","                      { return ','; }
":"                      { return ':'; }
"="                      { return '='; }
"("                      { return '('; }
")"                      { return ')'; }
"["                      { return '['; }
"]"                      { return ']'; }
"{"                      { return '{'; }
"}"                      { return '}'; }
"."                      { return '.'; }
"?"                      { return '?'; }

"||"                     { return OR; }
"&&"                     { return AND; }
"##"                     { return XOR; }
"!"                      { return '!'; }

"|"                      { return '|'; }
"&"                      { return '&'; }
"#"                      { return '#'; }
"~"                      { return '~'; }
"<<"                     { return SHIFT_LEFT; }
">>"                     { return SHIFT_RIGHT; }

"^"                      { return '^'; }
"+"                      { return '+'; }
"-"                      { return '-'; }
"*"                      { return '*'; }
"/"                      { return '/'; }
"%"                      { return '%'; }
"<"                      { return '<'; }
">"                      { return '>'; }

"=="                     { return EQ; }
"!="                     { return NE; }
"<="                     { return LE; }
">="                     { return GE; }
"<=>"                    { return SPACESHIP; }
"=~"                     { return MATCH; }

"::"                     { return DOUBLECOLON; }

"\\xEF\\xBB\\xBF"        { /* UTF-8 BOM mark, ignore */ }
{S}                      { }
.                        { return INVALID_CHAR; }

%%

