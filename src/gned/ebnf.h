/***************************************************/
/*            OMNeT++/OMNEST nedc source           */
/*                                                 */
/*  File: ebnf.h                                   */
/*                                                 */
/*  Contents:                                      */
/*    declarations shared by ebnf.lex and ebnf.y   */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EBNF_H
#define __EBNF_H

/*
 * IMPORTANT: This file is shared between NEDC and GNED.
 * The two copies must be kept identical!
 */

#define YYSTYPE   char *

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


#endif



