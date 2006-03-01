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

#define yylloc ned2yylloc

class NEDElement;
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
#error 'YYLTYPE defined before nedgrammar.h -- type clash?'
#endif

typedef struct {int li; int co;} LineColumn;

extern LineColumn pos,prevpos;


// FIXME: move to .y files
#define yy_scan_string ned2yy_scan_string
#define yyin ned2yyin
#define yyout ned2yyout
#define yyrestart ned2yyrestart
extern FILE *yyin;
extern FILE *yyout;
struct yy_buffer_state;
struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);

NEDElement *doParseNED2(NEDParser *p, const char *nedtext);


#endif



