package org.omnetpp.scave2.model2;

import org.omnetpp.scave.model.SetOperation;

/**
 * Filter parameters for datasets. 
 */
public class FilterParams {
	private String fileName;
	private String runName;
	private String experimentName;
	private String measurementName;
	private String replicationName;
	private String moduleName;
	private String dataName;
	
	public FilterParams() {
	}
	
	public FilterParams(String fileName, String runName,
			String experimentName, String measurementName, String replicationName,
			String moduleName, String dataName) {
		this.fileName = fileName;
		this.runName = runName;
		this.experimentName = experimentName;
		this.measurementName = measurementName;
		this.replicationName = replicationName;
		this.moduleName = moduleName;
		this.dataName = dataName;
	}
	
	public FilterParams(SetOperation op) {
		this.fileName = op.getFilenamePattern();
		this.runName = op.getRunNamePattern();
		this.experimentName = op.getExperimentNamePattern();
		this.measurementName = op.getMeasurementNamePattern();
		this.replicationName = op.getReplicationNamePattern();
		this.moduleName = op.getModuleNamePattern();
		this.dataName = op.getNamePattern();
	}
	
	public String getFileNamePattern() {
		return fileName != null ? fileName : "";
	}

	public void setFileNamePattern(String fileName) {
		this.fileName = fileName;
	}

	public String getRunNamePattern() {
		return runName != null ? runName : "";
	}
	
	public void setRunNamePattern(String runName) {
		this.runName = runName;
	}
	
	public String getExperimentNamePattern() {
		return experimentName != null ? experimentName : "";
	}

	public void setExperimentNamePattern(String experimentName) {
		this.experimentName = experimentName;
	}

	public String getMeasurementNamePattern() {
		return measurementName != null ? measurementName : "";
	}

	public void setMeasurementNamePattern(String measurementName) {
		this.measurementName = measurementName;
	}

	public String getReplicationNamePattern() {
		return replicationName != null ? replicationName : "";
	}

	public void setReplicationNamePattern(String replicationName) {
		this.replicationName = replicationName;
	}

	public String getModuleNamePattern() {
		return moduleName != null ? moduleName : "";
	}
	
	public void setModuleNamePattern(String moduleName) {
		this.moduleName = moduleName;
	}

	public String getDataNamePattern() {
		return dataName != null ? dataName : "";
	}

	public void setDataNamePattern(String dataName) {
		this.dataName = dataName;
	}
	
	public String toString() {
		StringBuffer sb = new StringBuffer();
		if (getModuleNamePattern().length() > 0) sb.append("module=").append(getModuleNamePattern()).append(" ");
		if (getDataNamePattern().length() > 0) sb.append("name=").append(getDataNamePattern()).append(" ");
		if (getFileNamePattern().length() > 0) sb.append("file=").append(getFileNamePattern()).append(" ");
		if (getRunNamePattern().length() > 0) sb.append("run=").append(getRunNamePattern()).append(" ");
		if (getExperimentNamePattern().length() > 0) sb.append("experiment=").append(getExperimentNamePattern()).append(" ");
		if (getMeasurementNamePattern().length() > 0) sb.append("measurement=").append(getMeasurementNamePattern()).append(" ");
		if (getReplicationNamePattern().length() > 0) sb.append("replication=").append(getReplicationNamePattern()).append(" ");
		if (sb.length() > 0)
			sb.deleteCharAt(sb.length() - 1);
		else
			sb.append("<all>");
		return sb.toString();
	}
}
