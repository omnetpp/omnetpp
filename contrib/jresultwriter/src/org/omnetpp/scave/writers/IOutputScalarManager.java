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
