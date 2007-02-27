/*==================================================
 * File: matchexpression.lex
 *
 *  Lexical analyser for match expressions.
 *
 *  Author: Andras Varga
 *
 ==================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


%x stringliteral

/* the following option keeps isatty() out */
%option never-interactive

%{
#include <string.h>
#include <string>
#include "stringutil.h"

#define YYSTYPE  char *
#define yylval  matchexpressionyylval
extern YYSTYPE yylval;

#include "matchexpression.tab.h"

// buffer to collect characters while in stringliteral mode
static std::string extendbuf;

static char *matchexpr_strdup(const char *s)
{
    char *d = new char[strlen(s)+1];
    strcpy(d, s);
    return d;
}

static void extend()
{
    extendbuf += yytext;
}

static char *unquote()
{
    std::string unquoted = opp_parsequotedstr(extendbuf.c_str());
    return matchexpr_strdup(unquoted.c_str());
}

%}

%%
"("                     { return '('; }
")"                     { return ')'; }

"not"                   { return NOT_; }
"and"                   { return AND_; }
"or"                    { return OR_;  }

"NOT"                   { return NOT_; }
"AND"                   { return AND_; }
"OR"                    { return OR_;  }

\"                      { BEGIN(stringliteral); extendbuf=yytext; }
<stringliteral>{
      \n                { BEGIN(INITIAL); throw std::runtime_error("unterminated string constant"); /* NOTE: BEGIN(INITIAL) is important, otherwise parsing of the next file will start from the <stringliteral> state! */  }
      \\\n              { extend(); /* line continuation */ }
      \\\"              { extend(); /* quoted quote */ }
      \\[^\n\"]         { extend(); /* quoted char */ }
      [^\\\n\"]+        { extend(); /* character inside string literal */ }
      \"                { extend(); BEGIN(INITIAL); yylval = unquote(); return STRINGLITERAL; /* closing quote */ }
}

[^ \t\n()"]*            { yylval = matchexpr_strdup(yytext); return STRINGLITERAL; }

.                       {  }

%%

int yywrap(void)
{
     return 1;
}


