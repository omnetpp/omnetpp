package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model2.DatasetManager;

public class VectorScatterPlotDataset extends XYDatasetSupport implements IXYDataset {

	private XYArray[] vectors;  // each array contains X,Y values
	private String[] keys;

	public VectorScatterPlotDataset(IDList vectors, XYArray[] vectorsData, ResultFileManager manager) {
		this.vectors = vectorsData;
		this.keys = DatasetManager.getResultItemNames(vectors, null /*nameFormat*/, manager); // XXX nameFormat
	}

	public int getSeriesCount() {
		return keys.length;
	}

	public String getSeriesKey(int series) {
		return keys[series];
	}

	public int getItemCount(int series) {
		return vectors[series].length();
	}

	public double getX(int series, int item) {
		return vectors[series].getX(item);
	}

	public BigDecimal getPreciseX(int series, int item) {
		return new BigDecimal(getX(series, item));
	}

	public double getY(int series, int item) {
		return vectors[series].getY(item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
}
