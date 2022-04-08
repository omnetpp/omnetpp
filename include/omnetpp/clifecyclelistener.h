//==========================================================================
//   CLIFECYCLELISTENER.H  -  header for
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

#ifndef __OMNETPP_CLIFECYCLELISTENER_H
#define __OMNETPP_CLIFECYCLELISTENER_H

#include "cobject.h"

namespace omnetpp {

/**
 * @brief Event type for cISimulationLifecycleListener's lifecycleEvent() method.
 *
 * @ingroup Misc
 */
enum SimulationLifecycleEventType
{
    /**
     * Fired on the startup of the simulation program, after global
     * configuration has been applied, but before any simulation
     * has been set up.
     */
    LF_ON_STARTUP,

    /**
     * Fired before network setup. At the time of the call, cSimulation's
     * network type pointer (see getNetworkType()) already points to the type
     * of the network that will be set up.
     */
    LF_PRE_NETWORK_SETUP,

    /**
     * Fired immediately after a network has been set up, and before
     * callInitialize() has been invoked. If there has been an error during
     * network setup, then only LF_SIMULATION_ERROR will be fired but not
     * this event. For practical purposes, this event is almost the same as
     * LF_PRE_NETWORK_INITIALIZE.
     */
    LF_POST_NETWORK_SETUP,

    /**
     * Fired just before the network is initialized, i.e. before
     * callInitialize() has been invoked on the system module.
     */
    LF_PRE_NETWORK_INITIALIZE,

    /**
     * Fired just after the network was initialized, i.e. after
     * callInitialize() was invoked on the system module. If there is has been
     * an error during initialization, then only LF_SIMULATION_ERROR will be
     * fired but not this event.
     */
    LF_POST_NETWORK_INITIALIZE,

    /**
     * Fired immediately before the first event is processed by the simulation.
     * This is similar to, but not the same as LF_POST_NETWORK_INITIALIZE, because
     * on the GUI there might be a delay between the user setting up the network and
     * actually hitting the Run button.
     */
    LF_ON_SIMULATION_START,

    /**
     * Fired when simulation execution has been temporarily stopped by the user
     * between two events.
     */
    LF_ON_SIMULATION_PAUSE,

    /**
     * Fired when the user resumes simulation execution after it has been paused.
     * This event may be used e.g. by the real-time scheduler to resynchronize
     * with wall clock time.
     */
    LF_ON_SIMULATION_RESUME,

    /**
     * Fired just after processing the last event of the simulation (i.e. still
     * before finalization). If the simulation stops with an error,
     * LF_ON_SIMULATION_ERROR will be fired instead of this event.
     */
    LF_ON_SIMULATION_SUCCESS,

    /**
     * Fired on a runtime error, in any stage of a simulation run's lifecycle
     * (during network setup, initialization, in an event during simulation,
     * during finalization and network deletion.)
     */
    //TODO the detail object should contain the exception object
    LF_ON_SIMULATION_ERROR,

    /**
     * Fired just before the network is finalized, i.e. before callFinish()
     * has been invoked on the system module. This only happens if the
     * simulation has completed successfully (did not stop with an error).
     */
    LF_PRE_NETWORK_FINISH,

    /**
     * Fired immediately after the network has been finalized, i.e. after
     * callFinish() invocation has taken place on the system module. If there
     * has been an error during finalization, then only LF_ON_SIMULATION_ERROR
     * will be fired but not this event.
     */
    LF_POST_NETWORK_FINISH,

    /**
     * Fired after the simulation has been terminated, either by calling finish()
     * or because of an error any previous state (including network setup).
     * This event is distinct from LF_PRE_NETWORK_DELETE, because when using
     * a GUI, there might be a delay between the simulation terminating and
     * the user action that causes the network to be deleted.
     * Note that using the GUI, it is also possible for the user to skip
     * finalization of the network (even if there was no error), so
     * LF_PRE_NETWORK_FINISH / LF_POST_NETWORK_FINISH are not guaranteed to
     * have been fired prior to this event.
     */
    LF_ON_RUN_END,

    /**
     * Fired before the network is deleted, i.e. before cSimulation's
     * deleteNetwork() has been called. This event is fired even if
     * there was an error during network setup.
     */
    LF_PRE_NETWORK_DELETE,

    /**
     * Fired after the network has been deleted, i.e. cSimulation's
     * deleteNetwork() was called. At the time of the call, the network
     * type pointer in cSimulation (see getNetworkType()) is still valid.
     * This event is not fired if an error occurs during deleteNetwork().
     */
    LF_POST_NETWORK_DELETE,

    /**
     * Fired before the simulation program exits. At the time of the call,
     * the simulated network has been torn down, but the simulation
     * infrastructure (scheduler object, etc) is still in place.
     */
    LF_ON_SHUTDOWN,
};


/**
 * @brief A callback interface for receiving notifications at various stages
 * simulations, including setting up, running, and tearing down..
 *
 * Listeners can be added to cEnvir with its addLifecycleListener() method,
 * and removed with removeLifecycleListener().
 *
 * Simulation lifecycle listeners are mainly intended for use by plug-in
 * classes that extend the simulator's functionality: schedulers, output
 * vector/scalar managers, parallel simulation algorithms and so on. The
 * lifecycle of a such plug-in class is managed by \opp, so one can use
 * their constructor to add the listener to cEnvir, and the destructor
 * to remove it. (The latter is done automatically.)
 *
 * Notifications always refer to the active simulation (see
 * cSimulation::getActiveSimulation()).
 *
 * For standalone listeners that have no other way to be invoked, one can
 * use the <tt>EXECUTE_ON_STARTUP(code)</tt> macro to automatically add a
 * listener when the program starts:
 *
 * <pre>
 * EXECUTE_ON_STARTUP(getEnvir()->addLifecycleListener(new MyListener()));
 * </pre>
 *
 * Note that listeners will not be deleted automatically when the program
 * exits. To delete a listener on shutdown, add <tt>delete this;</tt>
 * into the listenerRemoved() method.
 *
 * @ingroup Misc
 */
class SIM_API cISimulationLifecycleListener
{
  public:
    /**
     * The destructor removes the listener from cEnvir if it is still added.
     */
    virtual ~cISimulationLifecycleListener();

    /**
     * The main listener method, called on simulation lifecycle events.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) = 0;

    /**
     * Fired after this listener was added to cEnvir, via addLifecycleListener().
     */
    virtual void listenerAdded() {}

    /**
     * Fired after this listener was removed from cEnvir, via removeLifecycleListener().
     * It is OK for the listener to delete itself in the body of this method
     * (<tt>delete this</tt>).
     */
    virtual void listenerRemoved() {}

    /**
     * Returns the string representation of the simulation lifecycle event type.
     */
    static const char *getSimulationLifecycleEventName(SimulationLifecycleEventType eventType);
};

}  // namespace omnetpp

#endif
