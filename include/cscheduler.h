//=========================================================================
//
// CSCHEDULER.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2004 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSCHEDULER_H
#define __CSCHEDULER_H

#include "cpolymorphic.h"

// forward declarations
class cSimulation;
class cMessage;

/**
 * Abstract class to encapsulate event scheduling. See description
 * of getNextEvent() for more info.
 *
 * @ingroup Internals
 */
class cScheduler : public cPolymorphic
{
  protected:
    cSimulation *sim;

  public:
    /**
     * Constructor.
     */
    cScheduler();

    /**
     * Destructor.
     */
    virtual ~cScheduler();

    /**
     * Pass cSimulation object to scheduler.
     */
    virtual void setSimulation(cSimulation *_sim);

    /**
     * The scheduler function -- it should return the next event
     * to be processed. Normally (with sequential execution) it just
     * returns msgQueue.peekFirst(). With parallel and/or real-time
     * simulation, it is also the scheduler's task to synchronize
     * with real time and/or with other partitions.
     * 
     * If there's no more event, it throws cTerminationException.
     *
     * A NULL return value means that there's no error but execution
     * was stopped by the user (e.g. with STOP button on the GUI) 
     * while getNextEvent() was waiting for external synchronization.
     */
    virtual cMessage *getNextEvent() = 0;
};

/**
 * Event scheduler for sequential simulation.
 *
 * @ingroup Internals
 */
class cSequentialScheduler : public cScheduler
{
  public:
    /**
     * Constructor.
     */
    cSequentialScheduler() {}

    /**
     * Returns the first event in the Future Event Set (that is,
     * msgQueue.peekFirst()).
     */
    virtual cMessage *getNextEvent();
};

#endif

