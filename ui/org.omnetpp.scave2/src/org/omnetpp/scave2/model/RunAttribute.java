package org.omnetpp.scave2.model;

import org.omnetpp.scave.engine.Run;

public class RunAttribute {
	
	public static final String EXPERIMENT = "experiment";
	public static final String MEASUREMENT = "measurement";
	public static final String REPLICATION = "replication";
	
	private String name;
	private String value;
	
	public RunAttribute(String name, String value) {
		this.name = name;
		this.value = value;
	}
	
	public String getName() {
		return name;
	}
	
	public String getValue() {
		return value;
	}
	
	public String toString() {
		return name +  "\"" + (value != null ? value : "n/a") + "\"";
	}
	
	public static RunAttribute getRunAttribute(Run run, String attrName) {
		return new RunAttribute(attrName, run.getAttribute(attrName));
	}
}
