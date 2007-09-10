package org.omnetpp.scave.model2;

import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_DATANAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_FILENAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_MODULENAME;
import static org.omnetpp.scave.model2.ResultItemFields2.FIELD_RUNNAME;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.ResultType;

/**
 * Given an IDList, produces hints for different filter fields such as
 * module name, statistic name, etc.
 *
 * @author tomi
 */
public class FilterHints {
	private static final String[] EMPTY = new String[0];

	private Map<String,String[]> hints = new HashMap<String, String[]>();

	public FilterHints() {
	}

	public FilterHints(ResultFileManager manager, ResultType type) {
		this(manager, ScaveModelUtil.getAllIDs(manager, type));
	}

	public FilterHints(ResultFileManager manager, IDList idlist) {
		ResultFileList fileList = manager.getUniqueFiles(idlist);
		RunList runList = manager.getUniqueRuns(idlist);

		setHints(FIELD_FILENAME, getFileNameFilterHints(fileList));
		setHints(FIELD_RUNNAME, getRunNameFilterHints(runList));
		setHints(FIELD_MODULENAME, manager.getModuleFilterHints(idlist).toArray());
		setHints(FIELD_DATANAME, manager.getNameFilterHints(idlist).toArray());
		for (String attrName : RunAttribute.getNames())
			setHints(attrName, getFilterHintsForRunAttribute(manager, runList, attrName));
	}

	public String[] getHints(String fieldName) {
		if (hints.containsKey(fieldName))
			return hints.get(fieldName);
		else
			return EMPTY;
	}

	public void setHints(String fieldName, String[] value) {
		this.hints.put(fieldName, value);
	}

	private static String[] getFileNameFilterHints(ResultFileList fileList) {
		String[] hints = new String[(int)fileList.size() + 1];
		hints[0] = "*";
		for (int i = 0; i < fileList.size(); ++i)
			hints[i+1] = fileList.get(i).getFilePath();
		return hints;
	}

	private static String[] getRunNameFilterHints(RunList runList) {
		StringVector hints = new StringVector();
		hints.add("*");
		for (int i = 0; i < runList.size(); ++i) {
			String runName = runList.get(i).getRunName();
			if (runName.length() > 0)
				hints.add(runName);
		}
		return hints.toArray();
	}

	private static String[] getFilterHintsForRunAttribute(ResultFileManager manager, RunList runList, String attrName) {
		StringVector values = manager.getUniqueAttributeValues(runList, attrName);
		String[] filterHints = new String[(int)values.size() + 1];
		filterHints[0] = "*";
		for (int i = 0; i < values.size(); ++i)
			filterHints[i+1] = values.get(i);
		return filterHints;
	}
}
