package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned.editor.graph.properties.DisplayPropertySource;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;

public class CompoundModuleNodeEx extends CompoundModuleNode 
								  implements INedContainer, INedNode {

	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	
	public CompoundModuleNodeEx() {
	}

	public CompoundModuleNodeEx(NEDElement parent) {
		super(parent);
	}

	public String getDisplayString() {
		return NedElementExUtil.getDisplayString(this); 
	}
	
	public void setDisplayString(String dspString) {
		NedElementExUtil.setDisplayString(this, dspString);
	}

	public List<SubmoduleNodeEx> getSubmodules() {
		List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submodulesNode == null)
			return result;
		for(NEDElement currChild : submodulesNode) 
			if (currChild instanceof SubmoduleNodeEx) 
				result.add((SubmoduleNodeEx)currChild);
				
		return result;
	}

	public SubmoduleNodeEx getSubmoduleByName(String submoduleName) {
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submoduleName == null)
			return null;
		return (SubmoduleNodeEx)submodulesNode
					.getFirstChildWithAttribute(NED_SUBMODULE, SubmoduleNode.ATT_NAME, submoduleName);
	}

	public List<ConnectionNodeEx> getConnections() {
		List<ConnectionNodeEx> result = new ArrayList<ConnectionNodeEx>();
		ConnectionsNode connectionsNode = getFirstConnectionsChild();
		if (connectionsNode == null)
			return result;
		for(NEDElement currChild : connectionsNode) 
			if (currChild instanceof ConnectionNodeEx) 
				result.add((ConnectionNodeEx)currChild);
				
		return result;
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

	public void addModelChild(INedNode child) {
		getFirstSubmodulesChild().appendChild((NEDElement)child);
	}

	public void removeModelChild(INedNode child) {		
		getFirstSubmodulesChild().removeChild((NEDElement)child);
	}

	public void insertModelChild(int index, INedNode child) {
		// FIXME check wheter Submodules node exists
		NEDElement insertBefore = getFirstSubmodulesChild().getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		getFirstSubmodulesChild().insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertModelChild(INedNode insertBefore, INedNode child) {
		getFirstSubmodulesChild().insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

	public List<? extends INedNode> getModelChildren() {
		return getSubmodules();
	}

	public Point getLocation() {
		String dispstring = getDisplayString();
		DisplayPropertySource dps = new DisplayPropertySource(dispstring);

		return new Point (dps.getIntPropertyDef(DisplayPropertySource.PROP_X, 0),
						  dps.getIntPropertyDef(DisplayPropertySource.PROP_Y, 0));
	}

	public void setLocation(Point location) {
		String dispstring = getDisplayString();
		DisplayPropertySource dps = new DisplayPropertySource(dispstring);

		dps.setPropertyValue(DisplayPropertySource.PROP_X, location.x);
		dps.setPropertyValue(DisplayPropertySource.PROP_Y, location.y);
		setDisplayString(dps.getValue());
	}

	public Dimension getSize() {
		String dispstring = getDisplayString();
		DisplayPropertySource dps = new DisplayPropertySource(dispstring);

		return new Dimension(dps.getIntPropertyDef(DisplayPropertySource.PROP_W, 0),
						     dps.getIntPropertyDef(DisplayPropertySource.PROP_H, 0));
	}

	public void setSize(Dimension size) {
		String dispstring = getDisplayString();
		DisplayPropertySource dps = new DisplayPropertySource(dispstring);

		dps.setPropertyValue(DisplayPropertySource.PROP_W, size.width);
		dps.setPropertyValue(DisplayPropertySource.PROP_H, size.height);
		setDisplayString(dps.getValue());
	}
}
