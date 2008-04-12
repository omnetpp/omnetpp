package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model2.DatasetManager;

public class HistogramDataset implements IHistogramDataset {
	
	private class HistogramData {
		String key;
		boolean isDiscrete;
		double[] cellBreaks;
		double[] cellValues;

		public HistogramData(String key, boolean isDiscrete, double[] cellBreaks, double[] cellValues) {
			this.key = key;
			this.isDiscrete = isDiscrete;
			this.cellBreaks = cellBreaks;
			this.cellValues = cellValues;
		}
	}
	
	private HistogramData[] histograms;
	
	public HistogramDataset(IDList idlist, ResultFileManager manager) {
		String[] keys = DatasetManager.getResultItemNames(idlist, null, manager);
		Assert.isTrue(keys.length == idlist.size());
		histograms = new HistogramData[idlist.size()];
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			HistogramResult histogram = manager.getHistogram(id);
			boolean isDiscrete = "1".equals(histogram.getAttribute("isDiscrete"));
			DoubleVector bins = histogram.getBins();
			int size = (int)bins.size();
			double[] cellBreaks = new double[size+1];
			System.arraycopy(bins.toArray(), 0, cellBreaks, 0, size);
			cellBreaks[size] = Double.POSITIVE_INFINITY;
			double[] cellValues = histogram.getValues().toArray();
			histograms[i] = new HistogramData(keys[i], isDiscrete, cellBreaks, cellValues);
		}
	}

	public int getSeriesCount() {
		return histograms.length;
	}

	public String getSeriesKey(int series) {
		return histograms[series].key;
	}
	
	public boolean isDiscrete(int series) {
		return histograms[series].isDiscrete;
	}

	public int getCellsCount(int series) {
		return histograms[series].cellValues.length;
	}

	public double getCellMin(int series, int index) {
		return histograms[series].cellBreaks[index];
	}

	public double getCellMax(int series, int index) {
		return histograms[series].cellBreaks[index+1];
	}

	public double getCellValue(int series, int index) {
		return histograms[series].cellValues[index];
	}
}
