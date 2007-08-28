package org.omnetpp.scave.charting.dataset;

import java.util.Random;

import org.omnetpp.common.engine.BigDecimal;

/**
 * Random IXYDataset for sample data in preview.
 *
 * @author tomi
 */
public class RandomXYDataset implements IXYDataset {

	String[] seriesKeys;
	double[][] yCoords;
	
	public RandomXYDataset(long seed, String[] seriesKeys, int numOfItems) {
		Random rg = new Random(seed);
		this.seriesKeys = seriesKeys;
		yCoords = new double[seriesKeys.length][];
		for (int series = 0; series < seriesKeys.length; ++series) {
			yCoords[series] = new double[numOfItems];
			for (int i = 0; i < numOfItems; ++i) {
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

	public int getItemCount(int series) {
		return yCoords[series].length;
	}

	public double getX(int series, int item) {
		return (item+0.5)/(getItemCount(series));
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
