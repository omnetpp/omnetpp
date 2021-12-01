//=========================================================================
//  CPARSIMPARTITION.CC - part of
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

#include <cstdlib>
#include <cstdio>
#include "omnetpp/cmessage.h"
#include "omnetpp/errmsg.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/csimplemodule.h" // SendOptions
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cparsimpartition.h"
#include "cparsimsynchr.h"
#include "creceivedexception.h"
#include "messagetags.h"

namespace omnetpp {

Register_Class(cParsimPartition);

Register_GlobalConfigOption(CFGID_PARSIM_DEBUG, "parsim-debug", CFG_BOOL, "true", "With `parallel-simulation=true`: turns on printing of log messages from the parallel simulation code.");

cParsimPartition::cParsimPartition()
{
    debug = getEnvir()->getConfig()->getAsBool(CFGID_PARSIM_DEBUG);
}

void cParsimPartition::setContext(cSimulation *simul, cParsimCommunications *commlayer, cParsimSynchronizer *sync)
{
    sim = simul;
    comm = commlayer;
    synch = sync;
}

void cParsimPartition::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        case LF_ON_SHUTDOWN: shutdown(); break;
        default: break;
    }
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
    if (!comm)
        return;  // if initialization failed

    cException e("Process has shut down");
    broadcastException(e);

    comm->shutdown();
}

void cParsimPartition::connectRemoteGates()
{
    cCommBuffer *buffer = comm->createCommBuffer();

    //
    // Step 1: broadcast list of all "normal" input gates that may have corresponding
    // proxy gates in other partitions (as they need to redirect here)
    //
    EV << "connecting remote gates: step 1 - broadcasting input gates...\n";
    for (int modId = 0; modId <= sim->getLastComponentId(); modId++) {
        cModule *mod = sim->getModule(modId);
        if (mod && !mod->isPlaceholder()) {
            for (cModule::GateIterator it(mod); !it.end(); ++it) {
                cGate *g = *it;
                if (g->getType() == cGate::INPUT) {
                    // if gate is connected to a placeholder module, in another partition that will
                    // be a local module and our module will be a placeholder with proxy gates
                    cGate *srcg = g->getPathStartGate();
                    if (srcg != g && srcg->getOwnerModule()->isPlaceholder()) {
                        // pack gate "address" here
                        buffer->pack(g->getOwnerModule()->getId());
                        buffer->pack(g->getId());
                        // pack gate name
                        buffer->pack(g->getOwnerModule()->getFullPath().c_str());
                        buffer->pack(g->getName());
                        buffer->pack(g->isVector() ? g->getIndex() : -1);
                    }
                }
            }
        }
    }
    buffer->pack(-1);  // "the end"
    comm->broadcast(buffer, TAG_SETUP_LINKS);

    //
    // Step 2: collect info broadcast by others, and use it to fill in output cProxyGates
    //
    EV << "connecting remote gates: step 2 - collecting broadcasts sent by others...\n";
    for (int i = 0; i < comm->getNumPartitions()-1; i++) {
        // receive:
        int tag, remoteProcId;
        // note: *must* filter for TAG_SETUP_LINKS here, to prevent race conditions
        if (!comm->receiveBlocking(TAG_SETUP_LINKS, buffer, tag, remoteProcId))
            throw cRuntimeError("connectRemoteGates() interrupted by user");
        ASSERT(tag == TAG_SETUP_LINKS);

        EV << "  processing msg from procId=" << remoteProcId << "...\n";

        // process what we got:
        while (true) {
            int remoteModId;
            int remoteGateId;
            char *moduleFullPath;
            char *gateName;
            int gateIndex;

            // unpack a gate -- modId==-1 indicates end of packet
            buffer->unpack(remoteModId);
            if (remoteModId == -1)
                break;
            buffer->unpack(remoteGateId);
            buffer->unpack(moduleFullPath);
            buffer->unpack(gateName);
            buffer->unpack(gateIndex);

            // find corresponding output gate (if we have it) and set remote
            // gate address to the received one
            cModule *m = sim->findModuleByPath(moduleFullPath);
            cGate *g = !m ? nullptr : gateIndex == -1 ? m->gate(gateName) : m->gate(gateName, gateIndex);
            cProxyGate *pg = dynamic_cast<cProxyGate *>(g);

            EV << "    gate: " << moduleFullPath << "." << gateName;
            if (gateIndex >= 0)
                EV << "["  << gateIndex << "]";
            EV << " - ";
            if (!pg)
                EV << "not here\n";
            else
                EV << "points to (procId=" << remoteProcId << " moduleId=" << remoteModId << " gateId=" << remoteGateId << ")\n";

            if (pg) {
                pg->setPartition(this);
                pg->setRemoteGate(remoteProcId, remoteModId, remoteGateId);
            }

            delete[] moduleFullPath;
            delete[] gateName;
        }
        buffer->assertBufferEmpty();
    }
    EV << "  done.\n";
    comm->recycleCommBuffer(buffer);

    // verify that all gates have been connected
    for (int modId = 0; modId <= sim->getLastComponentId(); modId++) {
        cModule *mod = sim->getModule(modId);
        if (mod && mod->isPlaceholder()) {
            for (cModule::GateIterator it(mod); !it.end(); ++it) {
                cProxyGate *pg = dynamic_cast<cProxyGate *>(*it);
                if (pg && pg->getRemoteProcId() == -1 && !pg->getPathStartGate()->getOwnerModule()->isPlaceholder())
                    throw cRuntimeError("Parallel simulation error: Dangling proxy gate '%s' "
                                        "(module '%s' or some of its submodules not instantiated in any partition?)",
                            pg->getFullPath().c_str(), mod->getFullPath().c_str());
            }
        }
    }
}

void cParsimPartition::processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data)
{
    if (debug)
        EV << "sending message '" << msg->getFullName() << "' (for T="
           << msg->getArrivalTime() << " to procId=" << procId << ")\n";

    synch->processOutgoingMessage(msg, options, procId, moduleId, gateId, data);
}

void cParsimPartition::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId)
{
    opp_string errmsg;
    switch (tag) {
        case TAG_TERMINATIONEXCEPTION:
            buffer->unpack(errmsg);
            throw cReceivedTerminationException(sourceProcId, errmsg.c_str());

        case TAG_EXCEPTION:
            buffer->unpack(errmsg);
            throw cReceivedException(sourceProcId, errmsg.c_str());

        default:
            throw cRuntimeError("cParsimPartition::processReceivedBuffer(): Unexpected tag %d "
                                "from procId %d", tag, sourceProcId);
    }
    buffer->assertBufferEmpty();
}

void cParsimPartition::processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId)
{
    msg->setSrcProcId(sourceProcId);
    cModule *mod = sim->getModule(destModuleId);
    if (!mod)
        throw cRuntimeError("Parallel simulation error: Destination module id=%d for message \"%s\""
                             "from partition %d does not exist (any longer)",
                             destModuleId, msg->getName(), sourceProcId);
    cGate *g = mod->gate(destGateId);
    if (!g)
        throw cRuntimeError("Parallel simulation error: Destination gate %d of module id=%d "
                             "for message \"%s\" from partition %d does not exist",
                             destGateId, destModuleId, msg->getName(), sourceProcId);

    // do our best to set the source gate (the gate of a cPlaceholderModule)
    cGate *srcg = g->getPathStartGate();
    msg->setSentFrom(srcg->getOwnerModule(), srcg->getId(), msg->getSendingTime());

    // deliver it to the "destination" gate of the connection -- the channel
    // has already been simulated in the originating partition. The following
    // portion of code is analogous to the code in cSimpleModule::sendDelayed().
    EVCB.beginSend(msg, options);
    EVCB.messageSendHop(msg, srcg);  // TODO store approx propagationDelay, transmissionDelay (they were already simulated remotely)
    bool keepit = g->deliver(msg, options, msg->getArrivalTime());
    if (!keepit)
        delete msg;
    else
        EVCB.endSend(msg);
}

void cParsimPartition::broadcastTerminationException(cTerminationException& e)
{
    // send TAG_TERMINATIONEXCEPTION to all partitions
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(e.what());
    try {
        comm->broadcast(buffer, TAG_TERMINATIONEXCEPTION);
    }
    catch (std::exception&) {
        // swallow exceptions -- here we're not interested in them
    }
    comm->recycleCommBuffer(buffer);
}

void cParsimPartition::broadcastException(std::exception& e)
{
    // send TAG_EXCEPTION to all partitions
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(e.what());
    try {
        comm->broadcast(buffer, TAG_EXCEPTION);
    }
    catch (std::exception&) {
        // swallow any exceptions -- here we're not interested in them
    }
    comm->recycleCommBuffer(buffer);
}

}  // namespace omnetpp

