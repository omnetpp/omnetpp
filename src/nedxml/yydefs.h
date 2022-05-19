//==========================================================================
//  YYDEFS.H - part of
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

#ifndef __OMNETPP_NEDXML_YYDEFS_H
#define __OMNETPP_NEDXML_YYDEFS_H

#include "nedxmldefs.h"

//
// misc bison/flex related stuff, shared among *.lex, *.y and nedparser.cc/h files
//

#ifdef YYLTYPE
#error 'YYLTYPE defined before yydefs.h -- type clash?'
#endif

namespace omnetpp {
namespace nedxml {

class ErrorStore;
class SourceDocument;
class ASTNode;

struct YYLoc {
   int dummy;
   int first_line, first_column;
   int last_line, last_column;
   char *text;
};

#define YYLTYPE  omnetpp::nedxml::YYLoc
#define YYSTYPE  omnetpp::nedxml::ASTNode*

//TODO cleanup
struct ParseContext {
    bool storesrc = false;               // whether to fill in sourceCode attributes
    const char *filename = nullptr;      // name of file being parsed
    ErrorStore *errors = nullptr;        // accumulates error messages
    SourceDocument *source = nullptr;    // represents the source file

    bool getStoreSourceFlag()  {return storesrc;}
    const char *getFileName() {return filename;}
    ErrorStore *getErrors() {return errors;}
    SourceDocument *getSource() {return source;}
    void error(const char *msg, int line);
};

struct LineColumn {
    int li;
    int co;
};

extern LineColumn pos;

}  // namespace nedxml
}  // namespace omnetpp

omnetpp::nedxml::ASTNode *doParseNed(omnetpp::nedxml::ParseContext *np);
omnetpp::nedxml::ASTNode *doParseMsg(omnetpp::nedxml::ParseContext *np);


#endif



