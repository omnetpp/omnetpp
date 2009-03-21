package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Interface for recording output scalar data.
 * Errors throw ResultRecordingException.
 *  
 * @author Andras
 */
public interface IOutputScalarManager extends IResultManager {
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
