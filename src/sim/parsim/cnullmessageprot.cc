//=========================================================================
//  CNULLMESSAGEPROT.CC - part of
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
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/csimplemodule.h" // SendOptions
#include "cnullmessageprot.h"
#include "clinkdelaylookahead.h"
#include "cparsimpartition.h"
#include "messagetags.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"

namespace omnetpp {

Register_Class(cNullMessageProtocol);

Register_GlobalConfigOption(CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS, "parsim-nullmessageprotocol-lookahead-class", CFG_STRING, "cLinkDelayLookahead", "When `cNullMessageProtocol` is selected as parsim synchronization class: specifies the C++ class that calculates lookahead. The class should subclass from `cNMPLookahead`.");
Register_GlobalConfigOption(CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS, "parsim-nullmessageprotocol-laziness", CFG_DOUBLE, "0.5", "When `cNullMessageProtocol` is selected as parsim synchronization class: specifies the laziness of sending null messages. Values in the range `[0,1)` are accepted. Laziness=0 causes null messages to be sent out immediately as a new EOT is learned, which may result in excessive null message traffic.");
extern cConfigOption *CFGID_PARSIM_DEBUG;  // registered in cparsimpartition.cc

cNullMessageProtocol::cNullMessageProtocol() : cParsimProtocolBase()
{
    debug = getEnvir()->getConfig()->getAsBool(CFGID_PARSIM_DEBUG);
    std::string lookhClass = getEnvir()->getConfig()->getAsString(CFGID_PARSIM_NULLMESSAGEPROTOCOL_LOOKAHEAD_CLASS);
    lookaheadcalc = dynamic_cast<cNMPLookahead *>(createOne(lookhClass.c_str()));
    if (!lookaheadcalc) \
        throw cRuntimeError("Class \"%s\" is not subclassed from cNMPLookahead", lookhClass.c_str());
    laziness = getEnvir()->getConfig()->getAsDouble(CFGID_PARSIM_NULLMESSAGEPROTOCOL_LAZINESS);
}

cNullMessageProtocol::~cNullMessageProtocol()
{
    delete lookaheadcalc;

    // segInfo[*].eitEvent/eotEvent messages already deleted with getFES()->clear()
    delete[] segInfo;
}

void cNullMessageProtocol::setContext(cSimulation *sim, cParsimPartition *seg, cParsimCommunications *co)
{
    cParsimProtocolBase::setContext(sim, seg, co);
    lookaheadcalc->setContext(sim, seg, co);
}

void cNullMessageProtocol::startRun()
{
    EV << "starting Null Message Protocol...\n";

    delete[] segInfo;

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];
    int myProcId = comm->getProcId();

    char buf[30];
    int i;

    // temporarily initialize everything to zero.
    for (i = 0; i < numSeg; i++) {
        segInfo[i].eotEvent = nullptr;
        segInfo[i].eitEvent = nullptr;
        segInfo[i].lastEotSent = 0.0;
    }

    // Note boot sequence: first we have to schedule all "resend-EOT" events,
    // so that the simulation will start by sending out null messages --
    // otherwise we'd end up sitting blocked on an EIT event forever!

    // create "resend-EOT" events and schedule them to zero (1st thing to do)
    EV << "  scheduling 'resend-EOT' events...\n";
    for (i = 0; i < numSeg; i++) {
        if (i != myProcId) {
            snprintf(buf, sizeof(buf), "resendEOT-%d", i);
            cMessage *eotMsg = new cMessage(buf, MK_PARSIM_RESENDEOT);
            eotMsg->setContextPointer((void *)(uintptr_t)i);  // khmm...
            segInfo[i].eotEvent = eotMsg;
            rescheduleEvent(eotMsg, 0.0);
        }
    }

    // create EIT events and schedule them to zero (null msgs will bump them)
    EV << "  scheduling 'EIT' events...\n";
    for (i = 0; i < numSeg; i++) {
        if (i != myProcId) {
            snprintf(buf, sizeof(buf), "EIT-%d", i);
            cMessage *eitMsg = new cMessage(buf, MK_PARSIM_EIT);
            segInfo[i].eitEvent = eitMsg;
            rescheduleEvent(eitMsg, 0.0);
        }
    }

    // start lookahead calculator too
    lookaheadcalc->startRun();

    EV << "  setup done.\n";
}

void cNullMessageProtocol::endRun()
{
    lookaheadcalc->endRun();
}

void cNullMessageProtocol::processOutgoingMessage(cMessage *msg, const SendOptions& options, int destProcId, int destModuleId, int destGateId, void *data)
{
    // calculate lookahead
    simtime_t lookahead = lookaheadcalc->getCurrentLookahead(msg, destProcId, data);
    simtime_t eot = sim->getSimTime() + lookahead;
    if (eot < segInfo[destProcId].lastEotSent)
        throw cRuntimeError("cNullMessageProtocol error: Attempt to decrease EOT");

    // send a null message only if EOT is better than last time
    bool sendNull = (eot > segInfo[destProcId].lastEotSent);

    // send message
    cCommBuffer *buffer = comm->createCommBuffer();
    if (sendNull) {
        // update "resend-EOT" timer
        segInfo[destProcId].lastEotSent = eot;
        simtime_t eotResendTime = sim->getSimTime() + lookahead*laziness;
        rescheduleEvent(segInfo[destProcId].eotEvent, eotResendTime);

        {if (debug) EV << "piggybacking null msg on '" << msg->getName() << "' to " << destProcId << ", lookahead=" << lookahead << ", EOT=" << eot << "; next resend at " << eotResendTime << "\n";}

        // send cMessage with piggybacked null message
        buffer->pack(eot);
        buffer->pack(destModuleId);
        buffer->pack(destGateId);
        packOptions(buffer, options);
        buffer->packObject(msg);
        comm->send(buffer, TAG_CMESSAGE_WITH_NULLMESSAGE, destProcId);
    }
    else
    {
        {if (debug) EV << "sending '" << msg->getName() << "' to " << destProcId << "\n";}

        // send cMessage
        buffer->pack(destModuleId);
        buffer->pack(destGateId);
        packOptions(buffer, options);
        buffer->packObject(msg);
        comm->send(buffer, TAG_CMESSAGE, destProcId);
    }
    comm->recycleCommBuffer(buffer);
}

void cNullMessageProtocol::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    int destModuleId;
    int destGateId;
    simtime_t eit;

    switch (tag) {
        case TAG_CMESSAGE_WITH_NULLMESSAGE: {
            buffer->unpack(eit);
            processReceivedEIT(sourceProcId, eit);
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            SendOptions options = unpackOptions(buffer);
            cMessage *msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, options, destModuleId, destGateId, sourceProcId);
            break;
        }

        case TAG_CMESSAGE: {
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            SendOptions options = unpackOptions(buffer);
            cMessage *msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, options, destModuleId, destGateId, sourceProcId);
            break;
        }

        case TAG_NULLMESSAGE: {
            buffer->unpack(eit);
            processReceivedEIT(sourceProcId, eit);
            break;
        }

        default: {
            partition->processReceivedBuffer(buffer, tag, sourceProcId);
            break;
        }
    }
    buffer->assertBufferEmpty();
}

void cNullMessageProtocol::processReceivedEIT(int sourceProcId, simtime_t eit)
{
    cMessage *eitMsg = segInfo[sourceProcId].eitEvent;

    {if (debug) EV << "null msg received from " << sourceProcId << ", EIT=" << eit << ", rescheduling EIT event\n";}

    // sanity check
    ASSERT(eit > eitMsg->getArrivalTime());

    // reschedule it to the EIT just received
    rescheduleEvent(eitMsg, eit);
}

cEvent *cNullMessageProtocol::takeNextEvent()
{
    // our EIT and resendEOT messages are always scheduled, so the FES can
    // only be empty if there are no other partitions at all -- "no events" then
    // means we're finished.
    if (sim->getFES()->isEmpty())
        return nullptr;

    // we could do a receiveNonblocking() call here to look at our mailbox,
    // but for performance reasons we don't -- it's enough to read it
    // (receiveBlocking()) when we're stuck on an EIT. Or should we do it
    // every 1000 events or so? If MPI receive buffer fills up it can cause
    // deadlock.
    // receiveNonblocking();

    cEvent *event;
    while (true) {
        event = sim->getFES()->peekFirst();
        cMessage *msg = event->isMessage() ? static_cast<cMessage *>(event) : nullptr;
        if (msg && msg->getKind() == MK_PARSIM_RESENDEOT) {
            // send null messages if window closed for a partition
            int procId = (uintptr_t)msg->getContextPointer();  // khmm...
            sendNullMessage(procId, event->getArrivalTime());
        }
        else if (msg && msg->getKind() == MK_PARSIM_EIT) {
            // wait until it gets out of the way (i.e. we get a higher EIT)
            {if (debug) EV << "blocking on EIT event '" << event->getName() << "'\n";}
            if (!receiveBlocking())
                return nullptr;
        }
        else {
            // just a normal event -- go ahead with it
            break;
        }
    }

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return event;
}

void cNullMessageProtocol::putBackEvent(cEvent *event)
{
    throw cRuntimeError("cNullMessageProtocol: \"Run Until Event/Module\" functionality "
                        "cannot be used with this scheduler (putBackEvent() not implemented)");
}

void cNullMessageProtocol::sendNullMessage(int procId, simtime_t now)
{
    // calculate EOT and sending of next null message
    simtime_t lookahead = lookaheadcalc->getCurrentLookahead(procId);
    simtime_t eot = now + lookahead;

    // ensure that even with eager resend, we only send out EOTs that
    // differ from previous one!
    if (eot == segInfo[procId].lastEotSent)
        return;
    if (eot < segInfo[procId].lastEotSent)
        throw cRuntimeError("cNullMessageProtocol error: Attempt to decrease EOT");
    segInfo[procId].lastEotSent = eot;

    // calculate time of next null message sending, and schedule "resend-EOT" event
    simtime_t eotResendTime = now + lookahead*laziness;
    rescheduleEvent(segInfo[procId].eotEvent, eotResendTime);

    {if (debug) EV << "sending null msg to " << procId << ", lookahead=" << lookahead << ", EOT=" << eot << "; next resend at " << eotResendTime << "\n";}

    // send out null message
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(eot);
    comm->send(buffer, TAG_NULLMESSAGE, procId);
    comm->recycleCommBuffer(buffer);
}

void cNullMessageProtocol::rescheduleEvent(cMessage *msg, simtime_t t)
{
    sim->getFES()->remove(msg);  // also works if the event is not currently scheduled
    msg->setArrivalTime(t);
    sim->getFES()->insert(msg);
}

}  // namespace omnetpp

