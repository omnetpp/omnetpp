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

#include "simkerneldefs.h"

namespace omnetpp {

class cObject;
class cSimulation;

/**
 * @brief Event type for cISimulationLifecycleListener's lifecycleEvent() method.
 *
 * @ingroup Misc
 */
enum SimulationLifecycleEventType
{
    /**
     * Stands for "No lifecycle event", can be used as the invalid value.
     */
    LF_NONE,

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
     * Fired between LF_POST_NETWORK_FINISH and LF_PRE_NETWORK_DELETE. It is fired
     * just after LF_POST_NETWORK_FINISH if the simulation has completed successfully,
     * and just before LF_PRE_NETWORK_DELETE if it has not (i.e. there was no
     * LF_POST_NETWORK_FINISH notification, e.g. due to a previous error).
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
};


/**
 * @brief A callback interface for receiving notifications at various stages
 * simulations, including setting up, running, and tearing down..
 *
 * Simulation lifecycle listeners can be added to a simulation instance (cSimulation)
 * with its addLifecycleListener() method, and removed with removeLifecycleListener().
 *
 * Simulation lifecycle listeners are mainly intended for use by plug-in
 * classes that extend the simulator's functionality: schedulers, output
 * vector/scalar managers, parallel simulation algorithms and so on. The
 * lifecycle of a such plug-in class is managed by \opp, so one can use
 * their constructor to add the listener to cEnvir, and the destructor
 * to remove it. (The latter is done automatically.)
 *
 * Note that listeners will not be deleted automatically when their
 * simulation instance is deleted. To delete a listener, add <tt>delete this;</tt>
 * in the listenerRemoved() method.
 *
 * @ingroup Misc
 */
class SIM_API cISimulationLifecycleListener
{
  private:
    cSimulation *simulation = nullptr;

  public:
    /**
     * The destructor removes the listener from the simulation if it is still added.
     */
    virtual ~cISimulationLifecycleListener();

    /**
     * Returns the simulation instance the lifecycle listener is subscribed to.
     */
    virtual cSimulation *getSimulation() const {return simulation;}

    /**
     * The main listener method, called on simulation lifecycle events.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) = 0;

    /**
     * Fired after this listener was added to cSimulation, via addLifecycleListener().
     */
    virtual void listenerAdded(cSimulation *simulation) {this->simulation = simulation;}

    /**
     * Fired after this listener was removed from cSimulation, via removeLifecycleListener().
     * It is OK for the listener to delete itself in the body of this method
     * (<tt>delete this</tt>).
     */
    virtual void listenerRemoved() {simulation = nullptr;}

    /**
     * Returns the string representation of the simulation lifecycle event type.
     */
    static const char *getSimulationLifecycleEventName(SimulationLifecycleEventType eventType);
};

}  // namespace omnetpp

#endif
