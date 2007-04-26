package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.XYArray;

/**
 * IXYDataset implementation for output vectors.
 *
 * @author tomi
 */
public class VectorDataset implements IXYDataset {
	
	private String[] seriesKeys;
	private XYArray[] seriesData;

	public VectorDataset() {
		this.seriesKeys = new String[] {};
		this.seriesData = new XYArray[] {};
	}

	public VectorDataset(String[] seriesKeys, XYArray[] seriesData) {
		Assert.isTrue(seriesKeys.length == seriesData.length);
		this.seriesKeys = seriesKeys;
		this.seriesData = seriesData;
	}
	
	public int getSeriesCount() {
		return seriesData.length;
	}

	public String getSeriesKey(int series) {
		return seriesKeys[series];
	}

	public int getItemCount(int series) {
		return seriesData[series].length();
	}

	public double getX(int series, int item) {
		return seriesData[series].getX(item);
	}
	
	public BigDecimal getPreciseX(int series, int item) {
		return seriesData[series].getPreciseX(item);
	}

	public double getY(int series, int item) {
		return seriesData[series].getY(item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
}
