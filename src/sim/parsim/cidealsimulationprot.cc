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
#include "cconfiguration.h"
#include "cidealsimulationprot.h"
#include "cplaceholdermod.h"
#include "cparsimpartition.h"
#include "cparsimcomm.h"
#include "ccommbuffer.h"
#include "messagetags.h"
#include "globals.h"
#include "cconfigkey.h"
#include "regmacros.h"

USING_NAMESPACE


Register_Class(cIdealSimulationProtocol);

// load 100,000 values from eventlog at once (~800K allocated memory)
#define TABLESIZE   "100000"

Register_GlobalConfigEntry(CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "parsim-idealsimulationprotocol-tablesize", CFG_INT, TABLESIZE, "When cIdealSimulationProtocol is selected as parsim synchronization class: specifies the memory buffer size for reading the ISP event trace file.");
extern cConfigKey *CFGID_PARSIM_DEBUG; // registered in cparsimpartition.cc


cIdealSimulationProtocol::cIdealSimulationProtocol() : cParsimProtocolBase()
{
    fin = NULL;
    debug = ev.config()->getAsBool(CFGID_PARSIM_DEBUG);

    tableSize = ev.config()->getAsInt(CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE);
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
        throw cRuntimeError("cIdealSimulationProtocol error: cannot open file `%s' for read", fname);

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
                      << " t=" << nextExternalEvent.t << ") has already arrived, good!\n";
        readNextRecordedEvent();
        msg->setPriority(0);
        return msg;
    }

    // next external event not yet here, we have to wait until we've received it
    ASSERT(msgTime > nextExternalEvent.t);
    if (debug)
    {
        ev << "next local event at " << msgTime << " is PAST the next external event "
              "expected for t=" << nextExternalEvent.t << " -- waiting...\n";
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
        throw cRuntimeError("cIdealSimulationProtocol: event trace does not match actual events: "
                            "expected event with timestamp %s from proc=%d, and got one with timestamp %s",
                            SIMTIME_STR(nextExternalEvent.t), nextExternalEvent.srcProcId, SIMTIME_STR(msgTime));
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
            throw cTerminationException("cIdealSimulationProtocol: end of event trace file");
    }

    // get next entry from table
    nextExternalEvent = table[nextPos++];

    if (debug) ev << "next expected external event: srcProcId=" << nextExternalEvent.srcProcId
                  << " t=" << nextExternalEvent.t << "\n";
}

