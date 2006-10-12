package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.LiteralNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.PropertyKeyNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class NedElementExUtil implements NEDElementTags, NEDElementUtil {
	public static final String DISPLAY_PROPERTY = "display";
	
	public static String getDisplayString(NEDElement node) {
        // channel node is special. we have to look inside the channelSpec node for the display string
        if (node instanceof ConnectionNode) {
            NEDElement channelSpecNode = node.getFirstChildWithTag(NED_CHANNEL_SPEC);
            node = channelSpecNode;
        }
        
        if (node == null) return null;
        
        NEDElement parametersNode = node.getFirstChildWithTag(NED_PARAMETERS);
        if (parametersNode != null)
			for(NEDElement currElement : parametersNode)
				if(currElement instanceof PropertyNode) {
					PropertyNode currProp = (PropertyNode)currElement;
					if(DISPLAY_PROPERTY.equals(currProp.getName()))
						return getPropertyValue(currProp);
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
            !(node instanceof ConnectionNode) &&
            !(node instanceof ChannelNode) &&
			!(node instanceof ChannelInterfaceNode) &&
			!(node instanceof ChannelSpecNode))
				throw new NEDElementException(node, "Node does not support display property");
		
		// the connection node is special because the display string is stored inside its 
        // channel spec node, so we must create that too
        if (node instanceof ConnectionNode) {
            NEDElement channelSpecNode = node.getFirstChildWithTag(NED_CHANNEL_SPEC);
            if (channelSpecNode == null) {
                channelSpecNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
                node.appendChild(channelSpecNode);
            }
            // use the new channel spec node as the container of display string
            node = channelSpecNode;
        }
            
        
		// look for the parameters block (in therory it must be present)
		NEDElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);
		if (paramsNode == null) {
			paramsNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PARAMETERS);
            ((ParametersNode)paramsNode).setIsImplicit(true);
			node.appendChild(paramsNode);
		}
		
		// look for the first property parameter, named as display
		NEDElement displayPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyNode.ATT_NAME, DISPLAY_PROPERTY); 
		if (displayPropertyNode == null) {
			displayPropertyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY);
			((PropertyNode)displayPropertyNode).setName(DISPLAY_PROPERTY);
			paramsNode.appendChild(displayPropertyNode);
		}

        // if displayString was set to "" (empty) we want to delete the whole display property node
        if ("".equals(dspString)){
            paramsNode.removeChild(displayPropertyNode);
            return;
        }
		
		// look for propertykey
		NEDElement propertyKeyNode = displayPropertyNode.getFirstChildWithAttribute(NED_PROPERTY_KEY, PropertyKeyNode.ATT_NAME, ""); 
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
		
		boolean isNotifyEnabled = literalNode.isNotifyEnabled();
		literalNode.setNotifyEnabled(false);
		// finally set the value of display string
		literalNode.setType(NED_CONST_STRING);
		literalNode.setValue(dspString);
		// invalidate the text representation so next time the code will 
		// be generated from VALUE not from the text attribute
		literalNode.setText(null);
		// set notification back to the original state
		literalNode.setNotifyEnabled(isNotifyEnabled);
	}
	
	/**
	 * Returns the value of the property (assigned to the first key (default value))
	 * @param prop
	 * @return
	 */
	public static String getPropertyValue(PropertyNode prop) {
		PropertyKeyNode pkn = prop.getFirstPropertyKeyChild();
		if (pkn == null ) return null;
		LiteralNode ln = pkn.getFirstLiteralChild();
		if (ln == null ) return null;
		
		return ln.getValue();
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
