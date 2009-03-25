package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Contains common methods for IOutputScalarManager and IOutputVectorManager.
 * Methods throw ResultRecordingException on errors. 
 * 
 * @author Andras
 */
public interface IResultManager extends IResultAttributes {
    /**
     * Opens collecting. This should be called at the beginning of a simulation run.
     * 
     * @param runID  reasonably globally unique string to identify the run.
     * @param attributes  run attributes; may be null. See IResultAttributes 
     *      for potential attribute names. 
     */
    void open(String runID, Map<String,String> runAttributes);

    /**
     * Close collecting. This should be called at the end of a simulation run.
     */
    void close();

    /**
     * Force writing out all buffered output.
     */
    void flush();

    /**
     * Returns the name of the output scalar/vector file. Returns null if this 
     * object is not producing file output.
     */
    String getFileName();
}
