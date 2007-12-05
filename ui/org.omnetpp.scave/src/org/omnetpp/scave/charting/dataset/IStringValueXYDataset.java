package org.omnetpp.scave.charting.dataset;

/**
 * Implement this interface if custom formatting of the 
 * values is needed (e.g. enum values).
 *
 * @author tomi
 */
public interface IStringValueXYDataset extends IXYDataset {
	
	public String getXAsString(int series, int item);

	public String getYAsString(int series, int item);
}
