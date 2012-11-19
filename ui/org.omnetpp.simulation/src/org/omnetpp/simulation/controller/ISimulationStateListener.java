package org.omnetpp.simulation.controller;


/**
 *
 * Note: callbacks are just informational, we might miss something done by the
 * simulation process behind our back.
 *
 * @author Andras
 */
//TODO Note: this interface is incomplete; new functions to be added as needed
public interface ISimulationStateListener {
    //TODO void simulationConnected();

    void networkSetUp(SimulationController controller);

    /**
     * Called each time the simulation has processed some events; before animations.
     */
    void eventsProcessed(SimulationController controller);

    //TODO void simulationError();
    //TODO void simulationCompleted(); // when the simulation goes to TERMINATED? (XXX rename TERMINATED to COMPLETED?)
    //TODO void finishCalled();  // only after error? because simulationCompleted() already includes finish() too

    void beforeNetworkDelete(SimulationController controller);

    //TODO void networkDeleted();
    //TODO void simulationDisconnected();
}
