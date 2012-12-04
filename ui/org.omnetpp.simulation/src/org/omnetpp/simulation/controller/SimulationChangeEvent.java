package org.omnetpp.simulation.controller;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.simulation.controller.Simulation.RunMode;
import org.omnetpp.simulation.controller.Simulation.SimState;
import org.omnetpp.simulation.controller.SimulationController.ConnState;

/**
 * Provides details in an ISimulationChangeListener invocation.
 * 
 * Note: some event types may not be fired every time they are supposed to, 
 * because the front-end doesn't know if the simulation process does something
 * behind our back.
 * 
 * @author andras
 */
public class SimulationChangeEvent {
    public enum Reason { 
        STATUS_REFRESH, // fired after Simulation.refreshStatus()
        OBJECTCACHE_REFRESH,  // fired after Simulation.refreshObjectCache()
        CONNSTATE_CHANGE, SIMSTATE_CHANGE, RUNMODE_CHANGE,  // fired  after the corresponding state variables have changed
        EVENTS_PROCESSED,  // fired each time the simulation has processed some events, but before the animations
        BEFORE_NETWORK_DELETION,  // fired before deleting an old network
        NETWORK_SET_UP  // fired after a network has been set up
    };
    public Reason reason;
    public SimulationController simulationController;
    public ConnState oldConnState; // for CONNSTATE_CHANGE
    public ConnState newConnState; // for CONNSTATE_CHANGE
    public SimState oldSimState;  // for SIMSTATE_CHANGE
    public SimState newSimState;  // for SIMSTATE_CHANGE
    public RunMode oldRunMode; // for RUNMODE_CHANGE
    public RunMode newRunMode; // for RUNMODE_CHANGE

    public SimulationChangeEvent(Reason reason) {
        this.reason = reason;
    }

    public SimulationChangeEvent(Reason reason, ConnState oldState, ConnState newState) {
        Assert.isTrue(reason == Reason.CONNSTATE_CHANGE);
        this.reason = reason;
        this.oldConnState = oldState;
        this.newConnState = newState;
    }

    public SimulationChangeEvent(Reason reason, SimState oldState, SimState newState) {
        Assert.isTrue(reason == Reason.SIMSTATE_CHANGE);
        this.reason = reason;
        this.oldSimState = oldState;
        this.newSimState = newState;
    }

    public SimulationChangeEvent(Reason reason, RunMode oldRunMode, RunMode newRunMode) {
        Assert.isTrue(reason == Reason.RUNMODE_CHANGE);
        this.reason = reason;
        this.oldRunMode = oldRunMode;
        this.newRunMode = newRunMode;
    }

}
