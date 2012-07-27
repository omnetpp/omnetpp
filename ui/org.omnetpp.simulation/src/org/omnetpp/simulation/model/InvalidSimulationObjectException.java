package org.omnetpp.simulation.model;

/**
 * Thrown when members of an unfilled or already disposed simulation object (cObject) is accessed.
 *
 * @author Andras
 */
public class InvalidSimulationObjectException extends RuntimeException {

    public InvalidSimulationObjectException() {
    }

    public InvalidSimulationObjectException(String message) {
        super(message);
    }
}
