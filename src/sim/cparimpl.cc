//=========================================================================
//  CPARIMPL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cparimpl.h"
#include "cproperties.h"
#include "ccomponent.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

long cParImpl::total_parimpl_objs;
long cParImpl::live_parimpl_objs;
cStringPool cParImpl::unitStringPool;


cParImpl::cParImpl()
{
    unitp = NULL;
    total_parimpl_objs++;
    live_parimpl_objs++;
}

cParImpl::~cParImpl()
{
    unitStringPool.release(unitp);
    live_parimpl_objs--;
}

cParImpl& cParImpl::operator=(const cParImpl& other)
{
    bool shared = isShared();
    cNamedObject::operator=(other);
    setIsShared(shared); // preserve FL_ISSHARED flag
    setUnit(other.unit());
    return *this;
}

void cParImpl::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cParImpl::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

std::string cParImpl::info() const
{
    return str();
}

std::string cParImpl::detailedInfo() const
{
    std::stringstream out;
    out << cPar::typeName(type()) << " " << name();
    if (hasValue())
    {
        if (isInput())
            out << " = default(" << str() << ")";
        else
            out << " = " << str();
        out << " isExpression=" << (isExpression()?"true":"false");
    }
    else
    {
        out << " (unassigned)";
    }
    return out.str();
}

cParImpl *cParImpl::dup() const
{
    throw cRuntimeError(this, eCANTDUP);  // cannot instantiate an abstract class
}

const char *cParImpl::unit() const
{
    return unitp;
}

void cParImpl::setUnit(const char *s)
{
    unitStringPool.release(unitp);
    unitp = unitStringPool.get(s);
}

bool cParImpl::containsConstSubexpressions() const
{
    cExpression *expr = expression();
    return expr==NULL ? false : expr->containsConstSubexpressions();
}

void cParImpl::evaluateConstSubexpressions(cComponent *context)
{
    cExpression *expr = expression();
    if (expr)
        expr->evaluateConstSubexpressions(context);
}

int cParImpl::compare(const cParImpl *other) const
{
    int res = strcmp(name(), other->name());
    if (res!=0)
        return res;

    unsigned short flags2 = flags & ~FL_ISSHARED; // ignore "isShared" flag
    unsigned short otherflags2 = other->flags & ~FL_ISSHARED;
    if (flags2!=otherflags2)
        return flags2 < otherflags2 ? -1 : 1;

    if (type()!=other->type())
        return type() < other->type() ? -1 : 1;

    return opp_strcmp(unitp, other->unitp);
}

//----
#include "cboolparimpl.h"
#include "clongparimpl.h"
#include "cdoubleparimpl.h"
#include "cstringparimpl.h"
#include "cxmlparimpl.h"

cParImpl *cParImpl::createWithType(Type type)
{
    switch (type)
    {
        case cPar::BOOL:    return new cBoolParImpl();
        case cPar::DOUBLE:  return new cDoubleParImpl();
        case cPar::LONG:    return new cLongParImpl();
        case cPar::STRING:  return new cStringParImpl();
        case cPar::XML:     return new cXMLParImpl();
        default: throw cRuntimeError("cParImpl::createWithType(): no such type: %d", type);
    }
}

