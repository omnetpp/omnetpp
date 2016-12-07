package org.omnetpp.scave.writers;

import java.util.Map;

/**
 * Interface for recording output scalar data. It is recommended to have only one
 * instance of IOutputScalarManager for the whole simulation, and simulation objects
 * (components) should have <code>recordScalar()</code> methods that delegate to
 * methods of the global IOutputScalarManager instance.
 *
 * IOutputScalarManager methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
public interface IOutputScalarManager extends IResultManager {
    /**
     * Records a numeric scalar result.
     *
     * @param componentPath  full path name of the component that records the statistic.
     *      The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param statisticName  name of the statistic. This should not usually
     *      contain dots.
     * @param attributes  statistic attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    void recordScalar(String componentPath, String statisticName, Number value, Map<String,String> attributes);

    /**
     * Records a histogram or statistic object. If the object additionally implements
     * the IStatisticalSummary2 and/or IHistogramSummary interfaces, more data are recorded.
     *
     * @param componentPath  full path name of the component that records the statistic.
     *      The recommended separator for path segments is a dot.
     *      Example: <code>"net.host[2].app[1]"</code>
     * @param statisticName  name of the statistic object. This should not usually
     *      contain dots.
     * @param attributes  statistic attributes; may be null. See IResultAttributes
     *      for potential attribute names.
     */
    void recordStatistic(String componentPath, String statisticName, IStatisticalSummary statistic, Map<String,String> attributes);
}
