//=========================================================================
//  CPARSIMPARTITION.H - part of
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

#ifndef __OMNETPP_CPARSIMPARTITION_H
#define __OMNETPP_CPARSIMPARTITION_H

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cobject.h"
#include "omnetpp/simtime_t.h"
#include "omnetpp/clifecyclelistener.h"

namespace omnetpp {

class cSimulation;
class cParsimSynchronizer;
class cParsimCommunications;
class cCommBuffer;
class cException;
class cTerminationException;
struct SendOptions;

/**
 * @brief Represents one partition in a parallel simulation. Knows about
 * partitions and the links between this partition and its neighbors.
 *
 * Interconnections are stored not directly inside this object,
 * but in cProxyGate's of cPlaceholderModule's. The remote address fields
 * of cProxyGate's are filled out here in the connectRemoteGates() method.
 *
 * This layer also handles generic (synchronization-independent part of)
 * communication with other partitions:
 *
 * - cMessages outgoing from this partition flow through here
 *   (processOutgoingMessage()), and
 *
 * - incoming messages are processed at here by processReceivedBuffer()
 *   (invoked from the synchronization layer, cParsimSynchronizer)
 *
 * This layer is communication library independent (e.g. it contains
 * no MPI calls) -- it builds on the abstraction layer provided
 * by cParsimCommunications.
 *
 * @ingroup Parsim
 */
class SIM_API cParsimPartition : public cObject, public cISimulationLifecycleListener
{
  protected:
    cSimulation *sim = nullptr;
    cParsimCommunications *comm = nullptr;
    cParsimSynchronizer *synch = nullptr;
    bool debug;

  protected:
    // internal: fills in remote gate addresses of all cProxyGate's in the current partition
    void connectRemoteGates();

    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and
     * shutdown(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /**
     * Constructor.
     */
    cParsimPartition();

    /**
     * Virtual destructor.
     */
    virtual ~cParsimPartition() {}

    /**
     * Pass cParsimPartition the objects it has to cooperate with.
     */
    void setContext(cSimulation *sim, cParsimCommunications *comm, cParsimSynchronizer *synch);

    /**
     * Called at the beginning of a simulation run. Fills in remote gate addresses
     * of all cProxyGate's in the current partition.
     */
    void startRun();

    /**
     * Called at the end of a simulation run.
     */
    void endRun();

    /**
     * Shut down the parallel simulation system.
     */
    void shutdown();

    /**
     * A hook called from cProxyGate::deliver() when an outgoing cMessage
     * arrives at partition boundary. We just pass it up to the synchronization
     * layer (see similar method in cParsimSynchronizer).
     */
    virtual void processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data);

    /**
     * Process messages coming from other partitions. This method is called from
     * the synchronization layer (see cParsimSynchronizer), after it has
     * processed all tags that it understands (namely, cMessages
     * (tag=TAG_CMESSAGE) and all tags used by the synchronization protocol).
     */
    virtual void processReceivedBuffer(cCommBuffer *buffer, int tag, int sourceProcId);

    /**
     * Process cMessages received from other partitions. This method is called from
     * the synchronization layer (see cParsimSynchronizer) when it received
     * a message from other partitions. This method checks that the destination
     * module/gate still exists, sets the source module/gate to the appropriate
     * placeholder module, and inserts the message into the FES.
     */
    virtual void processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId);

    /**
     * Called when a cTerminationException occurs (i.e. the simulation is
     * about to be finished normally), this methods notifies other partitions
     * about the exception.
     *
     * This methods "eats" exceptions that occur during broadcast. (We're
     * not interested in cascading exceptions.)
     */
    virtual void broadcastTerminationException(cTerminationException& e);

    /**
     * Called when an exception occurs (i.e. the simulation is about to be
     * stopped with an error), this methods notifies other partitions
     * about the exception.
     *
     * This methods "eats" exceptions that occur during broadcast. (We're
     * not interested in cascading exceptions.)
     */
    virtual void broadcastException(std::exception& e);
};

}  // namespace omnetpp


#endif

