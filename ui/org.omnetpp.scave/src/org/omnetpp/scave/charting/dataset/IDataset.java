package org.omnetpp.scave.charting.dataset;

/**
 * Common interface for chart datasets.
 *
 * @author tomi
 */
public interface IDataset {
	
	/**
	 * The short description of the dataset displayed as the 
	 * default title of the chart.
	 * 
	 * @return the title of the dataset, may be null
	 */
	String getTitle();
}
