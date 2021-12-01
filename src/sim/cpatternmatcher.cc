//==========================================================================
//  CPATTERNMATCHER.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/patternmatcher.h"
#include "omnetpp/cpatternmatcher.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {

cPatternMatcher::cPatternMatcher() : impl(new PatternMatcher())
{
}

cPatternMatcher::cPatternMatcher(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive) : impl(new PatternMatcher())
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

cPatternMatcher::~cPatternMatcher()
{
    delete impl;
}

void cPatternMatcher::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    try {
        impl->setPattern(pattern, dottedpath, fullstring, casesensitive);
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s", e.what()); // allow debug-on-error
    }
}

bool cPatternMatcher::matches(const char *line) const
{
    return impl->matches(line);
}

const char *cPatternMatcher::patternPrefixMatches(const char *line, int suffixoffset) const
{
    return impl->patternPrefixMatches(line, suffixoffset);
}

std::string cPatternMatcher::debugStr() const
{
    return impl->debugStr();
}

void cPatternMatcher::dump() const
{
    impl->dump();
}

bool cPatternMatcher::containsWildcards(const char *pattern)
{
    return PatternMatcher::containsWildcards(pattern);
}

}  // namespace omnetpp

