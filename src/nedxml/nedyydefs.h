//==========================================================================
//  NEDYYDEFS.H - part of
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

#ifndef __OMNETPP_NEDXML_NEDYYDEFS_H
#define __OMNETPP_NEDXML_NEDYYDEFS_H

#include "nedxmldefs.h"


//
// misc bison/flex related stuff, shared among *.lex, *.y and nedparser.cc/h files
//
namespace omnetpp {
namespace nedxml {

class NEDElement;
class NEDParser;

} // namespace nedxml
}  // namespace omnetpp

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
#define YYSTYPE  omnetpp::nedxml::NEDElement*

namespace omnetpp {
namespace nedxml {

typedef struct {int li; int co;} LineColumn;
extern LineColumn pos, prevpos;

} // namespace nedxml
}  // namespace omnetpp

omnetpp::nedxml::NEDElement *doParseNED2(omnetpp::nedxml::NEDParser *p, const char *nedtext);
omnetpp::nedxml::NEDElement *doParseNED1(omnetpp::nedxml::NEDParser *p, const char *nedtext);
omnetpp::nedxml::NEDElement *doParseMSG2(omnetpp::nedxml::NEDParser *p, const char *nedtext);

#endif



