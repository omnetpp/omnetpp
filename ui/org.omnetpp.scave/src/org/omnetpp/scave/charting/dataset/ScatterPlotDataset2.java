package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.XYDataset;

import static org.omnetpp.scave.engine.ScalarFields.*;

public class ScatterPlotDataset2 implements IXYDataset {
	
	private XYDataset data; // first row contains X values,
	                        // other rows contain Y values (NaN if missing)
	private String[] dataNames;
	
	public ScatterPlotDataset2(IDList idlist, String moduleName, String scalarName, boolean averageReplications, ResultFileManager manager) {
		ScalarDataSorter sorter = new ScalarDataSorter(manager);
		ScalarFields rowFields = new ScalarFields(ScalarFields.MODULE | ScalarFields.NAME);
		ScalarFields columnFields = averageReplications ? new ScalarFields(EXPERIMENT | MEASUREMENT) :
			                                              new ScalarFields(EXPERIMENT | MEASUREMENT | REPLICATION);
		this.data = sorter.prepareScatterPlot2(idlist, moduleName, scalarName, rowFields, columnFields);
		computeNames();
	}
	
	public int getSeriesCount() {
		return data.getRowCount() - 1;
	}

	public String getSeriesKey(int series) {
		return dataNames[series];
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
	
	private void computeNames() {
		// first row contains the common X coordinates
		// name the lines after their Y data
		dataNames = new String[data.getRowCount()-1];
		for (int i=0; i<dataNames.length; ++i) {
			dataNames[i] = data.getRowField(i+1, ScalarFields.MODULE) + " " +
					   data.getRowField(i+1, ScalarFields.NAME);
		}
	}
}
