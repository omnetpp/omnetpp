package org.omnetpp.scave.charting.dataset;

import java.util.Random;

import org.omnetpp.common.engine.BigDecimal;

/**
 * Random IXYDataset for sample data in preview.
 *
 * @author tomi
 */
public class RandomXYDataset extends XYDatasetSupport implements IXYDataset {

	String[] seriesKeys;
	InterpolationMode[] interpolationModes;
	double[][] xCoords;
	double[][] yCoords;
	
	public RandomXYDataset(long seed, String[] seriesKeys, InterpolationMode[] interpolationModes,int numOfItems) {
		this.seriesKeys = seriesKeys;
		this.interpolationModes = interpolationModes;
		Random rg = new Random(seed);
		xCoords = new double[seriesKeys.length][];
		yCoords = new double[seriesKeys.length][];
		for (int series = 0; series < seriesKeys.length; ++series) {
			xCoords[series] = new double[numOfItems];
			yCoords[series] = new double[numOfItems];
			for (int i = 0; i < numOfItems; ++i) {
				xCoords[series][i] = (i + rg.nextDouble()) / numOfItems;
				yCoords[series][i] = rg.nextDouble();
			}
		}
	}

	public int getSeriesCount() {
		return seriesKeys.length;
	}

	public String getSeriesKey(int series) {
		return seriesKeys[series];
	}
	
	@Override
	public InterpolationMode getSeriesInterpolationMode(int series) {
		return interpolationModes != null && series < interpolationModes.length ?
				interpolationModes[series] : InterpolationMode.Unspecified;
	}

	public int getItemCount(int series) {
		return xCoords[series].length;
	}

	public double getX(int series, int item) {
		return xCoords[series][item];
	}

	public BigDecimal getPreciseX(int series, int item) {
		return null;
	}
	
	public double getY(int series, int item) {
		return yCoords[series][item];
	}
	
	public BigDecimal getPreciseY(int series, int item) {
		return null;
	}
}
