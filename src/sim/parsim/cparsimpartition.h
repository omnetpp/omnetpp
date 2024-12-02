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
    bool debug = false;

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
    virtual ~cParsimPartition();

    /**
     * Pass cParsimPartition the objects it has to cooperate with.
     */
    void configure(cSimulation *simulation, cConfiguration *cfg);

    /**
     * Returns the simulation instance associated with the object.
     * Returns the simulation instance this object is assocated with.
     */
    cSimulation *getSimulation() const override {return sim;}

    /**
     * Returns the parallel simulation communications layer.
     */
    cParsimCommunications *getCommunications() const {return comm;}

    /**
     * Returns the parallel simulation synchronizer.
     */
    cParsimSynchronizer *getSynchronizer() const {return synch;}

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
     * Returns total number of partitions.
     */
    virtual int getNumPartitions() const;

    /**
     * Returns id of this partition, an integer in the range 0..getNumPartitions()-1.
     */
    virtual int getProcId() const;

    /**
     * Returns true if the named future submodule of the given parent module is,
     * or will have any submodule, in the local partition. The submoduleIndex
     * parameter should be -1 if the submodule is not part of a module vector.
     *
     * Note that for compound modules that contain simple modules in
     * several partitions, this function will return true on all those
     * partitions.
     */
    virtual bool isModuleLocal(cModule *parent, const char *submoduleName, int submoduleIndex);

    /**
     * A hook called from cProxyGate::deliver() when an outgoing cMessage
     * arrives at partition boundary. We just pass it up to the synchronization
     * layer (see similar method in cParsimSynchronizer).
     * The return value is a "keepit" flag that cProxyGate::deliver() will
     * return; see cGate::deliver() for explanation.
     */
    virtual bool processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data);

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
     * Called when an exception occurs, this methods notifies other partitions
     * about the exception. This handles both cTerminationExceptions (i.e. when
     * the simulation is about to be finished normally), and others (when the
     * simulation is about to be stopped with an error message).
     *
     * This methods "eats" exceptions that occur during broadcast. (We're not
     * interested in cascading exceptions.)
     */
    virtual void broadcastException(std::exception& e);
};

}  // namespace omnetpp


#endif

