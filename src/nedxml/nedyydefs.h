//==========================================================================
//  NEDYYDEFS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDYYDEFS_H
#define __OMNETPP_NEDYYDEFS_H

#include "nedxmldefs.h"


//
// misc bison/flex related stuff, shared among *.lex, *.y and nedparser.cc/h files
//
NAMESPACE_BEGIN
namespace nedxml {

class NEDElement;
class NEDParser;

} // namespace nedxml
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
#define YYSTYPE  OPP::nedxml::NEDElement*

NAMESPACE_BEGIN
namespace nedxml {

typedef struct {int li; int co;} LineColumn;
extern LineColumn pos, prevpos;

} // namespace nedxml
NAMESPACE_END

OPP::nedxml::NEDElement *doParseNED2(OPP::nedxml::NEDParser *p, const char *nedtext);
OPP::nedxml::NEDElement *doParseNED1(OPP::nedxml::NEDParser *p, const char *nedtext);
OPP::nedxml::NEDElement *doParseMSG2(OPP::nedxml::NEDParser *p, const char *nedtext);

#endif



