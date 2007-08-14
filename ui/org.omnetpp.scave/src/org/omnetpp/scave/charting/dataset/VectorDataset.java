package org.omnetpp.scave.charting.dataset;

import static org.omnetpp.scave.model2.ResultItemFields.FIELD_DATANAME;
import static org.omnetpp.scave.model2.ResultItemFields.FIELD_FILENAME;
import static org.omnetpp.scave.model2.ResultItemFields.FIELD_MODULENAME;
import static org.omnetpp.scave.model2.ResultItemFields.FIELD_RUNNAME;
import static org.omnetpp.scave.model2.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.model2.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.model2.RunAttribute.REPLICATION;
import static org.omnetpp.scave.model2.RunAttribute.RUNNUMBER;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model2.ResultItemFields;
import org.omnetpp.scave.model2.ResultItemFormatter;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * IXYDataset implementation for output vectors.
 *
 * @author tomi
 */
public class VectorDataset implements IXYDataset {
	
	private IDList idlist;
	private String[] seriesKeys;
	private XYArray[] seriesData;

	/**
	 * Creates an empty dataset.
	 */
	public VectorDataset() {
		this.idlist = new IDList();
		this.seriesKeys = new String[] {};
		this.seriesData = new XYArray[] {};
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
		ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
		String format = StringUtils.isEmpty(lineNameFormat) ? getLineNameFormat(items) : lineNameFormat;
		this.seriesKeys = ResultItemFormatter.formatResultItems(format, items);
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

	public double getY(int series, int item) {
		return seriesData[series].getY(item);
	}

	public BigDecimal getPreciseY(int series, int item) {
		return new BigDecimal(getY(series, item));
	}
	
	public long getID(int series) {
		return idlist.get(series);
	}
	
	/**
	 * Returns the default format string for names of the lines in {@code items}.
	 * It is "{file} {run} {run-number} {module} {name} {experiment} {measurement} {replication}",
	 * but fields that are the same for each item are omitted.
	 * If all the fields has the same value in {@code items}, then the "{index}" is used as 
	 * the format string. 
	 */
	private String getLineNameFormat(ResultItem[] items) {
		if (idlist.isEmpty())
			return "";
		
		StringBuffer sbFormat = new StringBuffer();
		char separator = ' ';
		String[] fields = new String[] {FIELD_FILENAME, FIELD_RUNNAME, RUNNUMBER, FIELD_MODULENAME, FIELD_DATANAME,
										EXPERIMENT, MEASUREMENT, REPLICATION};
		for (String field : fields) {
			String firstValue = ResultItemFields.getFieldValue(items[0], field);
			
			for (int i = 1; i < items.length; ++i) {
				String value = ResultItemFields.getFieldValue(items[i], field);
				if (!ObjectUtils.equals(firstValue, value)) {
					sbFormat.append('{').append(field).append('}').append(separator);
					break;
				}
			}
		}
		
		if (sbFormat.length() > 0)
			sbFormat.deleteCharAt(sbFormat.length() - 1);
		else
			sbFormat.append("{index}");
		
		return sbFormat.toString();
	}
}
