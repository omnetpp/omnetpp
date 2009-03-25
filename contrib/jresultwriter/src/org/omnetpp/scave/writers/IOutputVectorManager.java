package org.omnetpp.scave.writers;

import java.util.Map;


/**
 * Abstract base class for output vector managers.
 *
 * @author Andras
 */
public interface IOutputVectorManager extends IResultManager {
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
