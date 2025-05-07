//=========================================================================
//  CSCHEDULER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CSCHEDULER_H
#define __OMNETPP_CSCHEDULER_H

#include "cobject.h"
#include "simtime_t.h"
#include "clifecyclelistener.h"

namespace omnetpp {

class cEvent;
class cSimulation;

/**
 * @brief Abstract class to encapsulate event scheduling.
 *
 * The central method is takeNextEvent().
 *
 * To switch to your own scheduler class (reasons you'd like to do that
 * include real-time simulation, hardware-in-the-loop simulation,
 * distributed (federated) simulation, parallel distributed simulation),
 * subclass cScheduler, register your new class with
 * the Register_Class() macro, then add the following to
 * `omnetpp.ini`:
 *
 * ```
 * [General]
 * scheduler-class = "MyClass"
 * ```
 *
 * @ingroup SimCore
 */
class SIM_API cScheduler : public cObject, public cISimulationLifecycleListener
{
  protected:
    cSimulation *sim = nullptr;

  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and
     * executionResumed(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /**
     * Constructor.
     */
    cScheduler() {}

    /**
     * Destructor.
     */
    virtual ~cScheduler() {}

    /**
     * Return a short description. This string will be displayed in Qtenv
     * as scheduler information. Returning an empty string means
     * "default scheduler", and is reserved for cSequentialScheduler.
     */
    virtual std::string str() const override;

    /**
     * Pass cSimulation object to scheduler.
     */
    virtual void setSimulation(cSimulation *_sim);

    /**
     * Returns the simulation the scheduler belongs to.
     */
    cSimulation *getSimulation() const {return sim;}

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() {}

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() {}

    /**
     * Called every time the user hits the Run button in Qtenv.
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
     * A nullptr return value means that there's no error but execution
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
     * ```
     * e = takeNextEvent();
     * putBackEvent(e);
     * e = takeNextEvent();
     * ```
     *
     * should be equivalent to a single takeNextEvent() call.
     */
    virtual void putBackEvent(cEvent *event) = 0;
};


/**
 * @brief Event scheduler for sequential simulation.
 *
 * @ingroup SimCore
 */
class SIM_API cSequentialScheduler : public cScheduler
{
  public:
    /**
     * Constructor.
     */
    cSequentialScheduler() {}

    /**
     * Returns empty string as description.
     */
    virtual std::string str() const override;

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent() override;

    /**
     * Removes the first event from the Future Event Set, and returns it.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Puts back the event into the Future Event Set.
     */
    virtual void putBackEvent(cEvent *event) override;
};


/**
 * @brief Real-time scheduler class.
 *
 * When installed as scheduler using the scheduler-class omnetpp.ini entry,
 * it will synchronize simulation execution to real (wall clock) time.
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
 * @ingroup SimCore
 */
//TODO soft realtime, hard realtime (that is: tries to catch up, or resynchronizes on each event)
class SIM_API cRealTimeScheduler : public cScheduler
{
  protected:
    // configuration:
    bool doScaling;
    double factor;

    // state:
    int64_t baseTime;  // in microseconds

  protected:
    virtual void startRun() override;
    bool waitUntil(int64_t targetTime); // in microseconds
    int64_t toUsecs(simtime_t t);

  public:
    /**
     * Constructor.
     */
    cRealTimeScheduler() {}

    /**
     * Destructor.
     */
    virtual ~cRealTimeScheduler() {}

    /**
     * Returns a description that depends on the parametrization of this class.
     */
    virtual std::string str() const override;

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed() override;

    /**
     * Returns the first event in the Future Event Set.
     */
    virtual cEvent *guessNextEvent() override;

    /**
     * Scheduler function -- it comes from cScheduler interface.
     * This function synchronizes to real time: before returning the
     * first event from the FES, it waits (using usleep()) until
     * the real time reaches the time of that simulation event.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Puts back the event into the Future Event Set.
     */
    virtual void putBackEvent(cEvent *event) override;
};

}  // namespace omnetpp


#endif

