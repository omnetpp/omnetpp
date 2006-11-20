//=========================================================================
//  CPAR.CC - part of
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

#include "cparimplbase.h"
#include "cproperties.h"
#include "ccomponent.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif


//FIXME longValue() etc methods in subclasses should check isInput() flag!

cParValue::cParValue()
{
}

cParValue::~cParValue()
{
}

cParValue& cParValue::operator=(const cParValue& other)
{
    cObject::operator=(other);
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

cParValue *cParValue::dup() const
{
    throw new cRuntimeError(this, eCANTDUP);  // cannot dup because this is an abstract class
}

bool cParValue::changed()
{
    bool ret = flags & FL_HASCHANGED;
    flags &= ~FL_HASCHANGED;
    return ret;
}

bool cParValue::equals(cParValue& other)
{
    if (type()!=other.type())
        return false;

    switch (type())
    {
        case cPar::BOOL:   return boolValue()==other.boolValue();
        case cPar::LONG:   return longValue()==other.longValue();
        case cPar::DOUBLE: return doubleValue()==other.doubleValue();
        case cPar::STRING: return stringValue()==other.stringValue();
        case cPar::XML:    return xmlValue()==other.xmlValue();
        default: return false;
    }
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

