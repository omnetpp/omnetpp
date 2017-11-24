//==========================================================================
// yyutil.h  -
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

#ifndef __OMNETPP_NEDXML_NEDYYLIB_H
#define __OMNETPP_NEDXML_NEDYYLIB_H

#include <string>
#include "astnode.h"
#include "nedparser.h"
#include "yydefs.h"

namespace omnetpp {
namespace nedxml {

#define NONREENTRANT_NED_PARSER(p) \
    struct Guard { \
      Guard(NEDParser *parser) {if (np) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); np=parser;} \
      ~Guard() {np=nullptr;} \
    } __guard(p);

std::string slashifyFilename(const char *fname);
const char *currentLocation();

ASTNode *createElementWithTag(ASTNodeFactory *factory, int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateElementWithTag(ASTNodeFactory *factory, int tagcode, ASTNode *parent);

void storePos(ASTNode *node, YYLTYPE pos);
void storePos(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos);

void swapAttributes(ASTNode *node, const char *attr1, const char *attr2);
void transferChildren(ASTNode *from, ASTNode *to);

YYLTYPE trimQuotes(YYLTYPE vectorpos);
YYLTYPE trimDoubleBraces(YYLTYPE vectorpos);

const char *toString(YYLTYPE);
const char *toString(long);
std::string removeSpaces(YYLTYPE pos);

inline bool isEmpty(YYLTYPE pos) {
    return pos.first_line > pos.last_line ||
           (pos.first_line == pos.last_line && pos.first_column >= pos.last_column);
}

inline YYLTYPE makeYYLTYPE(int fl, int fc, int ll, int lc) {
    YYLTYPE pos;
    pos.first_line = fl;
    pos.first_column = fc;
    pos.last_line = ll;
    pos.last_column = lc;
    return pos;
}

inline YYLTYPE makeEmptyYYLTYPE() {
    return makeYYLTYPE(1,0,1,0);
}

} // namespace nedxml
}  // namespace omnetpp


#endif

