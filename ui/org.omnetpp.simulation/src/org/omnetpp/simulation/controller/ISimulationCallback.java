package org.omnetpp.simulation.controller;

import java.net.SocketException;

/**
 * This interface is used by Simulation to notify when the simulation's state
 * has been re-read, the simulation process has exited, etc.
 *
 * @author Andras
 */
public interface ISimulationCallback {
    /**
     * Called when the socket cannot connect to the simulation, which we interpret
     * as a fatal condition. Timeout (which is likely a transient error) does NOT trigger
     * this callback; see subclasses of SocketException what does.
     */
    void socketError(SocketException e);

    /**
     * Called when the simulation process exits (according to the Job that was used to launch it).
     */
    void simulationProcessExited();

}
