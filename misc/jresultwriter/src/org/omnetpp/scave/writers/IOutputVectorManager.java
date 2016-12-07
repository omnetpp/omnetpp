package org.omnetpp.scave.writers;

import java.util.Map;


/**
 * Interface for recording output vector data. Output vectors are series of
 * (timestamp,value) pairs. It is recommended to have only one instance of
 * IOutputVectorManager for the whole simulation, and simulation objects
 * (components) should have <code>createVector()</code> or similar methods
 * that delegate to the similar method of the global IOutputVectorManager
 * instance. Actual data can be recorded via methods of the IOutputVector
 * instance returned from <code>createVector()</code>.
 *
 * Methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
public interface IOutputVectorManager extends IResultManager {
    /**
     * Creates an output vector for recording the given vector of the given
     * component.
     *
     * @param componentPath  full path name of the component that records the
     *      vector. The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param vectorName  name of the output vector. This should not usually
     *      contain dots.
     * @param attributes  vector attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    IOutputVector createVector(String componentPath, String vectorName, Map<String,String> attributes);

    /**
     * Sets the simulation time provider, to be used for generating a timestamp
     * when a value is recorded without an explicit timestamp.
     */
    void setSimtimeProvider(ISimulationTimeProvider simtimeProvider);

    /**
     * Returns the simulation time provider, to be used for generating a timestamp
     * when a value is recorded without an explicit timestamp.
     */
    ISimulationTimeProvider getSimtimeProvider();

}
