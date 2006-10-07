//=========================================================================
//  CPARSIMSYNCHR.H - part of
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

#ifndef __CPARSIMSYNCHR_H__
#define __CPARSIMSYNCHR_H__

#include "cscheduler.h"

// forward declarations
class cSimulation;
class cMessage;
class cParsimPartition;
class cParsimCommunications;

/**
 * Abstract base class for parallel simulation algorithms.
 * Subclasses implement the "null message algorithm" and others.
 *
 * Task of this layer is to implement conservative synchronization, that is,
 * block the execution at time points where it is not safe to proceed
 * since events from other partitions will (may) arrive before the next
 * local event.
 *
 * Synchronizer integrates into the simulation kernel via the
 * getNextEvent() and processOutgoingMessage() functions:
 *
 * - getNextEvent() is the scheduler function (comes from cScheduler)
 *
 * - processOutgoingMessage() is invoked when messages are sent
 * out of the partition, from gates (see cProxyGate) of placeholder modules
 * (cPlaceHolderModule) via cParsimPartition.
 *
 * @ingroup Parsim
 */
class cParsimSynchronizer : public cScheduler
{
  protected:
    cParsimPartition *partition;
    cParsimCommunications *comm;

  public:
    /**
     * Constructor.
     */
    cParsimSynchronizer() : cScheduler()  {partition = NULL; comm = NULL;}

    /**
     * Destructor.
     */
    virtual ~cParsimSynchronizer() {}

    /**
     * Pass cParsimSynchronizer the objects it has to cooperate with.
     */
    virtual void setContext(cSimulation *sim, cParsimPartition *seg, cParsimCommunications *co)
       {setSimulation(sim); partition = seg; comm = co;}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() = 0;

    /**
     * Scheduler function -- it comes from cScheduler interface.
     */
    virtual cMessage *getNextEvent() =0;

    /**
     * Hook, called when a cMessage is sent out of the partition.
     * It is provided here so that the synchronizer can potentially
     * perform optimizations, such as piggybacking null messages
     * (see null message algorithm) on outgoing messages.
     */
    virtual void processOutgoingMessage(cMessage *msg, int procId, int moduleId, int gateId, void *data) = 0;
};

#endif

