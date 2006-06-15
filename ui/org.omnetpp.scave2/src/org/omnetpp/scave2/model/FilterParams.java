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
	
	public String getModuleNamePattern() {
		return moduleName != null ? moduleName : "";
	}
	
	public String getDataNamePattern() {
		return dataName != null ? dataName : "";
	}
}
