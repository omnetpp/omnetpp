package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.Run;

public class RunAttribute {
	
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
