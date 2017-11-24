//==========================================================================
// msgyyutil.cc  -
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
#include "msgyyutil.h"
#include "sourcedocument.h"
#include "yydefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {
namespace msgyyutil { // for now

static MSGAstNodeFactory factory;

ASTNode *createMsgElementWithTag(int tagcode, ASTNode *parent)
{
    return createElementWithTag(&factory, tagcode, parent);
}

ASTNode *getOrCreateMsgElementWithTag(int tagcode, ASTNode *parent)
{
    return getOrCreateElementWithTag(&factory, tagcode, parent);
}

//
// Properties
//
static LiteralElement *createLiteral(int type, YYLTYPE valuepos, YYLTYPE textpos)
{
    LiteralElement *c = (LiteralElement *)createMsgElementWithTag(MSG_LITERAL);
    c->setType(type);
    c->setValue(opp_trim(toString(valuepos)).c_str());
    c->setText(toString(textpos));
    return c;
}

static LiteralElement *createStringLiteral(YYLTYPE textpos)
{
    LiteralElement *c = (LiteralElement *)createMsgElementWithTag(MSG_LITERAL);
    c->setType(LIT_STRING);

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

PropertyElement *addProperty(ASTNode *node, const char *name)
{
    PropertyElement *prop = (PropertyElement *)createMsgElementWithTag(MSG_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyElement *storeSourceCode(ASTNode *node, YYLTYPE tokenpos)
{
    PropertyElement *prop = addProperty(node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyElement *propkey = (PropertyKeyElement *)createMsgElementWithTag(MSG_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(LIT_STRING, tokenpos, makeEmptyYYLTYPE()));  // don't store it twice
    return prop;
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
        return createLiteral(LIT_SPEC, textpos, textpos);
}

//
// Comments
//
void addComment(ASTNode *node, const char *locId, const char *text, const char *defaultValue)
{
    // don't store empty string or the default
    if (!text[0] || strcmp(text, defaultValue) == 0)
        return;

    CommentElement *comment = (CommentElement *)createMsgElementWithTag(MSG_COMMENT);
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

}  // namespace msgyyutil
}  // namespace nedxml
}  // namespace omnetpp

