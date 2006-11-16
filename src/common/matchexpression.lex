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

#include "matchexpression.tab.h"

#define YYSTYPE  const char *
extern YYSTYPE yylval;

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
"|"                     { return '|'; }
"&"                     { return '&'; }
\"[^\"]*\"              { yylval = matchexpr_strdup(yytext); return STRINGLITERAL; }
[^ \t\n()|&]*           { yylval = matchexpr_strdup(yytext); return STRINGLITERAL; }

.                       { }

%%

int yywrap(void)
{
     return 1;
}
