package org.omnetpp.scave.writers;

/**
 * Simulation time provider interface, for use with IOutputVectorManager.
 * A simulation time provider should be implemented by the
 * simulation package, and passed to instances of IOutputVectorManager.
 * The <code>getSimulationTime()</code> method will be used to generate a
 * timestamp for values recorded via the <code>record(double value)</code>
 * method of IOutputVector.
 *
 * @author Andras
 */
public interface ISimulationTimeProvider {
    /**
     * Returns the current simulation time.
     */
    Number getSimulationTime();

    /**
     * Returns the current event number. If the simulator does
     * not keep track of event numbers, it should return -1.
     * This method is currently unused.
     */
    long getEventNumber();
}
