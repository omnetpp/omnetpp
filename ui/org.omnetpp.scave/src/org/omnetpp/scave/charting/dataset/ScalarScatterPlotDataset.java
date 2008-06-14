package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.XYDataset;

/**
 * IXYDataset implementation for a scatter plot computed from a list of scalars.
 * <p>
 * One of the scalars gives the X coordinates (selected by module/data name),
 * others gives the Y coordinates. There are as many series as many different
 * module/data names except the selected one.
 * <p>
 * Each series contains as many data points as many scalars are with the
 * module/data name, scalars differing only in their replications may optionally
 * be averaged.
 * 
 * @author tomi
 */
public class ScalarScatterPlotDataset extends XYDatasetSupport implements IAveragedXYDataset {
	
	private XYDataset scalars;      // first rows contains X values,
	                        		// other rows contain Y values (NaN if missing)
	private String[] keys;
	
	public ScalarScatterPlotDataset(XYDataset scalars) {
		this.scalars = scalars;
		this.keys = computeKeys(this.scalars);
	}
		
	public String getTitle(String format) {
		// TODO Auto-generated method stub
		return null;
	}

	public int getSeriesCount() {
		return keys.length;
	}

	public String getSeriesKey(int series) {
		return keys[series];
	}

	public int getItemCount(int series) {
		return scalars.getColumnCount();
	}

	public double getX(int series, int item) {
		return scalars.getValue(0, item).getMean();
	}
	
	public BigDecimal getPreciseX(int series, int item) {
		return new BigDecimal(getX(series, item));
	}

	public Statistics getXStatistics(int series, int item) {
		return scalars.getValue(0, item);
	}

	public double getY(int series, int item) {
		return scalars.getValue(series+1, item).getMean();
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}

	public Statistics getYStatistics(int series, int item) {
		return scalars.getValue(series+1, item);
	}
	
	private static final ResultItemField MODULE = new ResultItemField(ResultItemField.MODULE);
	private static final ResultItemField NAME = new ResultItemField(ResultItemField.NAME);
	
	private static String[] computeKeys(XYDataset data) {
		// first row contains the common X coordinates
		// name the lines after their Y data
		String[] keys = new String[data.getRowCount()-1];
		for (int i=0; i<keys.length; ++i) {
			keys[i] = data.getRowField(i+1, MODULE) + " " +
					   data.getRowField(i+1, NAME);
		}
		return keys;
	}
}
