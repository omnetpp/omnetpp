//=========================================================================
//  CIDEALSIMULATIONPROT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "cmessage.h"
#include "cmodule.h"
#include "cgate.h"
#include "cenvir.h"
#include "cconfig.h"
#include "cidealsimulationprot.h"
#include "cplaceholdermod.h"
#include "cparsimpartition.h"
#include "cparsimcomm.h"
#include "ccommbuffer.h"
#include "messagetags.h"
#include "macros.h"

// TBD needs to use cMessage::srcProcId()

Register_Class(cIdealSimulationProtocol);

// load 100,000 values from eventlog at once (~800K allocated memory)
#define TABLESIZE   100000

// helper function
inline bool cIdealSimulationProtocol::isExternalMessage(cMessage *msg)
{
    cModule *srcmod = sim->module(msg->senderModuleId());
    return dynamic_cast<cPlaceHolderModule *>(srcmod) != NULL;
}

cIdealSimulationProtocol::cIdealSimulationProtocol() : cParsimProtocolBase()
{
    fin = NULL;
    debug = ev.config()->getAsBool("General", "parsim-debug", true);

    tableSize = ev.config()->getAsInt("General", "parsim-idealsimulationprotocol-tablesize", TABLESIZE);
    table = new ExternalEvent[tableSize];
    numItems = 0;
    nextPos = 0;
}

cIdealSimulationProtocol::~cIdealSimulationProtocol()
{
    delete [] table;
}

void cIdealSimulationProtocol::startRun()
{
    char fname[200];
    sprintf(fname, "ispeventlog-%d.dat", comm->getProcId());
    fin = fopen(fname,"r");
    if (!fin)
        throw new cRuntimeError("cIdealSimulationProtocol error: cannot open file `%s' for read", fname);

    readNextRecordedEvent();
}

void cIdealSimulationProtocol::endRun()
{
    fclose(fin);
}

void cIdealSimulationProtocol::processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId)
{
    cParsimProtocolBase::processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
}

cMessage *cIdealSimulationProtocol::getNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    while (sim->msgQueue.empty())
        if (!receiveBlocking())
            return NULL;

    cMessage *msg = sim->msgQueue.peekFirst();
    simtime_t t = msg->arrivalTime();

    // if we aren't at the next external even yet --> nothing special to do
    if (t<nextExternalEvent.t)
    {
        // ASSERT(!isExternalMessage(msg)); -- TBD check: this might be the perf bottleneck?!
        return msg;
    }

    // if we got to the external event
    if (t==nextExternalEvent.t)
    {
        if (isExternalMessage(msg))
        {
            if (debug) ev << "expected external event for " << simtimeToStr(nextExternalEvent.t)
                          << " already here (arrived earlier), good!"<< endl;
            readNextRecordedEvent();
        }
        return msg;
    }

    // t>nextExternalEvent -- here we have to wait until external event arrives
    if (debug)
    {
        ev << "next local event at " << simtimeToStr(t);
        ev << " is PAST external event expected at " << simtimeToStr(nextExternalEvent.t);
        ev << " -- waiting..." << endl;
        ev.printf("DBG:next external event: %.30g, next local event: %.30g\n", nextExternalEvent.t, t);
    }

    do
    {
        if (!receiveBlocking())
            return NULL;
        msg = sim->msgQueue.peekFirst();
    }
    while (msg->arrivalTime()>nextExternalEvent.t || !isExternalMessage(msg));

    if (msg->arrivalTime()!=nextExternalEvent.t)
    {
        throw new cRuntimeError("cIdealSimulationProtocol: event trace does not match actual events: "
                                "expected event with timestamp %.18g, and got one with timestamp %.18g",
                                nextExternalEvent.t, msg->arrivalTime());
    }
    readNextRecordedEvent();
    return msg;
}

void cIdealSimulationProtocol::readNextRecordedEvent()
{
    // if table is empty, load new values into it
    if (nextPos==numItems)
    {
        nextPos = 0;
        numItems = fread(table, sizeof(table[0]), tableSize, fin);
        if (numItems==0)
            throw new cTerminationException("cIdealSimulationProtocol: end of event trace file");
    }

    // get next entry from table
    nextExternalEvent = table[nextPos++];

    if (debug) ev << "next external event: " << simtimeToStr(nextExternalEvent.t) << endl;
}

