/***************************************************/
/*  File: nedgrammar.h                             */
/*                                                 */
/*  Part of OMNeT++/OMNEST                         */
/*                                                 */
/*  Contents:                                      */
/*    declarations shared by ned.lex and ned.y     */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDGRAMMAR_H
#define __NEDGRAMMAR_H

// define the following if -p (-P) options are in use with bison/flex
#define PARSER_PREFIX_USED

#ifdef PARSER_PREFIX_USED

// from lex.yy.cc:
#define yy_create_buffer nedyy_create_buffer
#define yy_delete_buffer nedyy_delete_buffer
#define yy_scan_buffer nedyy_scan_buffer
#define yy_scan_string nedyy_scan_string
#define yy_scan_bytes nedyy_scan_bytes
#define yy_flex_debug nedyy_flex_debug
#define yy_init_buffer nedyy_init_buffer
#define yy_flush_buffer nedyy_flush_buffer
#define yy_load_buffer_state nedyy_load_buffer_state
#define yy_switch_to_buffer nedyy_switch_to_buffer
#define yyin nedyyin
#define yyleng nedyyleng
#define yylex nedyylex
#define yyout nedyyout
#define yyrestart nedyyrestart
#define yytext nedyytext
#define yywrap nedyywrap

// from ned.tab.cc:
#define yyparse nedyyparse
#define yylex nedyylex
#define yyerror nedyyerror
#define yylval nedyylval
#define yychar nedyychar
#define yydebug nedyydebug
#define yynerrs nedyynerrs

// only new bisons define the following, but works with older ones (1.28) too
#define yylloc nedyylloc

#endif


class NEDElement;
class NedFileNode;
class NEDParser;
#define YYSTYPE   NEDElement*

#ifndef YYLTYPE
struct my_yyltype {
   int dumy;
   int first_line, first_column;
   int last_line, last_column;
   char *text;
};
#define YYLTYPE  struct my_yyltype
#else
#error 'YYLTYPE defined before ebnf.h -- type clash?'
#endif

typedef struct {int li; int co;} LineColumn;

extern LineColumn pos,prevpos;

extern FILE *yyin;
extern FILE *yyout;

struct yy_buffer_state;

struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);

int runparse (NEDParser *p,NedFileNode *nf,bool parseexpr,bool storesrc, const char *sourcefname);

extern int use_chanattrname_token;

#endif



