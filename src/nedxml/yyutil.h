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
#include "nedelements.h"
#include "nedyydefs.h"
#include "nedparser.h"

namespace omnetpp {
namespace nedxml {

#define NONREENTRANT_NED_PARSER(p) \
    struct Guard { \
      Guard(NEDParser *parser) {if (np) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); np=parser;} \
      ~Guard() {np=nullptr;} \
    } __guard(p);

std::string slashifyFilename(const char *fname);
const char *currentLocation();

ASTNode *createElementWithTag(int tagcode, ASTNode *parent=nullptr);
ASTNode *getOrCreateElementWithTag(int tagcode, ASTNode *parent);

void storePos(ASTNode *node, YYLTYPE pos);
void storePos(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos);

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

YYLTYPE trimQuotes(YYLTYPE vectorpos);
YYLTYPE trimDoubleBraces(YYLTYPE vectorpos);
void swapAttributes(ASTNode *node, const char *attr1, const char *attr2);
void swapExpressionChildren(ASTNode *node, const char *attr1, const char *attr2);
void swapConnection(ASTNode *conn);
void transferChildren(ASTNode *from, ASTNode *to);

const char *toString(YYLTYPE);
const char *toString(long);
std::string removeSpaces(YYLTYPE pos);

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

