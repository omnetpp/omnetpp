package org.omnetpp.scave2.model;

/**
 * Filter parameters for datasets. 
 */
public class FilterParams {
	private String runName;
	private String moduleName;
	private String dataName;
	
	public FilterParams(String fileName, String moduleName, String dataName) {
		this.runName = fileName;
		this.moduleName = moduleName;
		this.dataName = dataName;
	}
	
	public String getRunNamePattern() {
		return runName != null ? runName : "";
	}
	
	public void setRunNamePattern(String runName) {
		this.runName = runName;
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
}
