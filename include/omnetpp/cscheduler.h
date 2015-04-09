//=========================================================================
//  CSCHEDULER.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSCHEDULER_H
#define __CSCHEDULER_H

#include "platdep/timeutil.h"  // for timeval
#include "cobject.h"
#include "clifecyclelistener.h"

NAMESPACE_BEGIN

// forward declarations
class cEvent;
class cSimulation;

/**
 * Abstract class to encapsulate event scheduling. See description
 * of takeNextEvent() for more info.
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
 * scheduler-class = "MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 * @ingroup Internals
 */
class SIM_API cScheduler : public cObject, public cISimulationLifecycleListener
{
  protected:
    cSimulation *sim;

  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and
     * executionResumed(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details);

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
    virtual void startRun() {}

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() {}

    /**
     * Called every time the user hits the Run button in Tkenv.
     * Real-time schedulers (e.g. cRealTimeScheduler) may make use of
     * this callback to pin current simulation time to current
     * wall clock time.
     */
    virtual void executionResumed() {}

    /**
     * Return the likely next event in the simulation. This method is for UI
     * purposes, it does not play any role in the simulation. A basic
     * implementation would just return a pointer to the first event in the FES,
     * which is accurate for sequential simulation; with parallel, distributed or
     * real-time simulation there might be other events coming from other processes
     * with a yet smaller timestamp.
     *
     * This method should not have side effects, except for discarding stale events
     * from the FES.
     */
    virtual cEvent *guessNextEvent() = 0;

    /**
     * Return the next event to be processed. Normally (with sequential execution),
     * it just returns the first event in the FES. With parallel and/or real-time
     * simulation, it is also the scheduler's task to synchronize with real time
     * and/or with other partitions.
     *
     * If there's no more event, it throws cTerminationException.
     *
     * A NULL return value means that there's no error but execution
     * was stopped by the user (e.g. with STOP button on the GUI)
     * while takeNextEvent() was waiting for external synchronization.
     */
    virtual cEvent *takeNextEvent() = 0;

    /**
     * Undo for takeNextEvent(), approximately: if an event was obtained from
     * takeNextEvent() but was not yet processed, it is possible to temporarily
     * put it back to the FES.
     *
     * The scheduler class must guarantee that removing the event via
     * takeNextEvent() again does NOT repeat the side effects of the
     * first takeNextEvent()! That is, the sequence
     *
     * <pre>
     * e = takeNextEvent();
     * putBackEvent(e);
     * e = takeNextEvent();
     * </pre>
     *
     * should be equivalent to a single takeNextEvent() call.
     */
    virtual void putBackEvent(cEvent *event) = 0;
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
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent();

    /**
     * Removes the first event from the Future Event Set, and returns it.
     */
    virtual cEvent *takeNextEvent();

    /**
     * Puts back the event into the Future Event Set.
     */
    virtual void putBackEvent(cEvent *event);
};


/**
 * Real-time scheduler class. When installed as scheduler using the scheduler-class
 * omnetpp.ini entry, it will synchronize simulation execution to real (wall clock)
 * time.
 *
 * Operation: a "base time" is determined when startRun() is called. Later on,
 * the scheduler object calls usleep() from takeNextEvent() to synchronize the
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
//TODO soft realtime, hard realtime (that is: tries to catch up, or resynchronizes on each event)
class SIM_API cRealTimeScheduler : public cScheduler
{
  protected:
    // configuration:
    bool doScaling;
    double factor;

    // state:
    timeval baseTime;

  protected:
    virtual void startRun();
    bool waitUntil(const timeval& targetTime);

  public:
    /**
     * Constructor.
     */
    cRealTimeScheduler();

    /**
     * Destructor.
     */
    virtual ~cRealTimeScheduler();

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed();

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent();

    /**
     * Scheduler function -- it comes from cScheduler interface.
     * This function synchronizes to real time: before returning the
     * first event from the FES, it waits (using usleep()) until
     * the real time reaches the time of that simulation event.
     */
    virtual cEvent *takeNextEvent();

    /**
     * Puts back the event into the Future Event Set.
     */
    virtual void putBackEvent(cEvent *event);
};

NAMESPACE_END


#endif

