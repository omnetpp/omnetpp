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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>  // strlen
#include "omnetpp/globals.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenum.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

Register_Class(cOutVector);

Register_Enum(cOutVector::Type, (cOutVector::TYPE_INT, cOutVector::TYPE_DOUBLE, cOutVector::TYPE_ENUM));
Register_Enum(cOutVector::InterpolationMode, (cOutVector::NONE, cOutVector::SAMPLE_HOLD, cOutVector::BACKWARD_SAMPLE_HOLD, cOutVector::LINEAR));

cOutVector::cOutVector(const char *name) : cNoncopyableOwnedObject(name)
{
    setFlag(FL_ENABLED, true);

    // register early if possible (only required by Akaroa)
    if (name) {
        handle = getEnvir()->registerOutputVector(getSimulation()->getContext()->getFullPath().c_str(), name);
        ASSERT(handle != nullptr);
    }
}

cOutVector::~cOutVector()
{
    if (handle)
        getEnvir()->deregisterOutputVector(handle);
}

void cOutVector::setName(const char *nam)
{
    if (handle)
        throw cRuntimeError(this, "setName(): Changing name of an output vector after record() calls is not allowed");

    cOwnedObject::setName(nam);

    // register early (only needed for Akaroa...)
    if (nam) {
        handle = getEnvir()->registerOutputVector(getSimulation()->getContext()->getFullPath().c_str(), getName());
        ASSERT(handle != nullptr);
    }
}

std::string cOutVector::str() const
{
    if (!handle)
        return std::string("(no values recorded yet)");
    std::stringstream out;
    out << "received " << numReceived << " values, stored " << numStored;
    return out.str();
}

void cOutVector::setUnit(const char *unit)
{
    if (!handle)
        throw cRuntimeError(this, "setUnit(): Set the object name first, using setName()");
    getEnvir()->setVectorAttribute(handle, "unit", unit);
}

void cOutVector::setEnum(const char *registeredEnumName)
{
    cEnum *enumDecl = cEnum::find(registeredEnumName);
    if (!enumDecl)
        throw cRuntimeError(this, "setEnum(): Enum '%s' not found -- is it declared with Register_Enum()?", registeredEnumName);
    setEnum(enumDecl);
}

void cOutVector::setEnum(cEnum *enumDecl)
{
    if (!handle)
        throw cRuntimeError(this, "setEnum(): Set the object name first, using setName()");
    getEnvir()->setVectorAttribute(handle, "enumname", enumDecl->getName());
    getEnvir()->setVectorAttribute(handle, "enum", enumDecl->str().c_str());
}

void cOutVector::setType(Type type)
{
    if (!handle)
        throw cRuntimeError(this,"setType(): Set the object name first, using setName()");

    const char *typeString = nullptr;
    switch (type) {
        case TYPE_INT:    typeString = "int"; break;
        case TYPE_DOUBLE: typeString = "double"; break;
        case TYPE_ENUM:   typeString = "enum"; break;
        //Note: no "default:" so that compiler can warn of incomplete coverage
    }
    if (!typeString)
        throw cRuntimeError(this, "setType(): Invalid type %d", type);
    getEnvir()->setVectorAttribute(handle, "type", typeString);
}

void cOutVector::setInterpolationMode(InterpolationMode mode)
{
    if (!handle)
        throw cRuntimeError(this,"setInterpolationMode(): Set the object name first, using setName()");

    const char *modeString = nullptr;
    switch (mode) {
        case NONE:                 modeString = "none"; break;
        case SAMPLE_HOLD:          modeString = "sample-hold"; break;
        case BACKWARD_SAMPLE_HOLD: modeString = "backward-sample-hold"; break;
        case LINEAR:               modeString = "linear"; break;
        //Note: no "default:" so that compiler can warn of incomplete coverage
    }
    if (!modeString)
        throw cRuntimeError(this, "setInterpolationMode(): Invalid interpolation mode %d", mode);
    getEnvir()->setVectorAttribute(handle, "interpolationmode", modeString);
}

void cOutVector::setMin(double minValue)
{
    if (!handle)
        throw cRuntimeError(this, "setMin(): Set the object name first, using setName()");

    char buf[32];
    sprintf(buf, "%g", minValue);
    getEnvir()->setVectorAttribute(handle, "min", buf);
}

void cOutVector::setMax(double maxValue)
{
    if (!handle)
        throw cRuntimeError(this, "setMax(): Set the object name first, using setName()");

    char buf[32];
    sprintf(buf, "%g", maxValue);
    getEnvir()->setVectorAttribute(handle, "max", buf);
}

bool cOutVector::record(double value)
{
    return recordWithTimestamp(getSimulation()->getSimTime(), value);
}

bool cOutVector::recordWithTimestamp(simtime_t t, double value)
{
    // check timestamp
    if (t < lastTimestamp)
        throw cRuntimeError(this, "Cannot record data with an earlier timestamp (t=%s) "
                                  "than the previously recorded value", SIMTIME_STR(t));
    lastTimestamp = t;

    numReceived++;

    // pass data to inspector
    if (recordInInspector)
        recordInInspector(dataForInspector, t, value);

    if (!isEnabled())
        return false;

    if (!getRecordDuringWarmupPeriod() && t < getSimulation()->getWarmupPeriod())
        return false;

    // initialize if not yet done
    if (!handle)
        handle = getEnvir()->registerOutputVector(getSimulation()->getContext()->getFullPath().c_str(), getName());

    // pass data to envir for storage
    bool stored = getEnvir()->recordInOutputVector(handle, t, value);
    if (stored)
        numStored++;
    return stored;
}

}  // namespace omnetpp

