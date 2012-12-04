package org.omnetpp.simulation.controller;


/**
 *
 * @author Andras
 */
public interface ISimulationChangeListener {
    /**
     * Called when the simulation's state has changed
     */
    void simulationStateChanged(SimulationChangeEvent e);
}
