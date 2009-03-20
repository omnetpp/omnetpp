package org.omnetpp.scave.writers;

import java.io.IOException;
import java.util.Map;

/**
 * Abstract base class for handling recording of output scalar data.
 * 
 * @author Andras
 */
public interface IOutputScalarManager extends IResultManager {
    /**
     * Opens collecting. Called at the beginning of a simulation run.
     * @throws IOException 
     */
    void open(Map<String,String> runAttributes) throws IOException;

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
     * Returns the output scalar file name. Returns NULL if this object is not
     * producing file output.
     */
    String getFileName();

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    void recordScalar(String componentPath, String statisticName, double value, Map<String,String> attributes);

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    void recordScalar(String componentPath, String statisticName, Number value, Map<String,String> attributes);

    /**
     * Records a histogram or statistic object into the scalar result file.
     * If the object additionally implements the IStatisticalSummary2 and/or 
     * IHistogramSummary interfaces, more data are recorded.
     */
    void recordStatistic(String componentPath, String statisticName, IStatisticalSummary statistic, Map<String,String> attributes);
}
