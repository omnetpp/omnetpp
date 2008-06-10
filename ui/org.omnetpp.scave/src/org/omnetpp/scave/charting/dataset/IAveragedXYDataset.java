package org.omnetpp.scave.charting.dataset;

import org.omnetpp.scave.engine.Statistics;

/**
 * Represents an XY dataset where the x,y coordinates are
 * collected by grouping individual points (typically by replications).
 *
 * @author tomi
 */
public interface IAveragedXYDataset extends IXYDataset {

	/**
     * Returns the statistics of collected X data.
     */
    public Statistics getXStatistics(int series, int item);
    
    /**
     * Returns the statistics of collected Y data.
     */
    public Statistics getYStatistics(int series, int item);
}
