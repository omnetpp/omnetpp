//==========================================================================
// msgyyutil.h  -
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

#ifndef __OMNETPP_NEDXML_MSGYYUTIL_H
#define __OMNETPP_NEDXML_MSGYYUTIL_H

#include "yyutil.h"
#include "msgelements.h"

namespace omnetpp {
namespace nedxml {
namespace msgyyutil { // for now

ASTNode *createMsgElementWithTag(ParseContext *np, int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateMsgElementWithTag(ParseContext *np, int tagcode, ASTNode *parent);

PropertyElement *addProperty(ParseContext *np, ASTNode *node, const char *name);  // directly under the node
PropertyElement *storeSourceCode(ParseContext *np, ASTNode *node, YYLoc tokenpos);
LiteralElement *createPropertyValue(ParseContext *np, YYLoc textpos);

void addComment(ParseContext *np, ASTNode *node, const char *locId, const char *comment, const char *defaultValue);
void storeFileComment(ParseContext *np, ASTNode *node);
void storeBannerComment(ParseContext *np, ASTNode *node, YYLoc tokenpos);
void storeRightComment(ParseContext *np, ASTNode *node, YYLoc tokenpos);
void storeTrailingComment(ParseContext *np, ASTNode *node, YYLoc tokenpos);
void storeBannerAndRightComments(ParseContext *np, ASTNode *node, YYLoc pos);
void storeBannerAndRightComments(ParseContext *np, ASTNode *node, YYLoc firstpos, YYLoc lastpos);
void storeInnerComments(ParseContext *np, ASTNode *node, YYLoc pos);

}  // namespace msgyyutil
}  // namespace nedxml
}  // namespace omnetpp


#endif

