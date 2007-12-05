package org.omnetpp.scave.charting.dataset;

/**
 * Represents an XY dataset where the x,y coordinates are
 * computed by averaging.
 *
 * @author tomi
 */
public interface IAveragedXYDataset extends IXYDataset {

	/**
     * 
     */
    public double getXConfidenceInterval(int series, int item, double p);
    
    /**
     * 
     */
    public double getYConfidenceInterval(int series, int item, double p);
}
