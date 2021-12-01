//=========================================================================
//  CNOSYNCHRONIZATION.H - part of
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

#ifndef __OMNETPP_NOSYNCHRONIZATION_H
#define __OMNETPP_NOSYNCHRONIZATION_H

#include "omnetpp/cfutureeventset.h"
#include "cparsimprotocolbase.h"

namespace omnetpp {

class cCommBuffer;

/**
 * @brief "Dummy" implementation -- just pass messages between partitions,
 * without any synchronization. Of course incausalities may occur
 * which terminate the simulation with error, so this class is
 * only useful as a base "template" for implementing "real" synchronization
 * protocols.
 *
 * @ingroup Parsim
 */
class SIM_API cNoSynchronization : public cParsimProtocolBase
{
  public:
    /**
     * Constructor.
     */
    cNoSynchronization() {}

    /**
     * Destructor.
     */
    virtual ~cNoSynchronization() {}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /**
     * Scheduler function. It processes whatever comes from other partitions,
     * then returns getFES()->peekFirst(). Performs no synchronization
     * with other partitions, so incausalities may occur which stop the
     * simulation with an error (see also class comment).
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(cEvent *event) override;
};

}  // namespace omnetpp


#endif

