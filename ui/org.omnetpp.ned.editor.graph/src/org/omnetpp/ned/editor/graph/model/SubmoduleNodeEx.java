package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INedNode {

	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	
	public SubmoduleNodeEx(NEDElement parent) {
		super(parent);
	}

	public String getDisplayString() {
		return NedElementExUtil.getDisplayString(this); 
	}
	
	public void setDisplayString(String dspString) {
		NedElementExUtil.setDisplayString(this, dspString);
	}

	protected CompoundModuleNodeEx getCompoundModule() {
		return (CompoundModuleNodeEx)(getParent().getParent());
	}

	public List<ConnectionNodeEx> getSrcConnections() {
		return srcConns;
	}
	
	public List<ConnectionNodeEx> getDestConnections() {
		return destConns;
	}

	public void addSrcConnection(ConnectionNodeEx conn) {
		assert(!srcConns.contains(conn));
		srcConns.add(conn);
		// TODO add property change event (connection added)
	}

	public void removeSrcConnection(ConnectionNodeEx conn) {
		assert(srcConns.contains(conn));
		srcConns.remove(conn);
		// TODO add property change event (connection removed)
	}

	public void addDestConnection(ConnectionNodeEx conn) {
		assert(!destConns.contains(conn));
		destConns.add(conn);
		// TODO add property change event (connection added)
	}

	public void removeDestConnection(ConnectionNodeEx conn) {
		assert(destConns.contains(conn));
		destConns.remove(conn);
		// TODO add property change event (connection removed)
	}
}
