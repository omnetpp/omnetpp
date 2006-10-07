//=========================================================================
//  CSCHEDULER.H - part of
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

#ifndef __CSCHEDULER_H
#define __CSCHEDULER_H

#include "cpolymorphic.h"
#include "platdep/time.h"  // for timeval

// forward declarations
class cSimulation;
class cMessage;

/**
 * Abstract class to encapsulate event scheduling. See description
 * of getNextEvent() for more info.
 *
 * To switch to your own scheduler class (reasons you'd like to do that
 * include real-time simulation, hardware-in-the-loop simulation,
 * distributed (federated) simulation, parallel distributed simulation),
 * subclass cScheduler, register your new class with
 * the Register_Class() macro, then add the following to
 * <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * scheduler-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 * @ingroup Internals
 */
class SIM_API cScheduler : public cPolymorphic
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
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() = 0;

    /**
     * Called every time the user hits the Run button in Tkenv.
     * Real-time schedulers (e.g. cRealTimeScheduler) may make use
     * this callback to pin current simulation time to current
     * wall clock time.
     */
    virtual void executionResumed()  {}

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
class SIM_API cSequentialScheduler : public cScheduler
{
  public:
    /**
     * Constructor.
     */
    cSequentialScheduler() {}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() {}

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() {}

    /**
     * Returns the first event in the Future Event Set (that is,
     * msgQueue.peekFirst()).
     */
    virtual cMessage *getNextEvent();
};


/**
 * Real-time scheduler class. When installed as scheduler using the scheduler-class
 * omnetpp.ini entry, it will syncronize simulation execution to real (wall clock)
 * time.
 *
 * Operation: a "base time" is determined when startRun() is called. Later on,
 * the scheduler object calls usleep() from getNextEvent() to synchronize the
 * simulation time to real time, that is, to wait until
 * the current time minus base time becomes equal to the simulation time.
 * Should the simulation lag behind real time, this scheduler will try to catch up
 * by omitting sleep calls altogether.
 *
 * Scaling is supported via the realtimescheduler-scaling omnetpp.ini entry.
 * For example, if it is set to 2.0, the simulation will try to execute twice
 * as fast as real time.
 *
 * @ingroup Internals
 */
class SIM_API cRealTimeScheduler : public cScheduler
{
  protected:
    // configuration:
    bool doScaling;
    double factor;

    // state:
    timeval baseTime;

    bool waitUntil(const timeval& targetTime);

  public:
    /**
     * Constructor.
     */
    cRealTimeScheduler() : cScheduler()  {}

    /**
     * Destructor.
     */
    virtual ~cRealTimeScheduler() {}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun();

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed();

    /**
     * Scheduler function -- it comes from cScheduler interface.
     * This function synchronizes to real time: it waits (usleep()) until
     * the real time reaches the time of the next simulation event.
     */
    virtual cMessage *getNextEvent();
};

#endif

