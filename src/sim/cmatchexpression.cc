//==========================================================================
//  CMATCHEXPRESSION.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "common/matchexpression.h"
#include "omnetpp/cmatchexpression.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {

#define TRY(code)   try { code; } catch (std::exception& e) { throw cRuntimeError("cMatchExpression: %s", removePrefix(e.what())); }

const char *removePrefix(const char *s)
{
    const char *prefix = "MatchExpression: ";
    return strncmp(s, prefix, strlen(prefix))==0 ? s+strlen(prefix) : s;
}

cMatchExpression::cMatchExpression() : impl(new MatchExpression())
{
}

cMatchExpression::cMatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    TRY(impl = new MatchExpression(pattern, dottedpath, fullstring, casesensitive));
}

cMatchExpression::~cMatchExpression()
{
    delete impl;
}

void cMatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    TRY(impl->setPattern(pattern, dottedpath, fullstring, casesensitive));
}

class Wrapper : public MatchExpression::Matchable
{
  private:
    const cMatchExpression::Matchable *impl;

  public:
    Wrapper(const cMatchExpression::Matchable *p): impl(p) { }
    virtual const char *getAsString() const override { return impl->getAsString(); }
    virtual const char *getAsString(const char *attribute) const override { return impl->getAsString(attribute); }
};

bool cMatchExpression::matches(const Matchable *object) const
{
    Wrapper wrapper(object);
    TRY(return impl->matches(&wrapper));
}

}  // namespace omnetpp

