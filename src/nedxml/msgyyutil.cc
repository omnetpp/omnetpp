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

static MsgAstNodeFactory factory;

ASTNode *createMsgElementWithTag(ParseContext *np, int tagcode, ASTNode *parent)
{
    return createElementWithTag(np, &factory, tagcode, parent);
}

ASTNode *getOrCreateMsgElementWithTag(ParseContext *np, int tagcode, ASTNode *parent)
{
    return getOrCreateElementWithTag(np, &factory, tagcode, parent);
}

//
// Properties
//
static LiteralElement *createLiteral(ParseContext *np, int type, YYLoc valuepos, YYLoc textpos)
{
    LiteralElement *c = (LiteralElement *)createMsgElementWithTag(np, MSG_LITERAL);
    c->setType(type);
    c->setValue(opp_trim(toString(np, valuepos)).c_str());
    c->setText(toString(np, textpos));
    return c;
}

static LiteralElement *createStringLiteral(ParseContext *np, YYLoc textpos)
{
    LiteralElement *c = (LiteralElement *)createMsgElementWithTag(np, MSG_LITERAL);
    c->setType(LIT_STRING);

    const char *text = toString(np, textpos);
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

PropertyElement *addProperty(ParseContext *np, ASTNode *node, const char *name)
{
    PropertyElement *prop = (PropertyElement *)createMsgElementWithTag(np, MSG_PROPERTY, node);
    prop->setName(name);
    return prop;
}

PropertyElement *storeSourceCode(ParseContext *np, ASTNode *node, YYLoc tokenpos)
{
    PropertyElement *prop = addProperty(np, node, "sourcecode");
    prop->setIsImplicit(true);
    PropertyKeyElement *propkey = (PropertyKeyElement *)createMsgElementWithTag(np, MSG_PROPERTY_KEY, prop);
    propkey->appendChild(createLiteral(np, LIT_STRING, tokenpos, makeEmptyYYLoc()));  // don't store it twice
    return prop;
}

LiteralElement *createPropertyValue(ParseContext *np, YYLoc textpos)  // which is a spec or a string literal
{
    np->getSource()->trimSpaceAndComments(textpos);

    bool isString = false;
    try {
        const char *text = toString(np, textpos);
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
        return createStringLiteral(np, textpos);
    else
        return createLiteral(np, LIT_SPEC, textpos, textpos);
}

//
// Comments
//
void addComment(ParseContext *np, ASTNode *node, const char *locId, const char *text, const char *defaultValue)
{
    // don't store empty string or the default
    if (!text[0] || strcmp(text, defaultValue) == 0)
        return;

    CommentElement *comment = (CommentElement *)createMsgElementWithTag(np, MSG_COMMENT);
    comment->setLocid(locId);
    comment->setContent(text);
    node->insertChildBefore(node->getFirstChild(), comment);
}

void storeFileComment(ParseContext *np, ASTNode *node)
{
    addComment(np, node, "banner", np->getSource()->getFileComment(), "");
}

void storeBannerComment(ParseContext *np, ASTNode *node, YYLoc tokenpos)
{
    addComment(np, node, "banner", np->getSource()->getBannerComment(tokenpos), "");
}

void storeRightComment(ParseContext *np, ASTNode *node, YYLoc tokenpos)
{
    addComment(np, node, "right", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeTrailingComment(ParseContext *np, ASTNode *node, YYLoc tokenpos)
{
    addComment(np, node, "trailing", np->getSource()->getTrailingComment(tokenpos), "\n");
}

void storeBannerAndRightComments(ParseContext *np, ASTNode *node, YYLoc pos)
{
    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(np, node, pos);
    storeRightComment(np, node, pos);
    storeInnerComments(np, node, pos);
}

void storeBannerAndRightComments(ParseContext *np, ASTNode *node, YYLoc firstpos, YYLoc lastpos)
{
    YYLoc pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;

    np->getSource()->trimSpaceAndComments(pos);
    storeBannerComment(np, node, pos);
    storeRightComment(np, node, pos);
}

void storeInnerComments(ParseContext *np, ASTNode *node, YYLoc pos)
{
    for (;;) {
        const char *comment = np->getSource()->getNextInnerComment(pos);  // updates "pos"
        if (!comment)
            break;
        addComment(np, node, "inner", comment, "");
    }
}

}  // namespace msgyyutil
}  // namespace nedxml
}  // namespace omnetpp

