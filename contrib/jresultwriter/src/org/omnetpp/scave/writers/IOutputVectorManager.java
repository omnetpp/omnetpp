package org.omnetpp.scave.writers;

import java.io.IOException;
import java.util.Map;


/**
 * Abstract base class for output vector managers.
 * 
 * @author Andras
 */
public interface IOutputVectorManager extends IResultManager {
    /**
     * Opens collecting. Called at the beginning of a simulation run.
     * @param runID TODO
     * @throws IOException 
     */
    void open(String runID, Map<String,String> runAttributes) throws IOException;

    /**
     * Closes collecting. Called at the end of a simulation run.
     * @throws IOException 
     */
    void close() throws IOException;

    /**
     * Force writing out all buffered output.
     * @throws IOException 
     */
    void flush() throws IOException;

    /**
     * Returns the output vector file name. Returns null if this object is not
     * producing file output.
     */
    String getFileName();

    /**
     * Creates an output vector.
     */
    IOutputVector createVector(String componentPath, String vectorName, Map<String,String> attributes);
    
    /**
     * XXX
     * @return
     */
    ISimulationTimeProvider getSimtimeProvider();
 
    /**
     * XXX
     * @param simtimeProvider
     */
    void setSimtimeProvider(ISimulationTimeProvider simtimeProvider);
    
}
