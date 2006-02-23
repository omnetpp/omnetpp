package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.PropertyNode;

public class NedElementExUtil implements NEDElementTags {
	public static final String DISPLAY_PROPERTY = "display";
	
	public static String getDisplayString(NEDElement node) {
		// check all parameters
		try {
			for(NEDElement currElement : node.getFirstChildWithTag(NED_PARAMETERS))
				if(currElement instanceof PropertyNode) {
					PropertyNode currProp = (PropertyNode)currElement;
					if(DISPLAY_PROPERTY.equals(currProp.getName()))
						return currProp.getFirstPropertyKeyChild()
											.getFirstLiteralChild().getValue();
				}
		} catch (NullPointerException e) {
			// display string is in illegal format, missing value etc (e.g @display )
		}
		return null;
	}
	
	public static void setDisplayString(NEDElement node, String dspString) {
		// TODO implement setter for display property 
	}
	
    /**
     * Filters the connections within the given ConnectionsNode by the "src-module" attribute 
     */
    public static List<ConnectionNodeEx> getSourceConnections(ConnectionsNode conns, String moduleName) {
		// connections may occur as direct children, or within ConnectionGroupNodes
		ArrayList<ConnectionNodeEx> list = new ArrayList<ConnectionNodeEx>();
		if (conns == null)
			return list;
		for (NEDElement child : conns) {
			if (child instanceof ConnectionNodeEx) {
				ConnectionNodeEx conn = (ConnectionNodeEx)child;
				if (conn.getSrcModule().equals(moduleName))
					list.add(conn);
			}
			if (child instanceof ConnectionGroupNode) {
				for (NEDElement child2 : conns) {
					if (child2 instanceof ConnectionNodeEx) {
						ConnectionNodeEx conn = (ConnectionNodeEx)child2;
						if (conn.getSrcModule().equals(moduleName))
							list.add(conn);
					}
				}
			}
		}
    	return list;
    }
    
    /**
     * Filters the connections within the given ConnectionsNode by the "dest-module" attribute 
     */
	public static List<ConnectionNodeEx> getTargetConnections(ConnectionsNode conns, String moduleName) {
		// connections may occur as direct children, or within ConnectionGroupNodes
		ArrayList<ConnectionNodeEx> list = new ArrayList<ConnectionNodeEx>();
		if (conns == null)
			return list;
		for (NEDElement child : conns) {
			if (child instanceof ConnectionNodeEx) {
				ConnectionNodeEx conn = (ConnectionNodeEx)child;
				if (conn.getDestModule().equals(moduleName))
					list.add(conn);
			}
			if (child instanceof ConnectionGroupNode) {
				for (NEDElement child2 : conns) {
					if (child2 instanceof ConnectionNodeEx) {
						ConnectionNodeEx conn = (ConnectionNodeEx)child2;
						if (conn.getDestModule().equals(moduleName))
							list.add(conn);
					}
				}
			}
		}
    	return list;
	}
}
