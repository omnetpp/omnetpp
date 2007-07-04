package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IDVector;
import org.omnetpp.scave.engine.IDVectorVector;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.ScalarResult;

/**
 * IXYDataset implementation for scatter plots.
 *
 * @author tomi
 */
public class ScatterPlotDataset implements IXYDataset {

	private IDVectorVector data; // first vector contains ids of X values,
	                             // other vectors contains ids of Y values or
	                             // -1 if no Y for the X
	private String[] dataNames;  // names of Y values (module+data)
	private ResultFileManager manager;
	
	public ScatterPlotDataset(IDList idlist, String moduleName, String dataName, ResultFileManager manager) {
		ScalarDataSorter sorter = new ScalarDataSorter(manager);
		this.data = sorter.prepareScatterPlot(idlist, moduleName, dataName);
		this.manager = manager;
		computeNames();
	}
	
	public int getSeriesCount() {
		return (int)data.size() - 1;
	}

	public String getSeriesKey(int series) {
		return dataNames[series];
	}

	public int getItemCount(int series) {
		IDVector xIDs = data.get(0);
		return (int)xIDs.size();
	}

	public double getX(int series, int item) {
		IDVector xIDs = data.get(0);
		long id = xIDs.get(item);
		ScalarResult result = manager.getScalar(id);
		return result.getValue();
	}
	
	public BigDecimal getPreciseX(int series, int item) {
		return new BigDecimal(getX(series, item));
	}

	public double getY(int series, int item) {
		IDVector yIDs = data.get(series+1);
		long id = yIDs.get(item);
		if (id != -1) {
			ScalarResult result = manager.getScalar(id);
			return result.getValue();
		}
		else
			return Double.NaN;
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
	
	private void computeNames() {
		dataNames = new String[(int)data.size()-1];
		for (int i=0; i<dataNames.length; ++i) {
			IDVector v = data.get(i+1);
			dataNames[i] = String.valueOf(i);
			for (int j=0; j<v.size(); ++j) {
				long id = v.get(j);
				if (id != -1) {
					ResultItem item = manager.getItem(id);
					dataNames[i] = item.getModuleName() + " " + item.getName();
					break;
				}
			}
		}
	}
}
