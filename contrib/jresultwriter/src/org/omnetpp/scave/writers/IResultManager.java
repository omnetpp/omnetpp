package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Contains common methods for IOutputScalarManager and IOutputVectorManager.
 * 
 * @author Andras
 */
public interface IResultManager extends IResultAttributes {
    /**
     * Opens collecting. Called at the beginning of a simulation run.
     * @param runID TODO
     */
    void open(String runID, Map<String,String> runAttributes);

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    void close();

    /**
     * Force writing out all buffered output.
     */
    void flush();
}
