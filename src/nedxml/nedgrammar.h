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

class NEDElement;
class NEDParser;

#ifdef YYLTYPE
#error 'YYLTYPE defined before nedgrammar.h -- type clash?'
#endif

struct my_yyltype {
   int dummy;
   int first_line, first_column;
   int last_line, last_column;
   char *text;
};
#define YYLTYPE  struct my_yyltype
#define YYSTYPE  NEDElement*

typedef struct {int li; int co;} LineColumn;
extern LineColumn pos, prevpos;

NEDElement *doParseNED2(NEDParser *p, const char *nedtext);
NEDElement *doParseNED(NEDParser *p, const char *nedtext);
NEDElement *doParseMSG(NEDParser *p, const char *nedtext);


#endif



