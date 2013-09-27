//==========================================================================
// nedyylib.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDYYLIB_H
#define __NEDYYLIB_H

#include <string>
#include "nedelements.h"
#include "nedyydefs.h"
#include "nedparser.h"

NAMESPACE_BEGIN

#define NONREENTRANT_NED_PARSER(p) \
    struct Guard { \
      Guard(NEDParser *parser) {if (np) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); np=parser;} \
      ~Guard() {np=NULL;} \
    } __guard(p);

std::string slashifyFilename(const char *fname);
const char *currentLocation();

NEDElement *createElementWithTag(int tagcode, NEDElement *parent=NULL);
NEDElement *getOrCreateElementWithTag(int tagcode, NEDElement *parent);

void storePos(NEDElement *node, YYLTYPE pos);
void storePos(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);

PropertyElement *addProperty(NEDElement *node, const char *name);  // directly under the node
PropertyElement *addComponentProperty(NEDElement *node, const char *name); // into ParametersElement child of node

PropertyElement *storeSourceCode(NEDElement *node, YYLTYPE tokenpos);  // directly under the node
PropertyElement *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos); // into ParametersElement child
PropertyElement *setIsNetworkProperty(NEDElement *node); // into ParametersElement child

void addComment(NEDElement *node, const char *locId, const char *comment, const char *defaultValue);
void storeFileComment(NEDElement *node);
void storeBannerComment(NEDElement *node, YYLTYPE tokenpos);
void storeRightComment(NEDElement *node, YYLTYPE tokenpos);
void storeTrailingComment(NEDElement *node, YYLTYPE tokenpos);
void storeBannerAndRightComments(NEDElement *node, YYLTYPE pos);
void storeBannerAndRightComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos);
void storeInnerComments(NEDElement *node, YYLTYPE pos);

ParamElement *addParameter(NEDElement *params, YYLTYPE namepos);
ParamElement *addParameter(NEDElement *params, const char *name, YYLTYPE namepos);
GateElement *addGate(NEDElement *gates, YYLTYPE namepos);

YYLTYPE trimQuotes(YYLTYPE vectorpos);
YYLTYPE trimDoubleBraces(YYLTYPE vectorpos);
void swapAttributes(NEDElement *node, const char *attr1, const char *attr2);
void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2);
void swapConnection(NEDElement *conn);
void transferChildren(NEDElement *from, NEDElement *to);

const char *toString(YYLTYPE);
const char *toString(long);
std::string removeSpaces(YYLTYPE pos);

ExpressionElement *createExpression(NEDElement *expr);
OperatorElement *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2=NULL, NEDElement *operand3=NULL);
FunctionElement *createFunction(const char *funcname, NEDElement *arg1=NULL, NEDElement *arg2=NULL, NEDElement *arg3=NULL, NEDElement *arg4=NULL, NEDElement *arg5=NULL, NEDElement *arg6=NULL, NEDElement *arg7=NULL, NEDElement *arg8=NULL, NEDElement *arg9=NULL, NEDElement *arg10=NULL);
IdentElement *createIdent(YYLTYPE parampos);
IdentElement *createIdent(YYLTYPE parampos, YYLTYPE modulepos, NEDElement *moduleindexoperand=NULL);
LiteralElement *createPropertyValue(YYLTYPE textpos);
LiteralElement *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos);
LiteralElement *createLiteral(int type, const char *value, const char *text);
LiteralElement *createStringLiteral(YYLTYPE textpos);
LiteralElement *createQuantityLiteral(YYLTYPE textpos);
NEDElement *unaryMinus(NEDElement *node);

void addOptionalExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);
void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr);

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

NAMESPACE_END


#endif

