//========================================================================
//  COUTVECTOR.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cOutVector         : represents a vector in the output file
//
//   Original version:  Gabor Lencse
//   Rewrite, bugfixes: Andras Varga
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>   // strlen
#include "globals.h"
#include "coutvector.h"
#include "cmodule.h"
#include "csimulation.h"
#include "cenvir.h"
#include "cexception.h"
#include "cenum.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

Register_Class(cOutVector);


Register_Enum(cOutVector::Type, (cOutVector::TYPE_INT, cOutVector::TYPE_DOUBLE, cOutVector::TYPE_ENUM) );
Register_Enum(cOutVector::InterpolationMode, (cOutVector::NONE, cOutVector::SAMPLE_HOLD, cOutVector::BACKWARD_SAMPLE_HOLD, cOutVector::LINEAR));


cOutVector::cOutVector(const char *name) : cNoncopyableOwnedObject(name)
{
    enabled = true;
    handle = NULL;
    num_received = 0;
    num_stored = 0;
    record_in_inspector = NULL;
    last_t = 0;

    // register early if possible (only required by Akaroa)
    if (name)
        handle = ev.registerOutputVector(simulation.context()->fullPath().c_str(), name);
}

cOutVector::~cOutVector()
{
    if (handle)
        ev.deregisterOutputVector(handle);
}

void cOutVector::setName(const char *nam)
{
    if (handle)
        throw cRuntimeError(this,"setName(): changing name of an output vector after record() calls is not allowed");
    cOwnedObject::setName(nam);

    // register early (only needed for Akaroa...)
    if (nam)
        handle = ev.registerOutputVector(simulation.context()->fullPath().c_str(), name());
}

std::string cOutVector::info() const
{
    if (!handle)
        return std::string("(no values recorded yet)");
    std::stringstream out;
    out << "received " << num_received << " values, stored " << num_stored;
    return out.str();
}

void cOutVector::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "netPack() not supported");
}

void cOutVector::netUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "netUnpack(): not supported");
}

void cOutVector::setUnit(const char *unit)
{
    ev.setVectorAttribute(handle, "unit", unit);
}

//FIXME crashes when called before setName()
//FIXME enum name doesn't get written out to the file
void cOutVector::setEnum(const char *registeredEnumName)
{
    cEnum *enumDecl = cEnum::find(registeredEnumName);
    if (!enumDecl)
        throw cRuntimeError(this, "setEnum(): enum `%s' not found -- is it declared with Register_Enum()?", registeredEnumName);
    setEnum(enumDecl);
}

void cOutVector::setEnum(cEnum *enumDecl)  //XXX
{
    ev.setVectorAttribute(handle, "enum", enumDecl->toString().c_str());
}

void cOutVector::setType(Type type)
{
    const char *typeString=NULL;
    switch (type)
    {
        case TYPE_INT:    typeString = "int"; break;
        case TYPE_DOUBLE: typeString = "double"; break;
        case TYPE_ENUM:   typeString = "enum"; break;
        //Note: no "default:" so that compiler can warn of incomplete coverage
    }
    if (!typeString)
        throw cRuntimeError(this, "setType(): invalid type %d", type);
    ev.setVectorAttribute(handle, "type", typeString);
}

void cOutVector::setInterpolationMode(InterpolationMode mode)
{
    const char *modeString=NULL;
    switch (mode)
    {
        case NONE:                 modeString = "none"; break;
        case SAMPLE_HOLD:          modeString = "sample-hold"; break;
        case BACKWARD_SAMPLE_HOLD: modeString = "backward-sample-hold"; break;
        case LINEAR:               modeString = "linear"; break;
        //Note: no "default:" so that compiler can warn of incomplete coverage
    }
    if (!modeString)
        throw cRuntimeError(this, "setInterpolationMode(): invalid interpolation mode %d", mode);
    ev.setVectorAttribute(handle, "interpolationmode", modeString);
}

void cOutVector::setMin(double minValue)
{
    char buf[32];
    sprintf(buf, "%g", minValue);
    ev.setVectorAttribute(handle, "min", buf);
}

void cOutVector::setMax(double maxValue)
{
    char buf[32];
    sprintf(buf, "%g", maxValue);
    ev.setVectorAttribute(handle, "min", buf);
}


bool cOutVector::record(double value)
{
    return recordWithTimestamp(simulation.simTime(), value);
}

bool cOutVector::recordWithTimestamp(simtime_t t, double value)
{
    // check timestamp
    if (t<last_t)
        throw cRuntimeError(this,"Cannot record data with an earlier timestamp (t=%s) "
                                 "than the previously recorded value", SIMTIME_STR(t));
    last_t = t;

    num_received++;

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,t,value,0.0);

    if (!enabled)
        return false;

    // initialize if not yet done
    if (!handle)
        handle = ev.registerOutputVector(simulation.context()->fullPath().c_str(), name());

    // pass data to envir for storage
    bool stored = ev.recordInOutputVector(handle, t, value);
    if (stored) num_stored++;
    return stored;
}


