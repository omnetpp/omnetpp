package org.omnetpp.scave.model2;

import static org.omnetpp.scave.model2.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.model2.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.model2.RunAttribute.REPLICATION;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.DatasetType;

public class FilterHints {
	private static final String[] EMPTY = new String[0];
	
	private String[] fileNameHints;
	private String[] runNameHints;
	private String[] experimentNameHints;
	private String[] measurementNameHints;
	private String[] replicationNameHints;
	private String[] moduleNameHints;
	private String[] dataNameHints;
	
	public FilterHints() {
	}
	
	public FilterHints(ResultFileManager manager, DatasetType type) {
		this(manager, ScaveModelUtil.getAllIDs(manager, type));
	}
	
	public FilterHints(ResultFileManager manager, IDList idlist) {
		ResultFileList fileList = manager.getUniqueFiles(idlist);
		RunList runList = manager.getUniqueRuns(idlist);
		
		setFileNameHints(getFileNameFilterHints(fileList));
		setRunNameHints(getRunNameFilterHints(runList));
		setModuleNameHints(manager.getModuleFilterHints(idlist).toArray());
		setDataNameHints(manager.getNameFilterHints(idlist).toArray());
		setExperimentNameHints(getFilterHintsForRunAttribute(manager, runList, EXPERIMENT));
		setMeasurementNameHints(getFilterHintsForRunAttribute(manager, runList, MEASUREMENT));
		setReplicationNameHints(getFilterHintsForRunAttribute(manager, runList, REPLICATION));
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
	

	public String[] getFileNameHints() {
		return fileNameHints != null ? fileNameHints : EMPTY;
	}
	public String[] getRunNameHints() {
		return runNameHints != null ? runNameHints : EMPTY;
	}
	public String[] getExperimentNameHints() {
		return experimentNameHints != null ? experimentNameHints : EMPTY;
	}
	public String[] getMeasurementNameHints() {
		return measurementNameHints != null ? measurementNameHints : EMPTY;
	}
	public String[] getReplicationNameHints() {
		return replicationNameHints != null ? replicationNameHints : EMPTY;
	}
	public String[] getModuleNameHints() {
		return moduleNameHints != null ? moduleNameHints : EMPTY;
	}
	public String[] getDataNameHints() {
		return dataNameHints != null ? dataNameHints : EMPTY;
	}
	
	public void setDataNameHints(String[] dataNameHints) {
		this.dataNameHints = dataNameHints;
	}
	public void setExperimentNameHints(String[] experimentNameHints) {
		this.experimentNameHints = experimentNameHints;
	}
	public void setFileNameHints(String[] fileNameHints) {
		this.fileNameHints = fileNameHints;
	}
	public void setMeasurementNameHints(String[] measurementNameHints) {
		this.measurementNameHints = measurementNameHints;
	}
	public void setModuleNameHints(String[] moduleNameHints) {
		this.moduleNameHints = moduleNameHints;
	}
	public void setReplicationNameHints(String[] replicationNameHints) {
		this.replicationNameHints = replicationNameHints;
	}
	public void setRunNameHints(String[] runNameHints) {
		this.runNameHints = runNameHints;
	}
}
