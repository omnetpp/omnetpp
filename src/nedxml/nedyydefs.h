//==========================================================================
//  NEDYYDEFS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDYYDEFS_H
#define __NEDYYDEFS_H

#include "nedxmldefs.h"


//
// misc bison/flex related stuff, shared among *.lex, *.y and nedparser.cc/h files
//
NAMESPACE_BEGIN

class NEDElement;
class NEDParser;

NAMESPACE_END

#ifdef YYLTYPE
#error 'YYLTYPE defined before nedyydefs.h -- type clash?'
#endif

struct my_yyltype {
   int dummy;
   int first_line, first_column;
   int last_line, last_column;
   char *text;
};
#define YYLTYPE  struct my_yyltype
#define YYSTYPE  OPP::NEDElement*

NAMESPACE_BEGIN

typedef struct {int li; int co;} LineColumn;
extern LineColumn pos, prevpos;

NAMESPACE_END

OPP::NEDElement *doParseNED2(OPP::NEDParser *p, const char *nedtext);
OPP::NEDElement *doParseNED1(OPP::NEDParser *p, const char *nedtext);
OPP::NEDElement *doParseMSG2(OPP::NEDParser *p, const char *nedtext);

#endif



