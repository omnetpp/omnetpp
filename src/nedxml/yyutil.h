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

#ifndef __OMNETPP_NEDXML_YYUTIL_H
#define __OMNETPP_NEDXML_YYUTIL_H

#include <string>
#include "astnode.h"
#include "yydefs.h"

namespace omnetpp {
namespace nedxml {

extern bool parseInProgress;

#define DETECT_PARSER_REENTRY() \
    struct Guard { \
     Guard() {if (parseInProgress) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); parseInProgress = true;} \
      ~Guard() {parseInProgress = false;} \
    } __guard;

std::string slashifyFilename(const char *fname);
ASTNode *createElementWithTag(ParseContext *np, ASTNodeFactory *factory, int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateElementWithTag(ParseContext *np, ASTNodeFactory *factory, int tagcode, ASTNode *parent);

void storePos(ParseContext *np, ASTNode *node, YYLoc pos);
void storePos(ParseContext *np, ASTNode *node, YYLoc firstpos, YYLoc lastpos);

void swapAttributes(ASTNode *node, const char *attr1, const char *attr2);
void transferChildren(ASTNode *from, ASTNode *to);

YYLoc trimQuotes(YYLoc vectorpos);
YYLoc trimDoubleBraces(YYLoc vectorpos);

const char *toString(ParseContext *np, YYLoc);
const char *toString(long);
std::string removeSpaces(ParseContext *np, YYLoc pos);

inline bool isEmpty(YYLoc pos) {
    return pos.first_line > pos.last_line ||
           (pos.first_line == pos.last_line && pos.first_column >= pos.last_column);
}

inline YYLoc makeYYLoc(int fl, int fc, int ll, int lc) { //TODO ctor!
    YYLoc pos;
    pos.first_line = fl;
    pos.first_column = fc;
    pos.last_line = ll;
    pos.last_column = lc;
    return pos;
}

inline YYLoc makeEmptyYYLoc() {  //TODO ctor!
    return makeYYLoc(1,0,1,0);
}

}  // namespace nedxml
}  // namespace omnetpp


#endif

