//=========================================================================
//  CIDEALSIMULATIONPROT.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "cidealsimulationprot.h"
#include "cplaceholdermod.h"
#include "cparsimpartition.h"
#include "messagetags.h"

namespace omnetpp {

Register_Class(cIdealSimulationProtocol);

// load 100,000 values from eventlog at once (~800K allocated memory)
#define TABLESIZE    "100000"

Register_GlobalConfigOption(CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE, "parsim-idealsimulationprotocol-tablesize", CFG_INT, TABLESIZE, "When `cIdealSimulationProtocol` is selected as parsim synchronization class: specifies the memory buffer size for reading the ISP event trace file.");
extern cConfigOption *CFGID_PARSIM_DEBUG;  // registered in cparsimpartition.cc

cIdealSimulationProtocol::cIdealSimulationProtocol() : cParsimProtocolBase()
{
    debug = getEnvir()->getConfig()->getAsBool(CFGID_PARSIM_DEBUG);
    tableSize = getEnvir()->getConfig()->getAsInt(CFGID_PARSIM_IDEALSIMULATIONPROTOCOL_TABLESIZE);
    table = new ExternalEvent[tableSize];
}

cIdealSimulationProtocol::~cIdealSimulationProtocol()
{
    delete[] table;
}

void cIdealSimulationProtocol::startRun()
{
    char fname[200];
    sprintf(fname, "ispeventlog-%d.dat", comm->getProcId());
    fin = fopen(fname, "rb");
    if (!fin)
        throw cRuntimeError("cIdealSimulationProtocol error: Cannot open file '%s' for read", fname);

    readNextRecordedEvent();
}

void cIdealSimulationProtocol::endRun()
{
    fclose(fin);
}

void cIdealSimulationProtocol::processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId)
{
    msg->setSchedulingPriority(sourceProcId);
    cParsimProtocolBase::processReceivedMessage(msg, options, destModuleId, destGateId, sourceProcId);
}

cEvent *cIdealSimulationProtocol::takeNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    while (sim->getFES()->isEmpty())
        if (!receiveBlocking())
            return nullptr;

    cEvent *event = sim->getFES()->peekFirst();
    simtime_t eventTime = event->getArrivalTime();

    // if we aren't at the next external even yet --> nothing special to do
    if (eventTime < nextExternalEvent.t) {
        ASSERT(event->getSrcProcId() == -1);  // must be local message
        return event;
    }

    // if we reached the next external event in the log file, do it
    if (event->getSrcProcId() == nextExternalEvent.srcProcId && eventTime == nextExternalEvent.t) {
        if (debug)
            EV << "expected external event (srcProcId=" << event->getSrcProcId()
               << " t=" << nextExternalEvent.t << ") has already arrived, good!\n";
        readNextRecordedEvent();
        event->setSchedulingPriority(0);

        // remove event from FES and return it
        cEvent *tmp = sim->getFES()->removeFirst();
        ASSERT(tmp == event);
        return event;
    }

    // next external event not yet here, we have to wait until we've received it
    ASSERT(eventTime > nextExternalEvent.t);
    if (debug) {
        EV << "next local event at " << eventTime << " is PAST the next external event "
              "expected for t=" << nextExternalEvent.t << " -- waiting...\n";
    }

    do {
        if (!receiveBlocking())
            return nullptr;
        event = sim->getFES()->peekFirst();
        eventTime = event->getArrivalTime();
    } while (event->getSrcProcId() != nextExternalEvent.srcProcId || eventTime > nextExternalEvent.t);

    if (eventTime < nextExternalEvent.t) {
        throw cRuntimeError("cIdealSimulationProtocol: Event trace does not match actual events: "
                            "expected event with timestamp %s from proc=%d, and got one with timestamp %s",
                            SIMTIME_STR(nextExternalEvent.t), nextExternalEvent.srcProcId, SIMTIME_STR(eventTime));
    }

    // we have the next external event we wanted, return it
    ASSERT(eventTime == nextExternalEvent.t);
    readNextRecordedEvent();
    event->setSchedulingPriority(0);

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return event;
}

void cIdealSimulationProtocol::readNextRecordedEvent()
{
    // if table is empty, load new values into it
    if (nextPos == numItems) {
        nextPos = 0;
        numItems = fread(table, sizeof(ExternalEvent), tableSize, fin);
        if (numItems == 0)
            throw cTerminationException("cIdealSimulationProtocol: End of event trace file");
    }

    // get next entry from table
    nextExternalEvent = table[nextPos++];

    if (debug)
        EV << "next expected external event: srcProcId=" << nextExternalEvent.srcProcId
           << " t=" << nextExternalEvent.t << "\n";
}

void cIdealSimulationProtocol::putBackEvent(cEvent *event)
{
    throw cRuntimeError("cIdealSimulationProtocol: \"Run Until Event/Module\" functionality "
                        "cannot be used with this scheduler (putBackEvent() not implemented)");
}

}  // namespace omnetpp

