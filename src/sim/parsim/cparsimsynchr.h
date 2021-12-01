//=========================================================================
//  CPARSIMSYNCHR.H - part of
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

#ifndef __OMNETPP_CPARSIMSYNCHR_H
#define __OMNETPP_CPARSIMSYNCHR_H

#include "omnetpp/cscheduler.h"
#include "omnetpp/simtime_t.h"

namespace omnetpp {

class cSimulation;
class cMessage;
class cParsimPartition;
class cParsimCommunications;

/**
 * @brief Abstract base class for parallel simulation algorithms.
 * Subclasses implement the "null message algorithm" and others.
 *
 * Task of this layer is to implement conservative synchronization, that is,
 * block the execution at time points where it is not safe to proceed
 * since events from other partitions will (may) arrive before the next
 * local event.
 *
 * Synchronizer integrates into the simulation kernel via the
 * takeNextEvent() and processOutgoingMessage() functions:
 *
 * - takeNextEvent() is the scheduler function (comes from cScheduler)
 *
 * - processOutgoingMessage() is invoked when messages are sent
 * out of the partition, from gates (see cProxyGate) of placeholder modules
 * (cPlaceholderModule) via cParsimPartition.
 *
 * @ingroup Parsim
 */
class SIM_API cParsimSynchronizer : public cScheduler
{
  protected:
    cParsimPartition *partition;
    cParsimCommunications *comm;

  public:
    /**
     * Constructor.
     */
    cParsimSynchronizer() : cScheduler()  {partition = nullptr; comm = nullptr;}

    /**
     * Destructor.
     */
    virtual ~cParsimSynchronizer() {}

    /**
     * Return "parsim scheduler" as description.
     */
    virtual std::string str() const override;

    /**
     * Pass cParsimSynchronizer the objects it has to cooperate with.
     */
    virtual void setContext(cSimulation *sim, cParsimPartition *seg, cParsimCommunications *co)
       {setSimulation(sim); partition = seg; comm = co;}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override = 0;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override = 0;

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent() override;

    /**
     * Scheduler function -- it comes from cScheduler interface.
     */
    virtual cEvent *takeNextEvent() override = 0;

    /**
     * Hook, called when a cMessage is sent out of the partition.
     * It is provided here so that the synchronizer can potentially
     * perform optimizations, such as piggybacking null messages
     * (see null message algorithm) on outgoing messages.
     */
    virtual void processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data) = 0;
};

}  // namespace omnetpp


#endif

