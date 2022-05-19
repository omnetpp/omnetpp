//==========================================================================
// yyutil.cc  -
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
#include "yydefs.h"
#include "yyutil.h"
#include "errorstore.h"
#include "sourcedocument.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

bool parseInProgress = false;

// this global var is shared by all lexers
LineColumn pos;

void ParseContext::error(const char *msg, int line)
{
    std::stringstream os;
    os << filename << ":" << line;
    errors->addError(os.str().c_str(), "%s", msg);
}

std::string slashifyFilename(const char *fname)
{
    std::string fnamewithslash = fname;
    for (char *s = const_cast<char *>(fnamewithslash.data()); *s; s++)
        if (*s == '\\')
            *s = '/';

    return fnamewithslash;
}

const char *toString(ParseContext *np, YYLoc pos)
{
    return np->getSource()->get(pos);
}

const char *toString(long l)
{
    static char buf[32];
    snprintf(buf, sizeof(buf), "%ld", l);
    return buf;
}

std::string removeSpaces(ParseContext *np, YYLoc pos)
{
    const char *s = np->getSource()->get(pos);
    std::string result;
    for ( ; *s; s++)
        if (!opp_isspace(*s))
            result += *s;

    return result;
}

ASTNode *createElementWithTag(ParseContext *np, ASTNodeFactory *factory, int tagcode, ASTNode *parent)
{
    ASTNode *e = factory->createElementWithTag(tagcode);
    e->setSourceLocation(np->getFileName(), pos.li);
    if (parent)
        parent->appendChild(e);
    return e;
}

ASTNode *getOrCreateElementWithTag(ParseContext *np, ASTNodeFactory *factory, int tagcode, ASTNode *parent)
{
    assert(parent);
    ASTNode *e = parent->getFirstChildWithTag(tagcode);
    return e != nullptr ? e : createElementWithTag(np, factory, tagcode, parent);
}

void storePos(ParseContext *np, ASTNode *node, YYLoc pos)
{
    np->getSource()->trimSpaceAndComments(pos);

    assert(node);
    SourceRegion region;
    region.startLine = pos.first_line;
    region.startColumn = pos.first_column;
    region.endLine = pos.last_line;
    region.endColumn = pos.last_column;
    node->setSourceRegion(region);
}

void storePos(ParseContext *np, ASTNode *node, YYLoc firstpos, YYLoc lastpos)
{
    YYLoc pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;
    storePos(np, node, pos);
}

YYLoc trimQuotes(YYLoc vectorpos)
{
    // should check it's really quotes that get chopped off
    vectorpos.first_column++;
    vectorpos.last_column--;
    return vectorpos;
}

YYLoc trimDoubleBraces(YYLoc vectorpos)
{
    // should check it's really '{{' and '}}' that get chopped off
    vectorpos.first_column += 2;
    vectorpos.last_column -= 2;
    return vectorpos;
}

void swapAttributes(ASTNode *node, const char *attr1, const char *attr2)
{
    std::string oldv1(node->getAttribute(attr1));
    node->setAttribute(attr1, node->getAttribute(attr2));
    node->setAttribute(attr2, oldv1.c_str());
}

void transferChildren(ASTNode *from, ASTNode *to)
{
    while (from->getFirstChild())
        to->appendChild(from->removeChild(from->getFirstChild()));
}

}  // namespace nedxml
}  // namespace omnetpp

