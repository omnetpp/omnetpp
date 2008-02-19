//=========================================================================
//  CPARVALUE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cparvalue.h"
#include "cproperties.h"
#include "ccomponent.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

long cParValue::total_parvalue_objs;
long cParValue::live_parvalue_objs;
cStringPool cParValue::unitStringPool;


cParValue::cParValue()
{
    unitp = NULL;
    total_parvalue_objs++;
    live_parvalue_objs++;
}

cParValue::~cParValue()
{
    unitStringPool.release(unitp);
    live_parvalue_objs--;
}

cParValue& cParValue::operator=(const cParValue& other)
{
    cNamedObject::operator=(other);
    setUnit(other.unit());
    return *this;
}

void cParValue::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cParValue::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

std::string cParValue::info() const
{
    return toString();
}

std::string cParValue::detailedInfo() const
{
    std::stringstream out;
    out << cPar::typeName(type()) << " " << name();
    if (hasValue())
    {
        if (isInput())
            out << " = default(" << toString() << ")";
        else
            out << " = " << toString();
        out << " isExpression=" << (isExpression()?"true":"false");
    }
    else
    {
        out << " (unassigned)";
    }
    return out.str();
}

cParValue *cParValue::dup() const
{
    throw cRuntimeError(this, eCANTDUP);  // cannot dup because this is an abstract class
}

const char *cParValue::unit() const
{
    return unitp;
}

void cParValue::setUnit(const char *s)
{
    unitStringPool.release(unitp);
    unitp = unitStringPool.get(s);
}

int cParValue::compare(const cParValue *other) const
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
#include "cboolpar.h"
#include "clongpar.h"
#include "cdoublepar.h"
#include "cstringpar.h"
#include "cxmlpar.h"

cParValue *cParValue::createWithType(Type type)
{
    switch (type)
    {
        case cPar::BOOL:    return new cBoolPar();
        case cPar::DOUBLE:  return new cDoublePar();
        case cPar::LONG:    return new cLongPar();
        case cPar::STRING:  return new cStringPar();
        case cPar::XML:     return new cXMLPar();
        default: throw cRuntimeError("cParValue::createWithType(): no such type: %d", type);
    }
}

