//========================================================================
//
//  COUTVECT.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cOutVector         : represents a vector in the output file
//
//   Original version:  Gabor Lencse
//   Rewrite, bugfixes: Andras Varga
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>   // strlen
#include "coutvect.h"
#include "cmodule.h"
#include "csimul.h"
#include "cenvir.h"


cOutVector::cOutVector(const char *name, int tupl) : cObject(name)
{
    enabled = true;
    handle = NULL;
    num_received = 0;
    num_stored = 0;
    record_in_inspector = NULL;
    tuple = tupl;

    if (tuple!=1 && tuple!=2)
    {
        tuple = 0;
        opp_error("(%s)%s: constructor: invalid value (%d) for tuple; supported values are 1 and 2", className(), fullPath(), tupl);
        return;
    }
}

cOutVector::~cOutVector()
{
    if (handle)
        ev.deregisterOutputVector(handle);
}

void cOutVector::setName(const char *name)
{
    if (handle)
        {opp_error("(%s)%s: setName(): changing name of an output vector after record() calls is not allowed", className(), fullPath());return;}
    cObject::setName(name);
    return;
}

void cOutVector::info(char *buf)
{
    cObject::info(buf);
    if (handle)
        sprintf( buf+strlen(buf), " (received %ld values, stored %ld)", num_received, num_stored);
    else
        sprintf( buf+strlen(buf), " (no values recorded yet)");
}

bool cOutVector::record(double value)
{
    // check tuple
    if (tuple!=1)
        {opp_error(eNUMARGS,className(),fullPath(),1);return false;}

    num_received++;

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,value,0.0);

    if (!enabled)
        return false;

    // initialize if not yet done
    if (!handle)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath(), name(), tuple);

    // pass data to envir for storage
    bool stored = ev.recordInOutputVector(handle, simulation.simTime(), value);
    if (stored) num_stored++;
    return stored;
}

bool cOutVector::record(double value1, double value2)
{
    // check tuple
    if (tuple!=2)
        {opp_error(eNUMARGS,className(),fullPath(),2);return false;}

    num_received++;

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,value1,value2);

    if (!enabled)
        return false;

    // initialize if not yet done
    if (!handle)
        handle = ev.registerOutputVector(simulation.contextModule()->fullPath(), name(), tuple);

    // pass data to envir for storage
    bool stored = ev.recordInOutputVector(handle, simulation.simTime(), value1, value2);
    if (stored) num_stored++;
    return stored;
}

