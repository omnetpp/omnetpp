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
#include "yyutil.h"
#include "sourcedocument.h"
#include "yydefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

// this global var is shared by all lexers
LineColumn pos, prevpos;

std::string slashifyFilename(const char *fname)
{
    std::string fnamewithslash = np->getFileName();
    for (char *s = const_cast<char *>(fnamewithslash.data()); *s; s++)
        if (*s == '\\')
            *s = '/';

    return fnamewithslash;
}

const char *toString(YYLTYPE pos)
{
    return np->getSource()->get(pos);
}

const char *toString(long l)
{
    static char buf[32];
    sprintf(buf, "%ld", l);
    return buf;
}

std::string removeSpaces(YYLTYPE pos)
{
    const char *s = np->getSource()->get(pos);
    std::string result;
    for ( ; *s; s++)
        if (!opp_isspace(*s))
            result += *s;

    return result;
}

const char *currentLocation()
{
    static char buf[200];
    sprintf(buf, "%s:%d", np->getFileName(), pos.li);
    return buf;
}

ASTNode *createElementWithTag(ASTNodeFactory *factory, int tagcode, ASTNode *parent)
{
    ASTNode *e = factory->createElementWithTag(tagcode);
    e->setSourceLocation(currentLocation());
    if (parent)
        parent->appendChild(e);
    return e;
}

ASTNode *getOrCreateElementWithTag(ASTNodeFactory *factory, int tagcode, ASTNode *parent)
{
    assert(parent);
    ASTNode *e = parent->getFirstChildWithTag(tagcode);
    return e != nullptr ? e : createElementWithTag(factory, tagcode, parent);
}

void storePos(ASTNode *node, YYLTYPE pos)
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

void storePos(ASTNode *node, YYLTYPE firstpos, YYLTYPE lastpos)
{
    YYLTYPE pos = firstpos;
    pos.last_line = lastpos.last_line;
    pos.last_column = lastpos.last_column;
    storePos(node, pos);
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

