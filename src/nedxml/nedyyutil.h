//==========================================================================
// nedyyutil.h  -
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

#ifndef __OMNETPP_NEDXML_NEDYYUTIL_H
#define __OMNETPP_NEDXML_NEDYYUTIL_H

#include "yyutil.h"
#include "nedelements.h"

namespace omnetpp {
namespace nedxml {
namespace nedyyutil {  // for now

ASTNode *createNedElementWithTag(int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateNedElementWithTag(int tagcode, ASTNode *parent);

PropertyElement *addProperty(ASTNode *node, const char *name);  // directly under the node
PropertyElement *addComponentProperty(ASTNode *node, const char *name); // into ParametersElement child of node

PropertyElement *storeSourceCode(ASTNode *node, YYLTYPE tokenpos);  // directly under the node
PropertyElement *storeComponentSourceCode(ASTNode *node, YYLTYPE tokenpos); // into ParametersElement child
PropertyElement *setIsNetworkProperty(ASTNode *node); // into ParametersElement child

void addComment(ASTNode *node, const char *locId, const char *comment, const char *defaultValue);
void storeFileComment(ASTNode *node);
void storeBannerComment(ASTNode *node, YYLTYPE tokenpos);
void storeRightComment(ASTNode *node, YYLTYPE tokenpos);
void storeTrailingComment(ASTNode *node, YYLTYPE tokenpos);
void storeBannerAndRightComments(ASTNode *node, YYLTYPE pos);
void storeBannerAndRightComments(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos);
void storeInnerComments(ASTNode *node, YYLTYPE pos);

ParamElement *addParameter(ASTNode *params, YYLTYPE namepos);
ParamElement *addParameter(ASTNode *params, const char *name, YYLTYPE namepos);
GateElement *addGate(ASTNode *gates, YYLTYPE namepos);

void swapExpressionChildren(ASTNode *node, const char *attr1, const char *attr2);
void swapConnection(ASTNode *conn);

ExpressionElement *createExpression(ASTNode *expr);
OperatorElement *createOperator(const char *op, ASTNode *operand1, ASTNode *operand2=nullptr, ASTNode *operand3=nullptr);
FunctionElement *createFunction(const char *funcname, ASTNode *arg1=nullptr, ASTNode *arg2=nullptr, ASTNode *arg3=nullptr, ASTNode *arg4=nullptr, ASTNode *arg5=nullptr, ASTNode *arg6=nullptr, ASTNode *arg7=nullptr, ASTNode *arg8=nullptr, ASTNode *arg9=nullptr, ASTNode *arg10=nullptr);
IdentElement *createIdent(YYLTYPE parampos);
IdentElement *createIdent(YYLTYPE parampos, YYLTYPE modulepos, ASTNode *moduleindexoperand=nullptr);
LiteralElement *createPropertyValue(YYLTYPE textpos);
LiteralElement *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos);
LiteralElement *createLiteral(int type, const char *value, const char *text);
LiteralElement *createStringLiteral(YYLTYPE textpos);
LiteralElement *createQuantityLiteral(YYLTYPE textpos);
ASTNode *unaryMinus(ASTNode *node);

void addOptionalExpression(ASTNode *elem, const char *attrname, YYLTYPE exprpos, ASTNode *expr);
void addExpression(ASTNode *elem, const char *attrname, YYLTYPE exprpos, ASTNode *expr);

std::string convertBackgroundDisplayString(const char *old);

} // namespace nedyyutil
} // namespace nedxml
}  // namespace omnetpp


#endif

