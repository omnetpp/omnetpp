package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDVector;
import org.omnetpp.scave.engine.IDVectorVector;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;

/**
 * Dataset for scatter plot charts.
 *
 * @author tomi
 */
public class ScatterPlotDataset implements IXYDataset {

	private IDVectorVector data;
	private String[] dataNames;
	private ResultFileManager manager;
	
	public ScatterPlotDataset(IDVectorVector data, String[] dataNames, ResultFileManager manager) {
		this.data = data;
		this.dataNames = dataNames;
		this.manager = manager;
	}
	
	public int getSeriesCount() {
		return (int)data.size() - 1;
	}

	public Comparable getSeriesKey(int series) {
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
}
