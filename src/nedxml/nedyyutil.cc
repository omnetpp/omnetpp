//==========================================================================
// nedyyutil.cc  -
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

#include <cassert>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "nedyyutil.h"
#include "sourcedocument.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {
namespace nedyyutil {  // for now

static NEDAstNodeFactory factory;

ASTNode *createNedElementWithTag(int tagcode, ASTNode *parent)
{
    return createElementWithTag(&factory, tagcode, parent);
}

ASTNode *getOrCreateNedElementWithTag(int tagcode, ASTNode *parent)
{
    return getOrCreateElementWithTag(&factory, tagcode, parent);
}

//
// Properties
//

PropertyElement *addProperty(ASTNode *node, const char *name)
{
    PropertyElement *prop = (PropertyElement *)createNedElementWithTag(NED_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyElement *addComponentProperty(ASTNode *node, const char *name)
{
    // add propery under the ParametersElement; create that if not yet exists
    ASTNode *params = node->getFirstChildWithTag(NED_PARAMETERS);
    if (!params) {
        params = createNedElementWithTag(NED_PARAMETERS, node);

        // move parameters section in front of potential gates, types, etc sections
        ASTNode *prev;
        while ((prev = params->getPrevSibling()) != nullptr &&
               (prev->getTagCode() == NED_GATES || prev->getTagCode() == NED_TYPES ||
                prev->getTagCode() == NED_SUBMODULES || prev->getTagCode() == NED_CONNECTIONS))
        {
            node->removeChild(params);
            node->insertChildBefore(prev, params);
        }
    }
    PropertyElement *prop = (PropertyElement *)createNedElementWithTag(NED_PROPERTY, params);
    prop->setName(name);
    return prop;
}

//
// Spec Properties: source code, display string
//

PropertyElement *storeSourceCode(ASTNode *node, YYLTYPE tokenpos)
{
    PropertyElement *prop = addProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyElement *propkey = (PropertyKeyElement *)createNedElementWithTag(NED_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, makeEmptyYYLTYPE()));  // don't store it twice
    return prop;
}

PropertyElement *storeComponentSourceCode(ASTNode *node, YYLTYPE tokenpos)
{
    PropertyElement *prop = addComponentProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyElement *propkey = (PropertyKeyElement *)createNedElementWithTag(NED_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, makeEmptyYYLTYPE()));  // don't store it twice
    return prop;
}

PropertyElement *setIsNetworkProperty(ASTNode *node)
{
    PropertyElement *prop = addComponentProperty(node, "isNetwork");
    prop->setIsImplicit(true);
    // Note: the following is not needed, because @isNetwork already means @isNetwork(true)
    // PropertyKeyElement *propkey = (PropertyKeyElement *)createNedElementWithTag(NED_PROPERTY_KEY, prop);
    // propkey->appendChild(createLiteral(NED_CONST_BOOL, "true", ""));
    return prop;
}

//
// Comments
//
void addComment(ASTNode *node, const char *locId, const char *text, const char *defaultValue)
{
    // don't store empty string or the default
    if (!text[0] || strcmp(text, defaultValue) == 0)
        return;

    CommentElement *comment = (CommentElement *)createNedElementWithTag(NED_COMMENT);
    comment->setLocid(locId);
    comment->setContent(text);
    node->insertChildBefore(node->getFirstChild(), comment);
}

void storeFileComment(ASTNode *node)
{
    addComment(node, "banner", np->getSource()->getFileComment(), "");
}

void storeBannerComment(ASTNode *node, YYLTYPE tokenpos)
{
    addComment(node, "banner", np->getSource()->getBannerComment(tokenpos), "");
}

void storeRightComment(ASTNode *node, YYLTYPE tokenpos)
{
    addComment(node, "right", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeTrailingComment(ASTNode *node, YYLTYPE tokenpos)
{
    addComment(node, "trailing", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeBannerAndRightComments(ASTNode *node, YYLTYPE pos)
{
    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(node, pos);
    storeRightComment(node, pos);
    storeInnerComments(node, pos);
}

void storeBannerAndRightComments(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(node, pos);
    storeRightComment(node, pos);
}

void storeInnerComments(ASTNode *node, YYLTYPE pos)
{
    for (;;) {
        const char *comment = np->getSource()->getNextInnerComment(pos);  // updates "pos"
        if (!comment)
            break;
        addComment(node, "inner", comment, "");
    }
}

ParamElement *addParameter(ASTNode *params, YYLTYPE namepos)
{
    ParamElement *param = (ParamElement *)createNedElementWithTag(NED_PARAM, params);
    param->setName(toString(namepos));
    return param;
}

ParamElement *addParameter(ASTNode *params, const char *name, YYLTYPE namepos)
{
    ParamElement *param = (ParamElement *)createNedElementWithTag(NED_PARAM, params);
    param->setName(name);
    return param;
}

GateElement *addGate(ASTNode *gates, YYLTYPE namepos)
{
    GateElement *gate = (GateElement *)createNedElementWithTag(NED_GATE, gates);
    gate->setName(toString(namepos));
    return gate;
}

void addOptionalExpression(ASTNode *elem, const char *attrname, YYLTYPE exprpos, ASTNode *expr)
{
    if (!expr)
        elem->setAttribute(attrname, toString(exprpos));
    else
        addExpression(elem, attrname, exprpos, expr);
}

void addExpression(ASTNode *elem, const char *attrname, YYLTYPE exprpos, ASTNode *expr)
{
    if (np->getParseExpressionsFlag()) {
        assert(expr);
        ((ExpressionElement *)expr)->setTarget(attrname);

        // in the DTD, whilespaces and expressions are at front, insert there
        ASTNode *insertPos = elem->getFirstChild();
        while (insertPos && (insertPos->getTagCode() == NED_COMMENT || insertPos->getTagCode() == NED_EXPRESSION))
            insertPos = insertPos->getNextSibling();
        if (!insertPos)
            elem->appendChild(expr);
        else
            elem->insertChildBefore(insertPos, expr);
    }
    else {
        elem->setAttribute(attrname, toString(exprpos));
    }
}

void swapConnection(ASTNode *conn)
{
    swapAttributes(conn, "src-module", "dest-module");
    swapAttributes(conn, "src-module-index", "dest-module-index");
    swapAttributes(conn, "src-gate", "dest-gate");
    swapAttributes(conn, "src-gate-index", "dest-gate-index");
    swapAttributes(conn, "src-gate-plusplus", "dest-gate-plusplus");
    swapAttributes(conn, "src-gate-subg", "dest-gate-subg");

    swapExpressionChildren(conn, "src-module-index", "dest-module-index");
    swapExpressionChildren(conn, "src-gate-index", "dest-gate-index");
}

void swapExpressionChildren(ASTNode *node, const char *attr1, const char *attr2)
{
    ExpressionElement *expr1, *expr2;
    for (expr1 = (ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr1; expr1 = expr1->getNextExpressionSibling())
        if (!strcmp(expr1->getTarget(), attr1))
            break;

    for (expr2 = (ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr2; expr2 = expr2->getNextExpressionSibling())
        if (!strcmp(expr2->getTarget(), attr2))
            break;


    if (expr1)
        expr1->setTarget(attr2);
    if (expr2)
        expr2->setTarget(attr1);
}

OperatorElement *createOperator(const char *op, ASTNode *operand1, ASTNode *operand2, ASTNode *operand3)
{
    OperatorElement *opnode = (OperatorElement *)createNedElementWithTag(NED_OPERATOR);
    opnode->setName(op);
    opnode->appendChild(operand1);
    if (operand2) opnode->appendChild(operand2);
    if (operand3) opnode->appendChild(operand3);
    return opnode;
}

FunctionElement *createFunction(const char *funcname, ASTNode *arg1, ASTNode *arg2, ASTNode *arg3, ASTNode *arg4, ASTNode *arg5, ASTNode *arg6, ASTNode *arg7, ASTNode *arg8, ASTNode *arg9, ASTNode *arg10)
{
    FunctionElement *funcnode = (FunctionElement *)createNedElementWithTag(NED_FUNCTION);
    funcnode->setName(funcname);
    if (arg1) funcnode->appendChild(arg1);
    if (arg2) funcnode->appendChild(arg2);
    if (arg3) funcnode->appendChild(arg3);
    if (arg4) funcnode->appendChild(arg4);
    if (arg5) funcnode->appendChild(arg5);
    if (arg6) funcnode->appendChild(arg6);
    if (arg7) funcnode->appendChild(arg7);
    if (arg8) funcnode->appendChild(arg8);
    if (arg9) funcnode->appendChild(arg9);
    if (arg10) funcnode->appendChild(arg10);
    return funcnode;
}

ExpressionElement *createExpression(ASTNode *expr)
{
    ExpressionElement *expression = (ExpressionElement *)createNedElementWithTag(NED_EXPRESSION);
    expression->appendChild(expr);
    return expression;
}

IdentElement *createIdent(YYLTYPE parampos)
{
    IdentElement *ident = (IdentElement *)createNedElementWithTag(NED_IDENT);
    ident->setName(toString(parampos));
    return ident;
}

IdentElement *createIdent(YYLTYPE parampos, YYLTYPE modulepos, ASTNode *moduleindexoperand)
{
    IdentElement *ident = (IdentElement *)createNedElementWithTag(NED_IDENT);
    ident->setName(toString(parampos));
    ident->setModule(toString(modulepos));
    if (moduleindexoperand)
        ident->appendChild(moduleindexoperand);
    return ident;
}

LiteralElement *createPropertyValue(YYLTYPE textpos)  // which is a spec or a string literal
{
    np->getSource()->trimSpaceAndComments(textpos);

    bool isString = false;
    try {
        const char *text = toString(textpos);
        while (opp_isspace(*text))
            text++;
        if (*text == '"') {
            const char *endp;
            opp_parsequotedstr(text, endp);
            if (*endp == '\0')
                isString = true;
        }
    }
    catch (std::exception& e) {  /*not string*/
    }
    if (isString)
        return createStringLiteral(textpos);
    else
        return createLiteral(NED_CONST_SPEC, textpos, textpos);
}

LiteralElement *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos)
{
    LiteralElement *c = (LiteralElement *)createNedElementWithTag(NED_LITERAL);
    c->setType(type);
    c->setValue(opp_trim(toString(valuepos)).c_str());
    c->setText(toString(textpos));
    return c;
}

LiteralElement *createLiteral(int type, const char *value, const char *text)
{
    LiteralElement *c = (LiteralElement *)createNedElementWithTag(NED_LITERAL);
    c->setType(type);
    c->setValue(value);
    c->setText(text);
    return c;
}

LiteralElement *createStringLiteral(YYLTYPE textpos)
{
    LiteralElement *c = (LiteralElement *)createNedElementWithTag(NED_LITERAL);
    c->setType(NED_CONST_STRING);

    const char *text = toString(textpos);
    c->setText(text);

    try {
        std::string value = opp_parsequotedstr(text);
        c->setValue(value.c_str());
    }
    catch (std::exception& e) {
        np->error(e.what(), pos.li);
    }
    return c;
}

LiteralElement *createQuantityLiteral(YYLTYPE textpos)
{
    LiteralElement *c = (LiteralElement *)createNedElementWithTag(NED_LITERAL);
    c->setType(NED_CONST_QUANTITY);

    const char *text = toString(textpos);
    c->setText(text);
    c->setValue(text);

    try {
        // validate syntax and unit compatibility ("5s 2kg")
        std::string unit;
        UnitConversion::parseQuantity(text, unit);
    }
    catch (std::exception& e) {
        np->error(e.what(), pos.li);
    }
    return c;
}

ASTNode *unaryMinus(ASTNode *node)
{
    // if not a constant, must apply unary minus operator
    if (node->getTagCode() != NED_LITERAL)
        return createOperator("-", node);

    LiteralElement *constNode = (LiteralElement *)node;

    if (constNode->getType() == NED_CONST_QUANTITY)
        return createOperator("-", node);

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType() != NED_CONST_INT && constNode->getType() != NED_CONST_DOUBLE) {
        char msg[140];
        sprintf(msg, "unary minus not accepted before '%.100s'", constNode->getValue());
        np->error(msg, pos.li);
        return node;
    }

    // prepend the constant with a '-'
    char *buf = new char[strlen(constNode->getValue())+2];
    buf[0] = '-';
    strcpy(buf+1, constNode->getValue());
    constNode->setValue(buf);
    constNode->setText(buf);  // for int and double literals, text and value are the same string
    delete[] buf;

    return node;
}

}  // namespace nedyyutil
}  // namespace nedxml
}  // namespace omnetpp

