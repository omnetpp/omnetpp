//==========================================================================
// nedyylib.cc  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include "nedyylib.h"
#include "nedfilebuffer.h"
#include "nedyydefs.h"
#include "stringutil.h"
#include "unitconversion.h"


// this global var is shared by all lexers
LineColumn pos, prevpos;

std::string slashifyFilename(const char *fname)
{
    std::string fnamewithslash = np->getFileName();
    for (char *s=const_cast<char *>(fnamewithslash.data()); *s; s++)
        if (*s=='\\')
            *s='/';
    return fnamewithslash;
}

const char *toString(YYLTYPE pos)
{
    return np->getSource()->get(pos);
}

const char *toString(long l)
{
    static char buf[32];
    sprintf(buf,"%ld", l);
    return buf;
}

const char *currentLocation()
{
    static char buf[200];
    sprintf(buf,"%s:%d", np->getFileName(), pos.li);
    return buf;
}

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent)
{
    // create via a factory
    NEDElement *e = NEDElementFactory::getInstance()->createNodeWithTag(tagcode);
    e->setSourceLocation(currentLocation());

    // add to parent
    if (parent)
       parent->appendChild(e);

    return e;
}

void storePos(NEDElement *node, YYLTYPE pos)
{
    np->getSource()->trimSpaceAndComments(pos);

    assert(node);
    NEDSourceRegion region;
    region.startLine = pos.first_line;
    region.startColumn = pos.first_column;
    region.endLine = pos.last_line;
    region.endColumn = pos.last_column;
    node->setSourceRegion(region);
}

void storePos(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;
    storePos(node, pos);
}

//
// Properties
//

PropertyNode *addProperty(NEDElement *node, const char *name)
{
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyNode *addComponentProperty(NEDElement *node, const char *name)
{
    // add propery under the ParametersNode; create that if not yet exists
    NEDElement *params = node->getFirstChildWithTag(NED_PARAMETERS);
    if (!params)
    {
        params = createNodeWithTag(NED_PARAMETERS, node);

        // move parameters section in front of potential gates, types, etc sections
        NEDElement *prev;
        while ((prev=params->getPrevSibling())!=NULL &&
                 (prev->getTagCode()==NED_GATES || prev->getTagCode()==NED_TYPES ||
                  prev->getTagCode()==NED_SUBMODULES || prev->getTagCode()==NED_CONNECTIONS))
        {
            node->removeChild(params);
            node->insertChildBefore(prev, params);
        }
    }
    PropertyNode *prop = (PropertyNode *)createNodeWithTag(NED_PROPERTY, params);
    prop->setName(name);
    return prop;
}

//
// Spec Properties: source code, display string
//

PropertyNode *storeSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
    PropertyNode *prop = addProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, makeEmptyYYLTYPE()));  // don't store it twice
    return prop;
}

PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
    PropertyNode *prop = addComponentProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, makeEmptyYYLTYPE()));  // don't store it twice
    return prop;
}

//
// Comments
//
void addComment(NEDElement *node, const char *locId, const char *text, const char *defaultValue)
{
    // don't store empty string or the default
    if (!text[0] || strcmp(text,defaultValue)==0)
        return;

    CommentNode *comment = (CommentNode *)createNodeWithTag(NED_COMMENT);
    comment->setLocid(locId);
    comment->setContent(text);
    node->insertChildBefore(node->getFirstChild(), comment);
}

void storeFileComment(NEDElement *node)
{
    addComment(node, "banner", np->getSource()->getFileComment(), "");
}

void storeBannerComment(NEDElement *node, YYLTYPE tokenpos)
{
    addComment(node, "banner", np->getSource()->getBannerComment(tokenpos), "");
}

void storeRightComment(NEDElement *node, YYLTYPE tokenpos)
{
    addComment(node, "right", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeTrailingComment(NEDElement *node, YYLTYPE tokenpos)
{
    addComment(node, "trailing", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeBannerAndRightComments(NEDElement *node, YYLTYPE pos)
{
    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(node, pos);
    storeRightComment(node, pos);
    storeInnerComments(node, pos);
}

void storeBannerAndRightComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(node, pos);
    storeRightComment(node, pos);
}

void storeInnerComments(NEDElement *node, YYLTYPE pos)
{
    for (;;)
    {
        const char *comment = np->getSource()->getNextInnerComment(pos); // updates "pos"
        if (!comment)
            break;
        addComment(node, "inner", comment, "");
    }
}

ParamNode *addParameter(NEDElement *params, YYLTYPE namepos)
{
    ParamNode *param = (ParamNode *)createNodeWithTag(NED_PARAM,params);
    param->setName( toString( namepos) );
    return param;
}

GateNode *addGate(NEDElement *gates, YYLTYPE namepos)
{
    GateNode *gate = (GateNode *)createNodeWithTag(NED_GATE,gates);
    gate->setName( toString( namepos) );
    return gate;
}

YYLTYPE trimBrackets(YYLTYPE vectorpos)
{
    // should check it's really brackets that get chopped off
    vectorpos.first_column++;
    vectorpos.last_column--;
    return vectorpos;
}

YYLTYPE trimAngleBrackets(YYLTYPE vectorpos)
{
    // should check it's really angle brackets that get chopped off
    vectorpos.first_column++;
    vectorpos.last_column--;
    return vectorpos;
}

YYLTYPE trimQuotes(YYLTYPE vectorpos)
{
    // should check it's really quotes that get chopped off
    vectorpos.first_column++;
    vectorpos.last_column--;
    return vectorpos;
}

YYLTYPE trimDoubleBraces(YYLTYPE vectorpos)
{
    // should check it's really '{{' and '}}' that get chopped off
    vectorpos.first_column+=2;
    vectorpos.last_column-=2;
    return vectorpos;
}

void addVector(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
    addExpression(elem, attrname, trimBrackets(exprpos), expr);
}

void addLikeParam(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
    if (np->getParseExpressionsFlag() && !expr)
        elem->setAttribute(attrname, toString(trimAngleBrackets(exprpos)));
    else
        addExpression(elem, attrname, trimAngleBrackets(exprpos), expr);
}

void addExpression(NEDElement *elem, const char *attrname, YYLTYPE exprpos, NEDElement *expr)
{
    if (np->getParseExpressionsFlag()) {
        ((ExpressionNode *)expr)->setTarget(attrname);

        // in the DTD, whilespaces and expressions are at front, insert there
        NEDElement *insertPos = elem->getFirstChild();
        while (insertPos && (insertPos->getTagCode()==NED_COMMENT || insertPos->getTagCode()==NED_EXPRESSION))
            insertPos = insertPos->getNextSibling();
        if (!insertPos)
            elem->appendChild(expr);
        else
            elem->insertChildBefore(insertPos, expr);

    } else {
        elem->setAttribute(attrname, toString(exprpos));
    }
}

void swapConnection(NEDElement *conn)
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

void swapAttributes(NEDElement *node, const char *attr1, const char *attr2)
{
    std::string oldv1(node->getAttribute(attr1));
    node->setAttribute(attr1,node->getAttribute(attr2));
    node->setAttribute(attr2,oldv1.c_str());
}

void swapExpressionChildren(NEDElement *node, const char *attr1, const char *attr2)
{
    ExpressionNode *expr1, *expr2;
    for (expr1=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr1; expr1=expr1->getNextExpressionNodeSibling())
        if (!strcmp(expr1->getTarget(),attr1))
            break;
    for (expr2=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr2; expr2=expr2->getNextExpressionNodeSibling())
        if (!strcmp(expr2->getTarget(),attr2))
            break;

    if (expr1) expr1->setTarget(attr2);
    if (expr2) expr2->setTarget(attr1);
}

void transferChildren(NEDElement *from, NEDElement *to)
{
    while (from->getFirstChild())
        to->appendChild(from->removeChild(from->getFirstChild()));
}

OperatorNode *createOperator(const char *op, NEDElement *operand1, NEDElement *operand2, NEDElement *operand3)
{
    OperatorNode *opnode = (OperatorNode *)createNodeWithTag(NED_OPERATOR);
    opnode->setName(op);
    opnode->appendChild(operand1);
    if (operand2) opnode->appendChild(operand2);
    if (operand3) opnode->appendChild(operand3);
    return opnode;
}

FunctionNode *createFunction(const char *funcname, NEDElement *arg1, NEDElement *arg2, NEDElement *arg3, NEDElement *arg4)
{
    FunctionNode *funcnode = (FunctionNode *)createNodeWithTag(NED_FUNCTION);
    funcnode->setName(funcname);
    if (arg1) funcnode->appendChild(arg1);
    if (arg2) funcnode->appendChild(arg2);
    if (arg3) funcnode->appendChild(arg3);
    if (arg4) funcnode->appendChild(arg4);
    return funcnode;
}

ExpressionNode *createExpression(NEDElement *expr)
{
    ExpressionNode *expression = (ExpressionNode *)createNodeWithTag(NED_EXPRESSION);
    expression->appendChild(expr);
    return expression;
}

IdentNode *createIdent(YYLTYPE parampos)
{
    IdentNode *ident = (IdentNode *)createNodeWithTag(NED_IDENT);
    ident->setName(toString(parampos));
    return ident;
}

IdentNode *createIdent(YYLTYPE parampos, YYLTYPE modulepos, NEDElement *moduleindexoperand)
{
    IdentNode *ident = (IdentNode *)createNodeWithTag(NED_IDENT);
    ident->setName(toString(parampos));
    ident->setModule(toString(modulepos));
    if (moduleindexoperand)
        ident->appendChild(moduleindexoperand);
    return ident;
}

LiteralNode *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos)
{
    LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
    c->setType(type);
    c->setValue(toString(valuepos));
    c->setText(toString(textpos));
    return c;
}

LiteralNode *createStringLiteral(YYLTYPE textpos)
{
    LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
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

LiteralNode *createQuantityLiteral(YYLTYPE textpos)
{
    LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
    c->setType(NED_CONST_DOUBLE);

    const char *text = toString(textpos);
    c->setText(text);

    double d = 0;
    std::string unit;

    try {
        // evaluate quantities like "5s 230ms"
        d = UnitConversion::parseQuantity(text, unit);
    }
    catch (std::exception& e) {
        np->error(e.what(), pos.li);
    }

    // convert value back to string
    char buf[32];
    sprintf(buf,"%g",d);
    c->setValue(buf);

    if (!unit.empty())
        c->setUnit(unit.c_str());
    return c;
}

NEDElement *unaryMinus(NEDElement *node)
{
    // if not a constant, must apply unary minus operator
    if (node->getTagCode()!=NED_LITERAL)
        return createOperator("-", node);

    LiteralNode *constNode = (LiteralNode *)node;

    // only int and real constants can be negative, string, bool, etc cannot
    if (constNode->getType()!=NED_CONST_INT && constNode->getType()!=NED_CONST_DOUBLE)
    {
       char msg[140];
       sprintf(msg,"unary minus not accepted before '%.100s'",constNode->getValue());
       np->error(msg, pos.li);
       return node;
    }

    // prepend the constant with a '-'
    char *buf = new char[strlen(constNode->getValue())+2];
    buf[0] = '-';
    strcpy(buf+1, constNode->getValue());
    constNode->setValue(buf);
    constNode->setText(buf); // for int and double literals, text and value are the same string
    delete [] buf;

    return node;
}



