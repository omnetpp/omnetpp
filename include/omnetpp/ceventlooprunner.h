//==========================================================================
//   CEVENTLOOPRUNNER.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CEVENTLOOPRUNNER_H
#define __OMNETPP_CEVENTLOOPRUNNER_H

#include "simkerneldefs.h"

namespace omnetpp {

class cSimulation;
class cConfiguration;

/**
 * @brief Encapsulates the discrete simulation event loop in cSimulation.
 * The loop repeats the two steps of (1) getting the next event from the FES
 * and (2) executing the event, until the simulation is over.
 *
 * In practice, the event loop may contain additional stuff like printing
 * periodic status updates, calling refreshDisplay() after each event or
 * with some frequency, or implementing "run until" functionality. This
 * is why various implementations exist.
 *
 * @see cSimulation::run()
 *
 * @ingroup SimSupport
 */
class SIM_API cIEventLoopRunner
{
  protected:
    cSimulation *simulation; // not owned

  public:
    /**
     * The constructor accepts the simulation object on which this object
     * is going to operate.
     */
    cIEventLoopRunner(cSimulation *simulation) : simulation(simulation) {}

    /**
     * Destructor.
     */
    virtual ~cIEventLoopRunner();

    /**
     * Configures the object.
     */
    virtual void configure(cConfiguration *cfg) = 0;

    /**
     * Exceptions thrown from the simulation are propagated. The method is
     * exited in one of 3 ways:
     *
     *   1. cTerminationException is thrown. This means the simulation has completed.
     *   2. Some other exception. This means the simulation has run into an error.
     *   3. Normal return. It is done if the simulation reach some kind of
     *      breakpoint (e.g. "run until").
     *
     * Generally, run() may be only called again if the previous call returned
     * normally (case 3). For cases 1 and 2, the simulation is considered finished;
     * successfully or with an error.
     */
    virtual void runEventLoop() = 0;
};

}  // namespace omnetpp

#endif


