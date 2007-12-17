package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ResultItemValueFormatter;

/**
 * IXYDataset implementation for output vectors.
 *
 * @author tomi
 */
// TODO bounds should be read from the vector files
public class VectorDataset extends XYDatasetSupport implements IStringValueXYDataset {
	
	private IDList idlist;
	private String[] seriesKeys;
	private XYArray[] seriesData;
	private ResultItemValueFormatter[] formatters; 

	/**
	 * Creates an empty dataset.
	 */
	public VectorDataset() {
		this.idlist = new IDList();
		this.seriesKeys = new String[] {};
		this.seriesData = new XYArray[] {};
		this.formatters = new ResultItemValueFormatter[] {};
	}
	
	/**
	 * Creates a dataset containing vectors from {@code idlist}.
	 * The data of the vectors are not loaded/computed, so
	 * each series will have 0 items.
	 * Intended for accessing the series keys only
	 * (property sheet, edit dialog).
	 */
	public VectorDataset(IDList idlist, String lineNameFormat, ResultFileManager manager) {
		Assert.isLegal(idlist != null);
		Assert.isLegal(manager != null);
		this.idlist = idlist;
		this.seriesKeys = DatasetManager.getResultItemNames(idlist, lineNameFormat, manager);
		Assert.isTrue(idlist.size() == seriesKeys.length);
	}

	/**
	 * Creates a dataset containing vectors from {@code idlist}.
	 * The data of the vectors are loaded/computed.
	 * Intended for displaying the dataset (e.g. charts).
	 */
	public VectorDataset(IDList idlist, XYArray[] seriesData, String lineNameFormat, ResultFileManager manager) {
		this(idlist, lineNameFormat, manager);
		Assert.isTrue(seriesData != null || idlist.size() == seriesData.length);
		this.seriesData = seriesData;
		this.formatters = new ResultItemValueFormatter[idlist.size()];
		for (int i = 0; i < formatters.length; ++i) {
			ResultItem vector = manager.getItem(idlist.get(i));
			formatters[i] = new ResultItemValueFormatter(vector);
		}
	}
	
	public int getSeriesCount() {
		return seriesKeys.length;
	}

	public String getSeriesKey(int series) {
		return seriesKeys[series];
	}

	public int getItemCount(int series) {
		return seriesData != null ? seriesData[series].length() : 0;
	}

	public double getX(int series, int item) {
		return seriesData[series].getX(item);
	}
	
	public BigDecimal getPreciseX(int series, int item) {
		return seriesData[series].getPreciseX(item);
	}
	
	public String getXAsString(int series, int item) {
		BigDecimal xp = getPreciseX(series, item);
		return xp != null ? xp.toString() : String.format("%g", getX(series, item)); 
	}
	
	public double getY(int series, int item) {
		return seriesData[series].getY(item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
	
	public String getYAsString(int series, int item) {
		ResultItemValueFormatter formatter = formatters[series];
		return formatter.format(getY(series, item));
	}
	
	public long getID(int series) {
		return idlist.get(series);
	}
}
