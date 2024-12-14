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
#include "omnetpp/checkandcast.h"
#include "common/enumstr.h"
#include "cplaceholdermod.h"
#include "cproxygate.h"
#include "cparsimpartition.h"
#include "cparsimsynchr.h"
#include "creceivedexception.h"
#include "messagetags.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_Class(cParsimPartition);

Register_GlobalConfigOption(CFGID_PARSIM_DEBUG, "parsim-debug", CFG_BOOL, "true", "With `parallel-simulation=true`: turns on printing of log messages from the parallel simulation code.");
Register_GlobalConfigOption(CFGID_PARSIM_NUM_PARTITIONS, "parsim-num-partitions", CFG_INT, nullptr, "If `parallel-simulation=true`, it specifies the number of parallel processes being used. This value must be in agreement with the number of simulator instances launched, e.g. with the `-n` or `-np` command-line option specified to the `mpirun` program when using MPI.");
Register_GlobalConfigOption(CFGID_PARSIM_PROCID, "parsim-procid", CFG_INT, nullptr, "Replaced by `parsim-process-partitionid`.");
Register_GlobalConfigOption(CFGID_PARSIM_PROCESS_PARTITIONID, "parsim-process-partitionid", CFG_INT, nullptr, "If `parallel-simulation=true` and `parsim-mode=distributed`, it specifies the ordinal of the current simulation process within the list parallel processes. The value must be in the range 0...n-1, where n is the number of partitions. This option is not required when using MPI communications, because MPI has its own way of conveying this information.");
Register_GlobalConfigOption(CFGID_PARSIM_COMMUNICATIONS_CLASS, "parsim-communications-class", CFG_STRING, "omnetpp::cFileCommunications", "If `parallel-simulation=true`, it selects the class that implements communication between partitions. The class must implement the `cParsimCommunications` interface.");
Register_GlobalConfigOption(CFGID_PARSIM_SYNCHRONIZATION_CLASS, "parsim-synchronization-class", CFG_STRING, "omnetpp::cNullMessageProtocol", "If `parallel-simulation=true`, it selects the parallel simulation algorithm. The class must implement the `cParsimSynchronizer` interface.");
Register_PerObjectConfigOption(CFGID_PARTITION_ID, "partition-id", KIND_MODULE, CFG_STRING, nullptr, "If `parallel-simulation=true`, it specifies in which partition the module should be instantiated. Specify numeric partition ID, or a comma-separated list of partition IDs for compound modules that span across multiple partitions. Ranges (`5..9`) and `*` (=all) are accepted too.");

cParsimPartition::cParsimPartition()
{
}

cParsimPartition::~cParsimPartition()
{
    shutdown(); //TODO this should probably be done earlier, when things are still up and running
}

void cParsimPartition::configure(cSimulation *simulation, cConfiguration *cfg, int partitionId)
{
    sim = simulation;

    debug = cfg->getAsBool(CFGID_PARSIM_DEBUG);

    std::string parsimcommClass = cfg->getAsString(CFGID_PARSIM_COMMUNICATIONS_CLASS);
    std::string parsimsynchClass = cfg->getAsString(CFGID_PARSIM_SYNCHRONIZATION_CLASS);

    comm = createByClassName<cParsimCommunications>(parsimcommClass.c_str(), "parallel simulation communications layer");
    synch = createByClassName<cParsimSynchronizer>(parsimsynchClass.c_str(), "parallel simulation synchronization layer");
    sim->addLifecycleListener(this);

    // wire them together (note: 'parsimSynchronizer' is also the scheduler for 'simulation')
    sim->setScheduler(synch);
    synch->configure(sim, cfg, this);

    // initialize them
    int parsimNumPartitions = cfg->getAsInt(CFGID_PARSIM_NUM_PARTITIONS, -1);
    if (cfg->getAsInt(CFGID_PARSIM_PROCID, -999) != -999)
        throw cRuntimeError("Configuration option `parsim-procid` has been renamed to `parsim-process-partitionid`, please use the latter name");
    int parsimPartitionId = partitionId != -1 ? partitionId : cfg->getAsInt(CFGID_PARSIM_PROCESS_PARTITIONID, -1);
    comm->configure(sim, cfg, parsimNumPartitions, parsimPartitionId);
}

int cParsimPartition::getNumPartitions() const
{
    return comm->getNumPartitions();
}

int cParsimPartition::getPartitionId() const
{
    return comm->getPartitionId();
}

void cParsimPartition::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        case LF_ON_SIMULATION_SUCCESS: {
            cTerminationException *e = check_and_cast<cTerminationException *>(details);
            bool isReceivedException = dynamic_cast<cReceivedTerminationException *>(e) != nullptr;
            if (!isReceivedException)
                broadcastException(*e);
        }
        case LF_ON_SIMULATION_ERROR: {
            cException *e = check_and_cast<cException *>(details);
            bool isReceivedException = dynamic_cast<cReceivedException *>(e) != nullptr;
            if (!isReceivedException)
                broadcastException(*e);
        }
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

    comm->shutdown();
}

void cParsimPartition::connectRemoteGates()
{
    //
    // Step 1: broadcast list of all "normal" input gates that may have corresponding
    // proxy gates in other partitions (as they need to redirect here)
    //
    EV << "connecting remote gates: step 1 - broadcasting input gates...\n";
    std::vector<RemoteGateInfo> inputGates;
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
                        RemoteGateInfo rgi;
                        rgi.moduleId = g->getOwnerModule()->getId();
                        rgi.gateId = g->getId();
                        rgi.moduleFullPath = g->getOwnerModule()->getFullPath();
                        rgi.gateName = g->getName();
                        rgi.gateIndex = g->isVector() ? g->getIndex() : -1;
                        inputGates.push_back(rgi);
                    }
                }
            }
        }
    }

    //
    // Step 2: collect info broadcast by others, and use it to fill in output cProxyGates
    //
    EV << "connecting remote gates: step 2 - collecting broadcasts sent by others...\n";
    std::vector<RemoteGateInfo> remoteGates = communicateRemoteGates(inputGates);

    // process what we got:
    for (const RemoteGateInfo& rgi : remoteGates) {
        // find corresponding output gate (if we have it) and set remote
        // gate address to the received one
        cModule *m = sim->findModuleByPath(rgi.moduleFullPath.c_str());
        cGate *g = !m ? nullptr : rgi.gateIndex == -1 ? m->gate(rgi.gateName.c_str()) : m->gate(rgi.gateName.c_str(), rgi.gateIndex);
        cProxyGate *pg = dynamic_cast<cProxyGate *>(g);

        EV << "    gate: " << rgi.moduleFullPath << "." << rgi.gateName;
        if (rgi.gateIndex >= 0)
            EV << "["  << rgi.gateIndex << "]";
        EV << " - ";
        if (!pg)
            EV << "not here\n";
        else
            EV << "points to (partitionId=" << rgi.remotePartitionId << " moduleId=" << rgi.moduleId << " gateId=" << rgi.gateId << ")\n";

        if (pg) {
            pg->setPartition(this);
            pg->setRemoteGate(rgi.remotePartitionId, rgi.moduleId, rgi.gateId);
        }
    }
    EV << "  done.\n";

    // verify that all gates have been connected
    for (int modId = 0; modId <= sim->getLastComponentId(); modId++) {
        cModule *mod = sim->getModule(modId);
        if (mod && mod->isPlaceholder()) {
            for (cModule::GateIterator it(mod); !it.end(); ++it) {
                cProxyGate *pg = dynamic_cast<cProxyGate *>(*it);
                if (pg && pg->getRemotePartitionId() == -1 && !pg->getPathStartGate()->getOwnerModule()->isPlaceholder())
                    throw cRuntimeError("Parallel simulation error: Dangling proxy gate '%s' "
                                        "(module '%s' or some of its submodules not instantiated in any partition?)",
                            pg->getFullPath().c_str(), mod->getFullPath().c_str());
            }
        }
    }
}

std::vector<cParsimPartition::RemoteGateInfo> cParsimPartition::communicateRemoteGates(const std::vector<RemoteGateInfo>& inputGates)
{
    std::vector<RemoteGateInfo> remoteGates;
    int numEnds = 0;
    int inputGateIndexToSend = 0;
    while (inputGateIndexToSend < inputGates.size() || numEnds < comm->getNumPartitions()-1) {

        if (inputGateIndexToSend < inputGates.size()) {
            // send one item
            cCommBuffer *buffer = comm->createCommBuffer();
            auto& rgi = inputGates[inputGateIndexToSend++];
            buffer->pack(rgi.moduleId);
            buffer->pack(rgi.gateId);
            buffer->pack(rgi.moduleFullPath.c_str());
            buffer->pack(rgi.gateName.c_str());
            buffer->pack(rgi.gateIndex);
            comm->broadcast(buffer, TAG_SETUP_LINKS);
            comm->recycleCommBuffer(buffer);

            if (inputGateIndexToSend == inputGates.size()) {
                cCommBuffer *buffer = comm->createCommBuffer();
                buffer->pack(-1);  // "the end"
                comm->broadcast(buffer, TAG_SETUP_LINKS);
                comm->recycleCommBuffer(buffer);
            }
        }

        if (numEnds < comm->getNumPartitions()-1) {
            // receive one item
            int tag, remotePartitionId;
            cCommBuffer *buffer = comm->createCommBuffer();
            while (comm->receiveNonblocking(TAG_SETUP_LINKS, buffer, tag, remotePartitionId)) {
                ASSERT(tag == TAG_SETUP_LINKS);
                RemoteGateInfo rgi;
                rgi.remotePartitionId = remotePartitionId;
                buffer->unpack(rgi.moduleId);
                if (rgi.moduleId == -1)
                    numEnds++;
                else {
                    buffer->unpack(rgi.gateId);
                    buffer->unpack(rgi.moduleFullPath);
                    buffer->unpack(rgi.gateName);
                    buffer->unpack(rgi.gateIndex);
                    remoteGates.push_back(rgi);
                }
            }
            comm->recycleCommBuffer(buffer);
        }
    }
    return remoteGates;
}

bool cParsimPartition::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
    // toplevel module is local everywhere
    if (!parentmod)
        return true;

    const int MAX_OBJECTFULLPATH = 1024;

    // find out if this module is (or has any submodules that are) on this partition
    char parname[MAX_OBJECTFULLPATH];
    if (index < 0)
        sprintf(parname, "%s.%s", parentmod->getFullPath().c_str(), modname);
    else
        sprintf(parname, "%s.%s[%d]", parentmod->getFullPath().c_str(), modname, index);  // FIXME this is incorrectly chosen for non-vector modules too!
    std::string partitionIds = getEnvir()->getConfig()->getAsString(parname, CFGID_PARTITION_ID, ""); //TODO eliminate getEnvir()
    if (partitionIds.empty()) {
        // modules inherit the setting from their parents, except when the parent is the system module (the network) itself
        if (!parentmod->getParentModule())
            throw cRuntimeError("Incomplete partitioning: Missing value for '%s'", parname);
        // "true" means "inherit", because an ancestor which answered "false" doesn't get recursed into
        return true;
    }
    else if (strcmp(partitionIds.c_str(), "*") == 0) {
        // present on all partitions (provided that ancestors have "*" set as well)
        return true;
    }
    else {
        // we expect a partition Id (or partition Ids, separated by commas) where this
        // module needs to be instantiated. So we return true if any of the numbers
        // is the Id of the local partition, otherwise false.
        EnumStringIterator partitionIdIter(partitionIds.c_str());
        if (partitionIdIter.hasError())
            throw cRuntimeError("Wrong partitioning: Syntax error in value '%s' for '%s' "
                                "(allowed syntax: '', '*', '1', '0,3,5-7')",
                    partitionIds.c_str(), parname);
        int numPartitions = comm->getNumPartitions();
        int myPartitionId = comm->getPartitionId();
        for ( ; partitionIdIter() != -1; partitionIdIter++) {
            if (partitionIdIter() >= numPartitions)
                throw cRuntimeError("Wrong partitioning: Value %d too large for '%s' (total partitions=%d)",
                        partitionIdIter(), parname, numPartitions);
            if (partitionIdIter() == myPartitionId)
                return true;
        }
        return false;
    }
}

bool cParsimPartition::processOutgoingMessage(cMessage *msg, const SendOptions& options, int partitionId, int moduleId, int gateId, void *data)
{
    if (debug)
        EV << "sending message '" << msg->getFullName() << "' (for T="
           << msg->getArrivalTime() << " to partitionId=" << partitionId << ")\n";

    return synch->processOutgoingMessage(msg, options, partitionId, moduleId, gateId, data);
}

void cParsimPartition::processReceivedBuffer(cCommBuffer *buffer, int tag, int sourcePartitionId)
{
    opp_string errmsg;
    switch (tag) {
        case TAG_TERMINATIONEXCEPTION:
            buffer->unpack(errmsg);
            throw cReceivedTerminationException(sourcePartitionId, errmsg.c_str());

        case TAG_EXCEPTION:
            buffer->unpack(errmsg);
            throw cReceivedException(sourcePartitionId, errmsg.c_str());

        default:
            throw cRuntimeError("cParsimPartition::processReceivedBuffer(): Unexpected tag %d "
                                "from partitionId %d", tag, sourcePartitionId);
    }
    buffer->assertBufferEmpty();
}

void cParsimPartition::processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourcePartitionId)
{
    msg->setSrcPartitionId(sourcePartitionId);
    cModule *mod = sim->getModule(destModuleId);
    if (!mod)
        throw cRuntimeError("Parallel simulation error: Destination module id=%d for message \"%s\""
                             "from partition %d does not exist (any longer)",
                             destModuleId, msg->getName(), sourcePartitionId);
    cGate *g = mod->gate(destGateId);
    if (!g)
        throw cRuntimeError("Parallel simulation error: Destination gate %d of module id=%d "
                             "for message \"%s\" from partition %d does not exist",
                             destGateId, destModuleId, msg->getName(), sourcePartitionId);

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

void cParsimPartition::broadcastException(std::exception& e)
{
    // send TAG_EXCEPTION to all partitions
    cCommBuffer *buffer = comm->createCommBuffer();
    buffer->pack(e.what());
    try {
        int tag = dynamic_cast<cTerminationException*>(&e) != nullptr ? TAG_TERMINATIONEXCEPTION : TAG_EXCEPTION;
        comm->broadcast(buffer, tag);
    }
    catch (std::exception&) {
        // swallow any exceptions -- here we're not interested in them
    }
    comm->recycleCommBuffer(buffer);
}

}  // namespace omnetpp

