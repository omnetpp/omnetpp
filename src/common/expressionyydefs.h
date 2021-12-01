//==========================================================================
//  EXPRESSIONYYDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_EXPRESSIONYYDEFS_H
#define __OMNETPP_COMMON_EXPRESSIONYYDEFS_H

#include "expression.h"

//
// misc bison/flex related stuff, shared among *.lex and *.y files
//
#ifdef YYLTYPE
#error 'YYLTYPE defined before expryydefs.h -- type clash?'
#endif

struct my_yyltype {
   int dummy;
   int first_line, first_column;
   int last_line, last_column;
   char *text;
};
#define YYLTYPE  struct my_yyltype


#define xpos     expressionxpos
#define xprevpos expressionxprevpos

typedef struct {int li; int co;} LineColumn;
extern LineColumn xpos, xprevpos;

#endif



