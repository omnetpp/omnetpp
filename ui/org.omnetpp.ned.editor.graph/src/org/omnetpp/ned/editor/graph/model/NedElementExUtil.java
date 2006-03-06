package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.LiteralNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.PropertyKeyNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class NedElementExUtil implements NEDElementTags, NEDElementUtil {
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
	
	/**
	 * Sets the display property of a given node.
	 * PATH: PARAMETERS -> PROPERTY -> PROPERTYKEY -> LITERAL.VALUE
	 * @param node Component node that can have display string
	 * @param dspString
	 */
	public static void setDisplayString(NEDElement node, String dspString) {
		// check if it's a supported node type
		if (!(node instanceof CompoundModuleNode) &&
			!(node instanceof SubmoduleNode) &&
			!(node instanceof SimpleModuleNode) &&
			!(node instanceof ModuleInterfaceNode) &&
			!(node instanceof ChannelNode) &&
			!(node instanceof ChannelInterfaceNode) &&
			!(node instanceof ChannelSpecNode))
				throw new IllegalArgumentException("Node does not support display property");
		
		// look for the parameters block (in therory it must be present)
		NEDElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);
		if (paramsNode == null) {
			paramsNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PARAMETERS);
			node.appendChild(paramsNode);
		}
		
		// look for the first property parameter, named as display
		NEDElement displayPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyNode.ATT_NAME, DISPLAY_PROPERTY); 
		if (displayPropertyNode == null) {
			displayPropertyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY);
			((PropertyNode)displayPropertyNode).setName(DISPLAY_PROPERTY);
			paramsNode.appendChild(displayPropertyNode);
		}
		
		// look for propertykey
		NEDElement propertyKeyNode = displayPropertyNode.getFirstChildWithAttribute(NED_PROPERTY_KEY, PropertyKeyNode.ATT_KEY, ""); 
		if (propertyKeyNode == null) {
			propertyKeyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY_KEY);
			displayPropertyNode.appendChild(propertyKeyNode);
		}

		// look for literal
		LiteralNode literalNode = (LiteralNode)propertyKeyNode.getFirstChildWithTag(NED_LITERAL); 
		if (literalNode == null) {
			literalNode = (LiteralNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_LITERAL);
			propertyKeyNode.appendChild(literalNode);
		}
		
		// finally set the value of display string
		literalNode.setType(NED_CONST_STRING);
		literalNode.setValue(dspString);
		// invalidate the text representation so next time the code will 
		// be generated from VALUE not from the text attribute
		literalNode.setText(null);
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
