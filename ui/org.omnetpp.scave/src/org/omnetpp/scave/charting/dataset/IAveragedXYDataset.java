package org.omnetpp.scave.charting.dataset;

/**
 * 
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
