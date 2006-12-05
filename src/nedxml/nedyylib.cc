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

NEDElement *createNodeWithTag(int tagcode, NEDElement *parent)
{
    // create via a factory
    NEDElement *e = NEDElementFactory::getInstance()->createNodeWithTag(tagcode);

    // "debug info"
    char buf[200];
    sprintf(buf,"%s:%d", np->getFileName(), pos.li);
    e->setSourceLocation(buf);

    // add to parent
    if (parent)
       parent->appendChild(e);

    return e;
}

void storePos(NEDElement *node, YYLTYPE pos)
{
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
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
    return prop;
}

PropertyNode *storeComponentSourceCode(NEDElement *node, YYLTYPE tokenpos)
{
    PropertyNode *prop = addComponentProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyNode *propkey = (PropertyKeyNode *)createNodeWithTag(NED_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(NED_CONST_STRING, tokenpos, tokenpos));  // FIXME don't store it twice
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
    storeBannerComment(node, pos);
    storeRightComment(node, pos);
    storeInnerComments(node, pos);
}

void storeBannerAndRightComments(NEDElement *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

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

//XXX find correct place for this stuff
enum UnitType {UNIT_NONE, TIME_SECONDS, DATARATE_BPS, DATA_BYTES, DISTANCE_METERS };
struct Unit { const char *name; double mult; UnitType type; };
Unit unitTable[] = {
    { "d",   86400, TIME_SECONDS },
    { "h",    3600, TIME_SECONDS },
    { "mi",     60, TIME_SECONDS },
    { "s",       1, TIME_SECONDS },
    { "ms",   1e-3, TIME_SECONDS },
    { "us",   1e-9, TIME_SECONDS },
    { "ns",  1e-12, TIME_SECONDS },
    { "Tbps", 1e12, DATARATE_BPS },
    { "Gbps",  1e9, DATARATE_BPS },
    { "Mbps",  1e6, DATARATE_BPS },
    { "Kbps",  1e3, DATARATE_BPS },
    { "bps",     1, DATARATE_BPS },
    { "TB",   1e12, DATA_BYTES },
    { "GB",    1e9, DATA_BYTES },
    { "MB",    1e6, DATA_BYTES },
    { "KB",    1e3, DATA_BYTES },
    { "B",       1, DATA_BYTES },
    { "km",    1e3, DISTANCE_METERS },
    { "m",       1, DISTANCE_METERS },
    { "cm",   1e-2, DISTANCE_METERS },
    { "mm",   1e-3, DISTANCE_METERS },
    //XXX stop here? or we have to do watts, Hz, m/s, bauds, 1/s, dB, ...
    { NULL,      0, UNIT_NONE }
};

double parseQuantity(const char *str)  //XXX error handling? return unit type?
{
    double d = 0;
    UnitType type = UNIT_NONE;
    while (*str!='\0')
    {
        // read number into num and skip it
        double num;
        int len;
        while (isspace(*str)) str++;
        if (0==sscanf(str, "%lf%n", &num, &len))
            break; // break if error
        str+=len;

        // is there a unit coming?
        while (isspace(*str)) str++;
        if (!isalpha(*str))
        {
            d += num;
            break; // nothing can come after a plain number  XXX revise
        }

        // extract unit
        char unit[16];
        int i;
        for (i=0; i<15 && isalpha(str[i]); i++)
            unit[i] = str[i];
        if (i==16)
            break; // error: unit name too long XXX revise
        unit[i] = '\0';

        // look up and apply unit
        for (i=0; unitTable[i].name; i++)
            if (!strcmp(unitTable[i].name, unit))
                break;
        if (!unitTable[i].name)
            break; // error: unit not in table  XXX revise
        if (type!=UNIT_NONE && unitTable[i].type!=type)
            break; // error: unit mismatch   XXX revise
        type = unitTable[i].type;

        d += unitTable[i].mult * num;
    }
    return d;
}

/*
simtime_t strToSimtime(const char *str)
{
    while (*str==' ' || *str=='\t') str++;
    if (*str=='\0') return -1; // empty string not accepted
    simtime_t d = strToSimtime0( str );
    return (*str=='\0') ? d : -1; // OK if whole string could be interpreted
}

simtime_t strToSimtime0(const char *&str)
{
    double simtime = 0;

    while (*str!='\0')
    {
          // read number into num and skip it
          double num;
          int len;
          while (*str==' ' || *str=='\t') str++;
          if (0==sscanf(str, "%lf%n", &num, &len)) break; // break if error
          str+=len;

          // process time unit: d,h,m,s,ms,us,ns
          while (*str==' ' || *str=='\t') str++;
          if (str[0]=='n' && str[1]=='s')
                          {simtime += 1e-9*num; str+=2;}
          else if (str[0]=='u' && str[1]=='s')
                          {simtime += 1e-6*num; str+=2;}
          else if (str[0]=='m' && str[1]=='s')
                          {simtime += 1e-3*num; str+=2;}
          else if (str[0]=='s')
                          {simtime += num; str++;}
          else if (str[0]=='m')
                          {simtime += 60*num; str++;}
          else if (str[0]=='h')
                          {simtime += 3600*num; str++;}
          else if (str[0]=='d')
                          {simtime += 24*3600*num; str++;}
          else
                          {simtime += num; break;} // nothing can come after 'pure' number
    }
    return (simtime_t)simtime;
}
*/

LiteralNode *createQuantity(const char *text)
{
    LiteralNode *c = (LiteralNode *)createNodeWithTag(NED_LITERAL);
    c->setType(NED_CONST_UNIT);
    if (text) c->setText(text);

    double d = parseQuantity(text);
    if (d<0)
    {
        char msg[130];
        sprintf(msg,"invalid constant '%.100s'",text);
        np->error(msg, pos.li);
    }
    char buf[32];
    sprintf(buf,"%g",d);
    c->setValue(buf);

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


