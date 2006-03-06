package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned.editor.graph.properties.DisplayPropertySource;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INedNode {

	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	
	private transient NEDChangeListenerList listeners = new NEDChangeListenerList();

	public SubmoduleNodeEx() {
	}

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

	public Point getLocation() {
		DisplayPropertySource dps = new DisplayPropertySource(getDisplayString());

		return new Point (dps.getIntPropertyDef(DisplayPropertySource.PROP_X, 0),
						  dps.getIntPropertyDef(DisplayPropertySource.PROP_Y, 0));
	}

	public void setLocation(Point location) {
		DisplayPropertySource dps = new DisplayPropertySource(getDisplayString());
		dps.setPropertyValue(DisplayPropertySource.PROP_X, location.x);
		dps.setPropertyValue(DisplayPropertySource.PROP_Y, location.y);
		setDisplayString(dps.getValue());
	}

	public Dimension getSize() {
		DisplayPropertySource dps = new DisplayPropertySource(getDisplayString());

		return new Dimension(dps.getIntPropertyDef(DisplayPropertySource.PROP_W, 0),
						     dps.getIntPropertyDef(DisplayPropertySource.PROP_H, 0));
	}

	public void setSize(Dimension size) {
		DisplayPropertySource dps = new DisplayPropertySource(getDisplayString());
		dps.setPropertyValue(DisplayPropertySource.PROP_W, size.width);
		dps.setPropertyValue(DisplayPropertySource.PROP_H, size.height);
		setDisplayString(dps.getValue());
	}

	public void addListener(INEDChangeListener l) {
		listeners.add(l);
	}

	public void removeListener(INEDChangeListener l) {
    	listeners.remove(l);
	}

	public void fireAttributeChanged(NEDElement node, String attr) {
		listeners.fireAttributeChanged(node, attr);
	}

	public void fireChildInserted(NEDElement node, NEDElement where, NEDElement child) {
		listeners.fireChildInserted(node, where, child);
	}

	public void fireChildRemoved(NEDElement node, NEDElement child) {
		listeners.fireChildRemoved(node, child);
	}
}
