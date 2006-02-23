package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INedComponent {

	public String getDisplayString() {
		return NedElementExUtil.getDisplayString(this); 
	}
	
	public void setDisplayString(String dspString) {
		NedElementExUtil.setDisplayString(this, dspString);
	}
}
