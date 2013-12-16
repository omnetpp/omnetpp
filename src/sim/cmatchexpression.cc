//==========================================================================
//  CMATCHEXPRESSION.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "cmatchexpression.h"
#include "../common/matchexpression.h"

NAMESPACE_BEGIN

cMatchExpression::cMatchExpression()
{
    impl = new MatchExpression();
}

cMatchExpression::~cMatchExpression()
{
    delete impl;
}

cMatchExpression::cMatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    impl = new MatchExpression(pattern, dottedpath, fullstring, casesensitive);
}

void cMatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    impl->setPattern(pattern, dottedpath, fullstring, casesensitive);
}

class Wrapper : public MatchExpression::Matchable
{
  private:
    const cMatchExpression::Matchable *impl;
  public:
    Wrapper(const cMatchExpression::Matchable *p) {impl = p;}
    virtual const char *getAsString() const {return impl->getAsString();}
    virtual const char *getAsString(const char *attribute) const {return impl->getAsString(attribute);}
};

bool cMatchExpression::matches(const Matchable *object)
{
    Wrapper wrapper(object);
    return impl->matches(&wrapper);
}

NAMESPACE_END
