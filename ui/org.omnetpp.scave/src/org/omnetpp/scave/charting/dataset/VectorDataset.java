package org.omnetpp.scave.charting.dataset;

import java.util.Arrays;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model2.RunAttribute;

/**
 * IXYDataset implementation for output vectors.
 *
 * @author tomi
 */
public class VectorDataset implements IXYDataset {
	
	private IDList idlist;
	private String[] seriesKeys;
	private XYArray[] seriesData;

	public VectorDataset() {
		this.idlist = new IDList();
		this.seriesKeys = new String[] {};
		this.seriesData = new XYArray[] {};
	}

	public VectorDataset(IDList idlist, XYArray[] seriesData, ResultFileManager manager) {
		Assert.isLegal(idlist != null);
		Assert.isLegal(manager != null);
		Assert.isTrue(seriesData == null || idlist.size() == seriesData.length);
		this.idlist = idlist;
		this.seriesData = seriesData;
		this.seriesKeys = getResultItemIDs(idlist, manager);
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
	 * Returns the ids of data items in the <code>idlist</code>.
	 * The id is formed from the file name, run number, run id, module name,
	 * data name, experiment, measurement and replication.
	 * Constant fields will be omitted from the id.
	 */
	public String[] getResultItemIDs(IDList idlist, ResultFileManager manager) {
		String[][] nameFragments = new String[(int)idlist.size()][];
		for (int i = 0; i < idlist.size(); ++i) {
			ResultItem item = manager.getItem(idlist.get(i));
			Run run = item.getFileRun().getRun();
			nameFragments[i] = new String[8];
			nameFragments[i][0] = item.getFileRun().getFile().getFilePath();
			nameFragments[i][1] = String.valueOf(run.getRunNumber());
			nameFragments[i][2] = String.valueOf(run.getRunName());
			nameFragments[i][3] = item.getModuleName();
			nameFragments[i][4] = item.getName();
			nameFragments[i][5] = run.getAttribute(RunAttribute.EXPERIMENT);
			nameFragments[i][6] = run.getAttribute(RunAttribute.REPLICATION);
			nameFragments[i][7] = run.getAttribute(RunAttribute.MEASUREMENT);
		}

		boolean[] same = new boolean[8];
		Arrays.fill(same, true);
		for (int i = 1; i < nameFragments.length; ++i) {
			for (int j = 0; j < 8; ++j)
				if (same[j] && !ObjectUtils.equals(nameFragments[0][j], nameFragments[i][j]))
					same[j] = false;
		}

		String[] result = new String[nameFragments.length];
		for (int i = 0; i < result.length; ++i) {
			StringBuffer id = new StringBuffer(30);
			for (int j = 0; j < 8; ++j)
				if (!same[j])
					id.append(nameFragments[i][j]).append(" ");
			if (id.length() == 0)
				id.append(i);
			else
				id.deleteCharAt(id.length() - 1);
			result[i] = id.toString();
		}
		
		Assert.isTrue(idlist.size() == result.length);
		return result;
	}
}
