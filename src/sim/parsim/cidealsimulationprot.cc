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


Register_Class(cIdealSimulationProtocol);

// load 100,000 values from eventlog at once (~800K allocated memory)
#define TABLESIZE   100000


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
    fin = fopen(fname,"rb");
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
    msg->setPriority(sourceProcId);
    cParsimProtocolBase::processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
}

cMessage *cIdealSimulationProtocol::getNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    while (sim->msgQueue.empty())
        if (!receiveBlocking())
            return NULL;

    cMessage *msg = sim->msgQueue.peekFirst();
    simtime_t msgTime = msg->arrivalTime();

    // if we aren't at the next external even yet --> nothing special to do
    if (msgTime < nextExternalEvent.t)
    {
        ASSERT(msg->srcProcId()==-1); // must be local message
        return msg;
    }

    // if we reached the next external event in the log file, do it
    if (msg->srcProcId()==nextExternalEvent.srcProcId && msgTime==nextExternalEvent.t)
    {
        if (debug) ev << "expected external event (srcProcId=" << msg->srcProcId()
                      << " t=" << simtimeToStr(nextExternalEvent.t)
                      << ") has already arrived, good!"<< endl;
        readNextRecordedEvent();
        msg->setPriority(0);
        return msg;
    }

    // next external event not yet here, we have to wait until we've received it
    ASSERT(msgTime > nextExternalEvent.t);
    if (debug)
    {
        ev << "next local event at " << simtimeToStr(msgTime) << " is PAST the "
              "next external event expected for t=" << simtimeToStr(nextExternalEvent.t) <<
              " -- waiting..." << endl;
        //printf("DBG:next external event: %.30g, next local event: %.30g\n", nextExternalEvent.t, msgTime);
    }

    do
    {
        if (!receiveBlocking())
            return NULL;
        msg = sim->msgQueue.peekFirst();
        msgTime = msg->arrivalTime();
    }
    while (msg->srcProcId()!=nextExternalEvent.srcProcId || msgTime > nextExternalEvent.t);

    if (msgTime < nextExternalEvent.t)
    {
        throw new cRuntimeError("cIdealSimulationProtocol: event trace does not match actual events: "
                                "expected event with timestamp %.18g from proc=%d, and got one with timestamp %.18g",
                                nextExternalEvent.t, nextExternalEvent.srcProcId, msgTime);
    }

    // we have the next external event we wanted, return it
    ASSERT(msgTime==nextExternalEvent.t);
    readNextRecordedEvent();
    msg->setPriority(0);
    return msg;
}

void cIdealSimulationProtocol::readNextRecordedEvent()
{
    // if table is empty, load new values into it
    if (nextPos==numItems)
    {
        nextPos = 0;
        numItems = fread(table, sizeof(ExternalEvent), tableSize, fin);
        if (numItems==0)
            throw new cTerminationException("cIdealSimulationProtocol: end of event trace file");
    }

    // get next entry from table
    nextExternalEvent = table[nextPos++];

    if (debug) ev << "next expected external event: srcProcId=" << nextExternalEvent.srcProcId
                  << " t=" << simtimeToStr(nextExternalEvent.t) << endl;
}

