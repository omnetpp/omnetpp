package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.XYDataset;

import static org.omnetpp.scave.engine.ScalarFields.*;

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
public class ScatterPlotDataset implements IXYDataset {
	
	private XYDataset data; // first row contains X values,
	                        // other rows contain Y values (NaN if missing)
	private String[] keys;
	
	public ScatterPlotDataset(IDList idlist, String moduleName, String scalarName, boolean averageReplications, ResultFileManager manager) {
		ScalarDataSorter sorter = new ScalarDataSorter(manager);
		ScalarFields rowFields = new ScalarFields(ScalarFields.MODULE | ScalarFields.NAME);
		ScalarFields columnFields = averageReplications ? new ScalarFields(EXPERIMENT | MEASUREMENT) :
			                                              new ScalarFields(EXPERIMENT | MEASUREMENT | REPLICATION);
		this.data = sorter.prepareScatterPlot2(idlist, moduleName, scalarName, rowFields, columnFields);
		this.keys = computeKeys(this.data);
	}
	
	public int getSeriesCount() {
		return data.getRowCount() - 1;
	}

	public String getSeriesKey(int series) {
		return keys[series];
	}

	public int getItemCount(int series) {
		return data.getColumnCount();
	}

	public double getX(int series, int item) {
		return data.getValue(0, item);
	}
	
	public BigDecimal getPreciseX(int series, int item) {
		return new BigDecimal(getX(series, item));
	}

	public double getY(int series, int item) {
		return data.getValue(series+1, item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
	
	private static String[] computeKeys(XYDataset data) {
		// first row contains the common X coordinates
		// name the lines after their Y data
		String[] keys = new String[data.getRowCount()-1];
		for (int i=0; i<keys.length; ++i) {
			keys[i] = data.getRowField(i+1, ScalarFields.MODULE) + " " +
					   data.getRowField(i+1, ScalarFields.NAME);
		}
		return keys;
	}
}
