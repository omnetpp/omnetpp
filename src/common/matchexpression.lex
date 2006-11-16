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


%{
#include <string.h>

#if defined(_MSC_VER)
# include <io.h>
# define isatty _isatty
#else
# include <unistd.h>  // isatty
#endif

#define YYSTYPE  char *
#define yylval  matchexpressionyylval
extern YYSTYPE yylval;

#include "matchexpression.tab.h"

static char *matchexpr_strdup(const char *s)
{
    char *d = new char[strlen(s)+1];
    strcpy(d, s);
    return d;
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

\"[^\"]*\"              { yylval = matchexpr_strdup(yytext+1); yylval[strlen(yylval)-1] = '\0'; return STRINGLITERAL; }
[^ \t\n()]*             { yylval = matchexpr_strdup(yytext); return STRINGLITERAL; }

.                       { }

%%

int yywrap(void)
{
     return 1;
}
