package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.Run;

/**
 * Instances of this class are used as the payload of GenericTreeNodes
 * representing run attributes in the content model of logical input tree.
 *
 * @author tomi
 */
public class RunAttributePayload {
	
	private String name;
	private String value;
	private String remark;
	
	public RunAttributePayload(String name, String value, String remark) {
		this.name = name;
		this.value = value;
		this.remark = remark;
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
		if (!(other instanceof RunAttributePayload))
			return false;
		
		RunAttributePayload otherAttr = (RunAttributePayload)other;
		
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
		return name +  " \"" + (value != null ? value : "n/a") + "\"" + (remark != null ? " " + remark : "");
	}
	
	public static RunAttributePayload getRunAttribute(Run run, String attrName, String remark) {
		return new RunAttributePayload(attrName, run.getAttribute(attrName), remark);
	}
}
