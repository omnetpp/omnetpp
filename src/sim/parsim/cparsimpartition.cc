//=========================================================================
//  CPARSIMSEGMENT.CC - part of
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

#include <stdlib.h>
#include <stdio.h>
#include "cmessage.h"
#include "errmsg.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cparsimpartition.h"
#include "ccommbuffer.h"
#include "cparsimcomm.h"
#include "cparsimsynchr.h"
#include "creceivedexception.h"
#include "messagetags.h"
#include "cenvir.h"
#include "cconfig.h"
#include "macros.h"

Register_Class(cParsimPartition);


cParsimPartition::cParsimPartition()
{
    sim = NULL;
    comm = NULL;
    synch = NULL;
    debug = ev.config()->getAsBool("General", "parsim-debug", true);
}

cParsimPartition::~cParsimPartition()
{
}

void cParsimPartition::setContext(cSimulation *simul, cParsimCommunications *commlayer, cParsimSynchronizer *sync)
{
    sim = simul;
    comm = commlayer;
    synch = sync;
}

void cParsimPartition::startRun()
{
    connectRemoteGates();
}

void cParsimPartition::endRun()
{
}

void cParsimPartition::shutdown()
{
    if (!comm) return; // if initialization failed

    cException e("Process has shut down");
    broadcastException(&e);

    comm->shutdown();
}


void cParsimPartition::connectRemoteGates()
{
    cCommBuffer *buffer = comm->createCommBuffer();

    //
    // Step 1: broadcast what input gates we have that have to be connected
    //
    ev << "connecting remote gates: step 1 - broadcasting input gates...\n";
    for (int modId=0; modId<=sim->lastModuleId(); modId++)
    {
        cPlaceHolderModule *mod = dynamic_cast<cPlaceHolderModule *>(sim->module(modId));
        if (mod)
        {
            for (int gateId=0; gateId<mod->gates(); gateId++)
            {
                cGate *g = mod->gate(gateId);
                // if this is a normal output gate which leads to a simple module,
                // send the input gate where it is connected.
                if (g && g->type()=='O' && g->toGate() &&
                    g->destinationGate()->ownerModule()->isSimple())
                {
                    cGate *ing = g->toGate();
                    // pack gate "address" here
                    buffer->pack(ing->ownerModule()->id());
                    buffer->pack(ing->id());
                    // pack gate name
                    buffer->pack(ing->ownerModule()->fullPath().c_str());
                    buffer->pack(ing->name());
                    buffer->pack(ing->index());
                }
            }
        }
    }
    buffer->pack(-1); // "the end"
    comm->broadcast(buffer, TAG_SETUP_LINKS);

    //
    // Step 2: collect info broadcast by others, and use it to fill in output cProxyGates
    //
    ev << "connecting remote gates: step 2 - collecting broadcasts sent by others...\n";
    for (int i=0; i<comm->getNumPartitions()-1; i++)
    {
        // receive:
        int tag, remoteProcId;
        // note: *must* filter for TAG_SETUP_LINKS here, to prevent race conditions
        if (!comm->receiveBlocking(TAG_SETUP_LINKS, buffer, tag, remoteProcId))
            throw new cRuntimeError("connectRemoteGates() interrupted by user");
        ASSERT(tag==TAG_SETUP_LINKS);

        ev << "  processing msg from procId=" << remoteProcId << "...\n";

        // process what we got:
        while(true)
        {
            int remoteModId;
            int remoteGateId;
            char *moduleFullPath;
            char *gateName;
            int gateIndex;

            // unpack a gate -- modId==-1 signals end of packet
            buffer->unpack(remoteModId);
            if (remoteModId==-1)
                break;
            buffer->unpack(remoteGateId);
            buffer->unpack(moduleFullPath);
            buffer->unpack(gateName);
            buffer->unpack(gateIndex);

            // find corresponding output gate (if we have it) and set remote
            // gate address to the received one
            cModule *m = sim->moduleByPath(moduleFullPath);
            cGate *g = m ? m->gate(gateName,gateIndex) : NULL;
            cProxyGate *pg = dynamic_cast<cProxyGate *>(g);

            ev << "    gate: " << moduleFullPath << "." << gateName << "["  << gateIndex << "] - ";
            if (!pg)
                ev << "not here" << endl;
            else
                ev << "points to (procId=" << remoteProcId << " moduleId=" << remoteModId << " gateId=" << remoteGateId << ")" << endl;

            if (pg)
            {
                pg->setPartition(this);
                pg->setRemoteGate(remoteProcId,remoteModId,remoteGateId);
            }

            delete [] moduleFullPath;
            delete [] gateName;
        }
        buffer->assertBufferEmpty();
    }
    ev << "  done.\n";
    comm->recycleCommBuffer(buffer);
}

void cParsimPartition::processOutgoingMessage(cMessage *msg, int procId, int moduleId, int gateId, void *data)
{
    if (debug) ev << "sending message '" << msg->fullName() << "' (for T=" <<
                 simtimeToStr(msg->arrivalTime()) << " to procId=" << procId << endl;

    synch->processOutgoingMessage(msg, procId, moduleId, gateId, data);
}

void cParsimPartition::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    opp_string errmsg;
    switch (tag)
    {
        case TAG_TERMINATIONEXCEPTION:
            buffer->unpack(errmsg);
            throw new cReceivedTerminationException(sourceProcId, errmsg.c_str());
        case TAG_EXCEPTION:
            buffer->unpack(errmsg);
            throw new cReceivedException(sourceProcId, errmsg.c_str());
        default:
            throw new cRuntimeError("cParsimPartition::processReceivedBuffer(): unexpected tag %d "
                                 "from procId %d", tag, sourceProcId);
    }
    buffer->assertBufferEmpty();
}

void cParsimPartition::processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId)
{
    msg->setSrcProcId(sourceProcId);
    cModule *mod = sim->module(destModuleId);
    if (!mod)
        throw new cRuntimeError("parallel simulation error: destination module id=%d for message \"%s\""
                             "from partition %d does not exist (any longer)",
                             destModuleId, msg->name(), sourceProcId);
    cGate *g = mod->gate(destGateId);
    if (!g)
        throw new cRuntimeError("parallel simulation error: destination gate %d of module id=%d "
                             "for message \"%s\" from partition %d does not exist",
                             destGateId, destModuleId, msg->name(), sourceProcId);

    // do our best to set the source gate (the gate of a cPlaceHolderModule)
    cGate *srcg = g->sourceGate();
    msg->setSentFrom(srcg->ownerModule(), srcg->id(), msg->sendingTime());

    // deliver it to the "destination" gate of the connection -- the channel
    // has already been simulated in the originating partition.
    g->deliver(msg, msg->arrivalTime());
    ev.messageSent(msg);
}

void cParsimPartition::broadcastTerminationException(cTerminationException *e)
{
    // send TAG_TERMINATIONEXCEPTION to all partitions
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(e->message());
    try
    {
        comm->broadcast(buffer, TAG_TERMINATIONEXCEPTION);
    }
    catch (cException *e)
    {
        delete e; // eat any exceptions -- here we're not interested in them
    }
    comm->recycleCommBuffer(buffer);
}

void cParsimPartition::broadcastException(cException *e)
{
    // send TAG_EXCEPTION to all partitions
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(e->message());
    try
    {
        comm->broadcast(buffer, TAG_EXCEPTION);
    }
    catch (cException *e)
    {
        delete e; // eat any exceptions -- here we're not interested in them
    }
    comm->recycleCommBuffer(buffer);
}


