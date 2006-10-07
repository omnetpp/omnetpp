//=========================================================================
//  CNULLMESSAGEPROT.CC - part of
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
#include "cnullmessageprot.h"
#include "clinkdelaylookahead.h"
#include "cparsimpartition.h"
#include "cparsimcomm.h"
#include "ccommbuffer.h"
#include "messagetags.h"
#include "macros.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cchannel.h"

Register_Class(cNullMessageProtocol);


cNullMessageProtocol::cNullMessageProtocol() : cParsimProtocolBase()
{
    numSeg = 0;
    segInfo = NULL;

    const char *lookhClass = ev.config()->getAsString("General", "parsim-nullmessageprotocol-lookahead-class", "cLinkDelayLookahead");
    lookaheadcalc = dynamic_cast<cNMPLookahead *>(createOne(lookhClass));
    if (!lookaheadcalc) \
         throw new cRuntimeError("Class \"%s\" is not subclassed from cNMPLookahead", lookhClass);
    debug = ev.config()->getAsBool("General", "parsim-debug", true);
    laziness = ev.config()->getAsDouble("General", "parsim-nullmessageprotocol-laziness", 0.5);
}

cNullMessageProtocol::~cNullMessageProtocol()
{
    delete lookaheadcalc;

    // segInfo[*].eitEvent/eotEvent messages already deleted with msgQueue.clear()
    delete [] segInfo;
}

void cNullMessageProtocol::setContext(cSimulation *sim, cParsimPartition *seg, cParsimCommunications *co)
{
    cParsimProtocolBase::setContext(sim, seg, co);
    lookaheadcalc->setContext(sim, seg, co);
}

void cNullMessageProtocol::startRun()
{
    ev << "starting Null Message Protocol...\n";

    delete [] segInfo;

    numSeg = comm->getNumPartitions();
    segInfo = new PartitionInfo[numSeg];
    int myProcId = comm->getProcId();

    char buf[30];
    int i;

    // temporarily initialize everything to zero.
    for (i=0; i<numSeg; i++)
    {
        segInfo[i].eotEvent = NULL;
        segInfo[i].eitEvent = NULL;
        segInfo[i].lastEotSent = 0.0;
    }

    // Note boot sequence: first we have to schedule all "resend-EOT" events,
    // so that the simulation will start by sending out null messages --
    // otherwise we'd end up sitting blocked on an EIT event forever!

    // create "resend-EOT" events and schedule them to zero (1st thing to do)
    ev << "  scheduling 'resend-EOT' events...\n";
    for (i=0; i<numSeg; i++)
    {
        if (i!=myProcId)
        {
            sprintf(buf,"resendEOT-%d", i);
            cMessage *eotMsg =  new cMessage(buf,MK_PARSIM_RESENDEOT);
            eotMsg->setContextPointer((void *)(long)i);  // khmm...
            segInfo[i].eotEvent = eotMsg;
            rescheduleEvent(eotMsg, 0.0);
        }
    }

    // create EIT events and schedule them to zero (null msgs will bump them)
    ev << "  scheduling 'EIT' events...\n";
    for (i=0; i<numSeg; i++)
    {
        if (i!=myProcId)
        {
            sprintf(buf,"EIT-%d", i);
            cMessage *eitMsg =  new cMessage(buf,MK_PARSIM_EIT);
            segInfo[i].eitEvent = eitMsg;
            rescheduleEvent(eitMsg, 0.0);
        }
    }

    // start lookahead calculator too
    lookaheadcalc->startRun();

    ev << "  setup done.\n";

}

void cNullMessageProtocol::endRun()
{
    lookaheadcalc->endRun();
}

void cNullMessageProtocol::processOutgoingMessage(cMessage *msg, int destProcId, int destModuleId, int destGateId, void *data)
{
    // calculate lookahead
    simtime_t lookahead = lookaheadcalc->getCurrentLookahead(msg, destProcId, data);
    simtime_t eot = sim->simTime() + lookahead;
    if (eot < segInfo[destProcId].lastEotSent)
        throw new cRuntimeError("cNullMessageProtocol error: attempt to decrease EOT");

    // send a null message only if EOT is better than last time
    bool sendNull = (eot > segInfo[destProcId].lastEotSent);

    // send message
    cCommBuffer *buffer = comm->createCommBuffer();
    if (sendNull)
    {
        // update "resend-EOT" timer
        segInfo[destProcId].lastEotSent = eot;
        simtime_t eotResendTime = sim->simTime() + lookahead*laziness;
        rescheduleEvent(segInfo[destProcId].eotEvent, eotResendTime);

        {if (debug) ev.printf("piggybacking null msg on `%s' to %d, lookahead=%lg, EOT=%lg; next resend at %lg\n",
                              msg->name(),destProcId,lookahead,eot,eotResendTime);}

        // send cMessage with piggybacked null message
        buffer->pack(eot);
        buffer->pack(destModuleId);
        buffer->pack(destGateId);
        buffer->packObject(msg);
        comm->send(buffer, TAG_CMESSAGE_WITH_NULLMESSAGE, destProcId);
    }
    else
    {
        {if (debug) ev.printf("sending `%s' to %d\n",msg->name(),destProcId);}

        // send cMessage
        buffer->pack(destModuleId);
        buffer->pack(destGateId);
        buffer->packObject(msg);
        comm->send(buffer, TAG_CMESSAGE, destProcId);
    }
    comm->recycleCommBuffer(buffer);
}

void cNullMessageProtocol::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    int destModuleId;
    int destGateId;
    cMessage *msg;
    simtime_t eit;

    switch (tag)
    {
        case TAG_CMESSAGE_WITH_NULLMESSAGE:
            buffer->unpack(eit);
            processReceivedEIT(sourceProcId, eit);
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
            break;

        case TAG_CMESSAGE:
            buffer->unpack(destModuleId);
            buffer->unpack(destGateId);
            msg = (cMessage *)buffer->unpackObject();
            processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
            break;

        case TAG_NULLMESSAGE:
            buffer->unpack(eit);
            processReceivedEIT(sourceProcId, eit);
            break;

        default:
            partition->processReceivedBuffer(buffer, tag, sourceProcId);
            break;
    }
    buffer->assertBufferEmpty();
}

void cNullMessageProtocol::processReceivedEIT(int sourceProcId, simtime_t eit)
{
    cMessage *eitMsg = segInfo[sourceProcId].eitEvent;

    {if (debug) ev.printf("null msg received from %d, EIT=%lg, rescheduling EIT event\n", sourceProcId, eit);}

    // sanity check
    ASSERT(eit > eitMsg->arrivalTime());

    // reschedule it to the EIT just received
    rescheduleEvent(eitMsg, eit);
}

cMessage *cNullMessageProtocol::getNextEvent()
{
    // our EIT and resendEOT messages are always scheduled, so the FES can
    // only be empty if there are no other partitions at all -- "no events" then
    // means we're finished.
    if (sim->msgQueue.empty())
        return NULL;

    // we could do a receiveNonblocking() call here to look at our mailbox,
    // but for performance reasons we don't -- it's enough to read it
    // (receiveBlocking()) when we're stuck on an EIT. Or should we do it
    // every 1000 events or so? If MPI receive buffer fills up it can cause
    // deadlock.
    //receiveNonblocking();

    cMessage *msg;
    while (true)
    {
        msg = sim->msgQueue.peekFirst();
        if (msg->kind() == MK_PARSIM_RESENDEOT)
        {
            // send null messages if window closed for a partition
            int procId = (long) msg->contextPointer();  // khmm...
            sendNullMessage(procId, msg->arrivalTime());
        }
        else if (msg->kind() == MK_PARSIM_EIT)
        {
            // wait until it gets out of the way (i.e. we get a higher EIT)
            {if (debug) ev.printf("blocking on EIT event `%s'\n", msg->name());}
            if (!receiveBlocking())
                return NULL;
        }
        else
        {
            // just a normal event -- go ahead with it
            break;
        }
    }
    return msg;
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
        throw new cRuntimeError("cNullMessageProtocol error: attempt to decrease EOT");
    segInfo[procId].lastEotSent = eot;

    // calculate time of next null message sending, and schedule "resend-EOT" event
    simtime_t eotResendTime = now + lookahead*laziness;
    rescheduleEvent(segInfo[procId].eotEvent, eotResendTime);

    {if (debug) ev.printf("sending null msg to %d, lookahead=%lg, EOT=%lg; next resend at %lg\n",procId,lookahead,eot,eotResendTime);}

    // send out null message
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(eot);
    comm->send(buffer, TAG_NULLMESSAGE, procId);
    comm->recycleCommBuffer(buffer);
}

void cNullMessageProtocol::rescheduleEvent(cMessage *msg, simtime_t t)
{
    sim->msgQueue.get(msg);  // also works if the event is not currently scheduled
    msg->setArrivalTime(t);
    sim->msgQueue.insert(msg);
}


