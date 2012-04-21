package org.omnetpp.simulation.controller;


/**
 *
 * @author Andras
 */
public interface ISimulationStateListener {
    /**
     * Called when the simulation's state has changed
     */
    void simulationStateChanged(SimulationController controller);
}
