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

ASTNode *createMsgElementWithTag(int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateMsgElementWithTag(int tagcode, ASTNode *parent);

PropertyElement *addProperty(ASTNode *node, const char *name);  // directly under the node
PropertyElement *storeSourceCode(ASTNode *node, YYLTYPE tokenpos);
LiteralElement *createPropertyValue(YYLTYPE textpos);

void addComment(ASTNode *node, const char *locId, const char *comment, const char *defaultValue);
void storeFileComment(ASTNode *node);
void storeBannerComment(ASTNode *node, YYLTYPE tokenpos);
void storeRightComment(ASTNode *node, YYLTYPE tokenpos);
void storeTrailingComment(ASTNode *node, YYLTYPE tokenpos);
void storeBannerAndRightComments(ASTNode *node, YYLTYPE pos);
void storeBannerAndRightComments(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos);
void storeInnerComments(ASTNode *node, YYLTYPE pos);

} // namespace msgyyutil
} // namespace nedxml
}  // namespace omnetpp


#endif

