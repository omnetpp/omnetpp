package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;

public class CompoundXYDataset extends XYDatasetSupport implements IAveragedXYDataset {

	private IXYDataset[] seriesToDatasetMap;
	private int[] seriesToOffsetMap;
	
	public CompoundXYDataset(IXYDataset... datasets) {
		int seriesCount = 0;
		for (int i = 0; i < datasets.length; ++i)
			seriesCount += datasets[i].getSeriesCount();
		
		seriesToDatasetMap = new IXYDataset[seriesCount];
		seriesToOffsetMap = new int[seriesCount];
		int offset = 0;
		for (IXYDataset dataset : datasets) {
			int count = dataset.getSeriesCount();
			for (int series = 0; series < count; ++series) {
				seriesToDatasetMap[offset + series] = dataset;
				seriesToOffsetMap[offset + series] = offset;
			}
			offset += count;
		}
	}

	public String getTitle() {
		return null;
	}

	public int getSeriesCount() {
		return seriesToDatasetMap.length;
	}

	public String getSeriesKey(int series) {
		return seriesToDatasetMap[series].getSeriesKey(series - seriesToOffsetMap[series]);
	}

	public int getItemCount(int series) {
		return seriesToDatasetMap[series].getItemCount(series - seriesToOffsetMap[series]);
	}

	public double getX(int series, int item) {
		return seriesToDatasetMap[series].getX(series - seriesToOffsetMap[series], item);
	}

	public BigDecimal getPreciseX(int series, int item) {
		return seriesToDatasetMap[series].getPreciseX(series - seriesToOffsetMap[series], item);
	}

	public double getXConfidenceInterval(int series, int item, double p) {
		IXYDataset dataset = seriesToDatasetMap[series];
		return dataset instanceof IAveragedXYDataset ?
				((IAveragedXYDataset)dataset).getXConfidenceInterval(series - seriesToOffsetMap[series], item, p) :
				Double.NaN;
	}

	public double getY(int series, int item) {
		return seriesToDatasetMap[series].getY(series - seriesToOffsetMap[series], item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return seriesToDatasetMap[series].getPreciseY(series - seriesToOffsetMap[series], item);
	}

	public double getYConfidenceInterval(int series, int item, double p) {
		IXYDataset dataset = seriesToDatasetMap[series];
		return dataset instanceof IAveragedXYDataset ?
				((IAveragedXYDataset)dataset).getYConfidenceInterval(series - seriesToOffsetMap[series], item, p) :
				Double.NaN;
	}
}
