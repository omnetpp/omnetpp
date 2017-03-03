//==========================================================================
//  EXPRYYDEFS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EXPRYYDEFS_H
#define __OMNETPP_EXPRYYDEFS_H

#include "omnetpp/cdynamicexpression.h"

//namespace omnetpp {

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
#define YYSTYPE  char*

typedef struct {int li; int co;} LineColumn;
extern LineColumn xpos, xprevpos;

void doParseExpression(const char *nedtext, omnetpp::cDynamicExpression::Elem *&elems, int& nelems);

//}  // namespace omnetpp


#endif



