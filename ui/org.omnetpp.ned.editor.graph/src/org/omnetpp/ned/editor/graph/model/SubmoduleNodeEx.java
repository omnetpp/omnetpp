package org.omnetpp.ned.editor.graph.model;

import java.util.List;

import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INedComponent {

	public String getDisplayString() {
		return NedElementExUtil.getDisplayString(this); 
	}
	
	public void setDisplayString(String dspString) {
		NedElementExUtil.setDisplayString(this, dspString);
	}

	protected CompoundModuleNodeEx getCompoundModule() {
		return (CompoundModuleNodeEx)(getParent().getParent());
	}

	public List<ConnectionNodeEx> getSourceConnections() {
		ConnectionsNode conns = getCompoundModule().getFirstConnectionsChild();
		return NedElementExUtil.getSourceConnections(conns, getName());
	}

	public List<ConnectionNodeEx> getTargetConnections() {
		ConnectionsNode conns = getCompoundModule().getFirstConnectionsChild();
		return NedElementExUtil.getTargetConnections(conns, getName());
	}
}
