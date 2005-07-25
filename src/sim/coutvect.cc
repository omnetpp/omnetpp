//========================================================================
//  COUTVECT.CC - part of
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

#include <assert.h>
#include <string.h>   // strlen
#include "macros.h"
#include "coutvect.h"
#include "cmodule.h"
#include "csimul.h"
#include "cenvir.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

Register_Class(cOutVector);

cOutVector::cOutVector(const char *nam, int tuple) : cObject(nam)
{
    enabled = true;
    handle = NULL;
    num_received = 0;
    num_stored = 0;
    record_in_inspector = NULL;
    tupl = tuple;
    last_t = 0;

    if (tupl!=1 && tupl!=2)
    {
        tupl = 0;
        throw new cRuntimeError(this,"constructor: invalid value (%d) for tuple; supported values are 1 and 2", tupl);
    }

    // register early if possible (only needed for Akaroa...)
    if (nam)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath().c_str(), name(), tupl);
}

cOutVector::~cOutVector()
{
    if (handle)
        ev.deregisterOutputVector(handle);
}

void cOutVector::setName(const char *nam)
{
    if (handle)
        throw new cRuntimeError(this,"setName(): changing name of an output vector after record() calls is not allowed");
    cObject::setName(nam);

    // register early (only needed for Akaroa...)
    if (nam)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath().c_str(), name(), tupl);
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
    throw new cRuntimeError(this, "netPack() not supported");
}

void cOutVector::netUnpack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this, "netUnpack(): not supported");
}


bool cOutVector::record(double value)
{
    return recordWithTimestamp(simulation.simTime(), value);
}

bool cOutVector::record(double value1, double value2)
{
    return recordWithTimestamp(simulation.simTime(), value1, value2);
}

bool cOutVector::recordWithTimestamp(simtime_t t, double value)
{
    // check tuple
    if (tupl!=1)
        throw new cRuntimeError(this,eNUMARGS,1);

    // check timestamp
    if (t<last_t)
        throw new cRuntimeError(this,"Cannot record data with an earlier timestamp (t=%s) "
                                     "than the previously recorded value", simtimeToStr(t));
    last_t = t;

    num_received++;

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,t,value,0.0);

    if (!enabled)
        return false;

    // initialize if not yet done
    if (!handle)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath().c_str(), name(), tupl);

    // pass data to envir for storage
    bool stored = ev.recordInOutputVector(handle, t, value);
    if (stored) num_stored++;
    return stored;
}

bool cOutVector::recordWithTimestamp(simtime_t t, double value1, double value2)
{
    // check tuple
    if (tupl!=2)
        throw new cRuntimeError(this,eNUMARGS,2);

    // check timestamp
    if (t<last_t)
        throw new cRuntimeError(this,"Cannot record data with an earlier timestamp (t=%s) "
                                     "than the previously recorded value", simtimeToStr(t));
    last_t = t;

    num_received++;

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,t,value1,value2);

    if (!enabled)
        return false;

    // initialize if not yet done
    if (!handle)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath().c_str(), name(), tupl);

    // pass data to envir for storage
    bool stored = ev.recordInOutputVector(handle, t, value1, value2);
    if (stored) num_stored++;
    return stored;
}


