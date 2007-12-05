package org.omnetpp.scave.charting.dataset;

/**
 * Implement this interface if custom formatting of the 
 * values is needed (e.g. enum values).
 *
 * @author tomi
 */
public interface IStringValueScalarDataset extends IScalarDataset {
	String getValueAsString(int row, int column);
}
