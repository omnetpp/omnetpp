package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model.Chart;

/**
 * Data class to identify a line on a chart.
 * If the line represents the content of a vector,
 * the vector is also accessible from this class.
 *
 * @author tomi
 */
public class LineID {
	// the chart containing the line
	private Chart chart;
	// the key of the line within the chart
	private String key;
	// the id of the result item that the line represents
	// -1 for computed data
	private long id;
	// the ResultFileManager that loaded the result item, and can resolve the id
	private ResultFileManager manager;
	
	public LineID(Chart chart, String key, long id, ResultFileManager manager) {
		this.chart = chart;
		this.key = key;
		this.id = id;
		this.manager = manager;
	}
	
	public Chart getChart() {
		return chart;
	}
	
	public String getKey() {
		return key;
	}
	
	public long getResultItemID() {
		return id;
	}
	
	public ResultFileManager getResultFileManager() {
		return manager;
	}
	
	public ResultItem getResultItem() {
		return id != -1 && manager != null ? manager.getItem(id) : null;
	}
}
