//========================================================================
//
//  COUTVECT.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cOutFileMgr        : manages output files
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
    num_recorded = 0;
    num_stored = 0;
    record_in_inspector = NULL;
    tuple = tupl;

    if (tuple!=1 && tuple!=2)
    {
        tuple = 0;
        opp_error("(%s)%s: constructor: invalid value (%d) for tuple; supported values are 1 and 2", className(), fullPath(), tupl);
        return;
    }

    handle = ev.registerOutputVector(simulation.contextModule()->fullPath(), name, tuple);
}

cOutVector::~cOutVector()
{
    ev.deregisterOutputVector(handle);
}

void cOutVector::setName(const char *name)
{
    opp_error("(%s)%s: setName(): changing name of an output vector after creation is not supported", className(), fullPath());
    return;
}

void cOutVector::info(char *buf)
{
    cObject::info(buf);
    sprintf( buf+strlen(buf), " (received %ld values, stored %ld)", num_recorded, num_stored);
}

bool cOutVector::record(double value)
{
    // check tuple
    if (tuple!=1)
        {opp_error(eNUMARGS,className(),fullPath(),1);return false;}

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,value,0.0);

    if (!enabled)
        return false;

    // pass data to envir for storage
    num_recorded++;
    bool stored = ev.recordInOutputVector(handle, simulation.simTime(), value);
    if (stored) num_stored++;
    return stored;
}

bool cOutVector::record(double value1, double value2)
{
    // check tuple
    if (tuple!=2)
        {opp_error(eNUMARGS,className(),fullPath(),2);return false;}

    // pass data to inspector
    if (record_in_inspector)
        record_in_inspector(data_for_inspector,value1,value2);

    if (!enabled)
        return false;

    // pass data to envir for storage
    num_recorded++;
    bool stored = ev.recordInOutputVector(handle, simulation.simTime(), value1, value2);
    if (stored) num_stored++;
    return stored;
}

