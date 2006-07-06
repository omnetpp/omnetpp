package org.omnetpp.scave2.model;

public class FilterHints {
	private static final String[] EMPTY = new String[0];
	
	private String[] fileNameHints;
	private String[] runNameHints;
	private String[] experimentNameHints;
	private String[] measurementNameHints;
	private String[] replicationNameHints;
	private String[] moduleNameHints;
	private String[] dataNameHints;

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
