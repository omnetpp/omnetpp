package org.omnetpp.simulation.controller;

import java.net.SocketException;

/**
 * This interface is used by Simulation to notify SimulationController of certain events.
 *
 * @author Andras
 */
public interface ISimulationCallback {
    /**
     *
     */
    void communicationInterrupted();

    /**
     * TODO
     * @param e TODO
     */
    void transientCommunicationFailure(Exception e);

    /**
     * Called when the socket cannot connect to the simulation, which we interpret
     * as a fatal condition. Timeout (which is likely a transient error) does NOT trigger
     * this callback; see subclasses of SocketException what does.
     */
    void fatalCommunicationError(SocketException e);
}
