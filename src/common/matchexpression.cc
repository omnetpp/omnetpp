//==========================================================================
//  MATCHEXPRESSION.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "matchexpression.h"
#include "patternmatcher.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

MatchExpression::Elem::Elem(PatternMatcher *pattern, const char *fieldname)
{
    type = fieldname == nullptr ? PATTERN : FIELDPATTERN;
    this->fieldname = fieldname == nullptr ? "" : fieldname;
    this->pattern = pattern;
}

MatchExpression::Elem::~Elem()
{
    if (type == PATTERN || type == FIELDPATTERN)
        delete pattern;
}

void MatchExpression::Elem::operator=(const Elem& other)
{
    type = other.type;
    if (type == PATTERN || type == FIELDPATTERN) {
        fieldname = other.fieldname;
        pattern = new PatternMatcher(*(other.pattern));
    }
}

MatchExpression::MatchExpression()
{
}

MatchExpression::MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

void MatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    this->matchDottedPath = dottedpath;
    this->matchFullString = fullstring;
    this->caseSensitive = casesensitive;

    elems.clear();

    Assert(pattern);
    if (*pattern)
        parsePattern(elems, pattern, dottedpath, fullstring, casesensitive);
}

bool MatchExpression::matches(const Matchable *object)
{
    if (elems.empty())
        return false;

    const int stksize = 20;
    bool stk[stksize];

    int tos = -1;
    for (auto & e : elems) {
        const char *attr;
        switch (e.type) {
            case Elem::PATTERN:
                if (tos >= stksize-1)
                    throw opp_runtime_error("MatchExpression: Malformed expression: Stack overflow");
                attr = object->getAsString();
                stk[++tos] = attr == nullptr ? false : e.pattern->matches(attr);
                break;

            case Elem::FIELDPATTERN:
                if (tos >= stksize-1)
                    throw opp_runtime_error("MatchExpression: Malformed expression: Stack overflow");
                attr = object->getAsString(e.fieldname.c_str());
                stk[++tos] = attr == nullptr ? false : e.pattern->matches(attr);
                break;

            case Elem::OR:
                if (tos < 1)
                    throw opp_runtime_error("MatchExpression: Malformed expression: Stack underflow");
                stk[tos-1] = stk[tos-1] || stk[tos];
                tos--;
                break;

            case Elem::AND:
                if (tos < 1)
                    throw opp_runtime_error("MatchExpression: Malformed expression: Stack underflow");
                stk[tos-1] = stk[tos-1] && stk[tos];
                tos--;
                break;

            case Elem::NOT:
                if (tos < 0)
                    throw opp_runtime_error("MatchExpression: Malformed expression: Stack underflow");
                stk[tos] = !stk[tos];
                break;

            default:
                throw opp_runtime_error("MatchExpression: Malformed expression: Unknown element type");
        }
    }
    if (tos != 0)
        throw opp_runtime_error("MatchExpression: Malformed expression: %d items left on stack", tos);

    return stk[tos];
}

const char *MatchableStringMap::getAsString(const char *attribute) const
{
    std::map<std::string,std::string>::const_iterator it = stringMap.find(attribute);
    return it == stringMap.end() ? nullptr : it->second.c_str();
}

}  // namespace common
}  // namespace omnetpp

