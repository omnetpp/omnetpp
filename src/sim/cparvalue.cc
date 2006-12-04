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

long cParValue::total_parvalue_objs;
long cParValue::live_parvalue_objs;

cParValue::cParValue()
{
    total_parvalue_objs++;
    live_parvalue_objs++;
}

cParValue::~cParValue()
{
    live_parvalue_objs--;
}

cParValue& cParValue::operator=(const cParValue& other)
{
    cNamedObject::operator=(other);
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
    throw new cRuntimeError(this, eCANTDUP);  // cannot dup because this is an abstract class
}

bool cParValue::equals(cParValue& other, cComponent *thiscontext, cComponent *othercontext)
{
    if (type()!=other.type())
        return false;

    switch (type())
    {
        case cPar::BOOL:   return boolValue(thiscontext)==other.boolValue(othercontext);
        case cPar::LONG:   return longValue(thiscontext)==other.longValue(othercontext);
        case cPar::DOUBLE: return doubleValue(thiscontext)==other.doubleValue(othercontext);
        case cPar::STRING: return stringValue(thiscontext)==other.stringValue(othercontext);
        case cPar::XML:    return xmlValue(thiscontext)==other.xmlValue(othercontext);
        default: return false;
    }
}

int cParValue::compare(const cParValue *other) const
{
    int res = strcmp(name(), other->name());
    if (res!=0)
        return res;
    if (flags!=other->flags)
        return flags < other->flags ? -1 : 1;
    if (type()!=other->type())
        return type() < other->type() ? -1 : 1;
    return 0;
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
        default: throw new cRuntimeError("cParValue::createWithType(): no such type: %d", type);
    }
}

