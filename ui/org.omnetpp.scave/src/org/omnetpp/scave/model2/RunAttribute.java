package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.Run;

public class RunAttribute {
	
	// Note: these field names must be kept consistent with resultfilemanager.cc
	// XXX and with FilterUtil
	// XXX or: merge all these into one place? 
	public static final String EXPERIMENT = "experiment";
	public static final String MEASUREMENT = "measurement";
	public static final String REPLICATION = "replication";
	public static final String RUNNUMBER = "run-number";
	public static final String NETWORKNAME = "network";
	public static final String DATETIME = "dateTime";
	
	private static final String[] ALL = new String[] {
		EXPERIMENT, MEASUREMENT, REPLICATION, RUNNUMBER, NETWORKNAME, DATETIME,
	};

	private String name;
	private String value;
	
	public RunAttribute(String name, String value) {
		this.name = name;
		this.value = value;
	}
	
	public static String[] getNames() {
		return ALL;
	}
	
	public String getName() {
		return name;
	}
	
	public String getValue() {
		return value;
	}
	
	public boolean equals(Object other) {
		if (this == other)
			return true;
		if (!(other instanceof RunAttribute))
			return false;
		
		RunAttribute otherAttr = (RunAttribute)other;
		
		return this.name.equals(otherAttr.name) &&
				(this.value == null && otherAttr.value == null ||
				 this.value != null && this.value.equals(otherAttr.value));
	}
	
	
	@Override
	public int hashCode() {
		return name.hashCode() ^ (value != null ? value.hashCode() : 0);
	}

	@Override
	public String toString() {
		return name +  " \"" + (value != null ? value : "n/a") + "\"";
	}
	
	public static RunAttribute getRunAttribute(Run run, String attrName) {
		return new RunAttribute(attrName, run.getAttribute(attrName));
	}
}
