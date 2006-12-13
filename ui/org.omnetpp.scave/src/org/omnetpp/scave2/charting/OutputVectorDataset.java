package org.omnetpp.scave2.charting;

import org.eclipse.core.runtime.Assert;
import org.jfree.data.DomainOrder;
import org.jfree.data.general.AbstractSeriesDataset;
import org.jfree.data.xy.XYDataset;

import org.omnetpp.scave.engine.XYArray;

public class OutputVectorDataset extends AbstractSeriesDataset implements XYDataset {

	private static final long serialVersionUID = -3852560945270511274L;

	private String[] seriesKeys;
	private XYArray[] seriesData;

	public OutputVectorDataset(String[] seriesKeys, XYArray[] seriesData) {
		Assert.isTrue(seriesKeys.length == seriesData.length);
		this.seriesKeys = seriesKeys;
		this.seriesData = seriesData;
	}

	public DomainOrder getDomainOrder() {
        return DomainOrder.ASCENDING;
	}

	public int getItemCount(int series) {
		return seriesData[series].length();
	}

	public Number getX(int series, int item) {
		return new Double(seriesData[series].getX(item));
	}

	public double getXValue(int series, int item) {
		return seriesData[series].getX(item);
	}

	public Number getY(int series, int item) {
		return new Double(seriesData[series].getY(item));
	}

	public double getYValue(int series, int item) {
		return seriesData[series].getY(item);
	}

	public int getSeriesCount() {
		return seriesData.length;
	}

	public Comparable getSeriesKey(int series) {
		return seriesKeys[series];
	}
}
