package org.omnetpp.ned.model.ex;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElementConstants;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.LiteralNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.PropertyKeyNode;
import org.omnetpp.ned.model.pojo.PropertyNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NEDElementUtilEx implements NEDElementTags, NEDElementConstants {
	public static final String DISPLAY_PROPERTY = "display";

	//XXX move to NEDElement
	public static boolean isDisplayStringUpToDate(DisplayString displayStringObject, IHasDisplayString owner) {
		// Note: cannot call owner.getDisplayString() here, as it also wants to update the fallback, etc.
		String displayStringLiteral = StringUtils.nullToEmpty(getDisplayStringLiteral(owner));
		String cachedDisplayString = displayStringObject == null ? "" : displayStringObject.toString();
		return displayStringLiteral.equals(cachedDisplayString);
	}

	/**
	 * Convenience method: Returns node.getDisplayString(), or null if node==null.
	 */
	public static DisplayString displayStringOf(IHasDisplayString node) {
		return node == null ? null : node.getDisplayString();
	}
	
	/**
	 * Returns the display string of the given element (submodule, connection or
	 * toplevel type) in an unparsed form, from the NED tree. 
	 * 
	 * Returns null if the NED tree doesn't contain a display string.
	 */
	public static String getDisplayStringLiteral(IHasDisplayString node) {
        // for connections, we need to look inside the channelSpec node for the display string
		INEDElement parametersParent = (node instanceof ConnectionNode) ?
				((ConnectionNode)node).getFirstChildWithTag(NED_CHANNEL_SPEC) : (INEDElement)node;

		// look for the "display" property inside the parameters section (if it exists)
        INEDElement parametersNode = parametersParent == null ? null : parametersParent.getFirstChildWithTag(NED_PARAMETERS);
        if (parametersNode != null)
        	for (INEDElement currElement : parametersNode)
        		if (currElement instanceof PropertyNode) {
        			PropertyNode currProp = (PropertyNode)currElement;
        			if (DISPLAY_PROPERTY.equals(currProp.getName()))
        				return getPropertyValue(currProp);
        		}
        return null;
	}

	/**
	 * Sets the display string of a given node in the NED tree,
	 * by creating or updating the LiteralNode that contains the
	 * "@display" property in the tree.
	 * 
	 * Returns the LiteralNode which was created/updated. 
	 */
	public static LiteralNode setDisplayStringLiteral(IHasDisplayString node1, String displayString) {
		INEDElement node = node1;
		
		// the connection node is special because the display string is stored inside its
        // channel spec node, so we must create that too
        if (node instanceof ConnectionNode) {
            INEDElement channelSpecNode = node.getFirstChildWithTag(NED_CHANNEL_SPEC);
            if (channelSpecNode == null) {
                channelSpecNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
                node.appendChild(channelSpecNode);
            }
            // use the new channel spec node as the container of display string
            node = channelSpecNode;
        }

		// look for the "parameters" block
		INEDElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);
		if (paramsNode == null) {
			paramsNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PARAMETERS);
            ((ParametersNode)paramsNode).setIsImplicit(true);
			node.appendChild(paramsNode);
		}

		// look for the first property parameter named "display"
		INEDElement displayPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyNode.ATT_NAME, DISPLAY_PROPERTY);
		if (displayPropertyNode == null) {
			displayPropertyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY);
			((PropertyNode)displayPropertyNode).setName(DISPLAY_PROPERTY);
			paramsNode.appendChild(displayPropertyNode);
		}

        // if displayString was set to "" (empty), we want to delete the whole display property node
        if ("".equals(displayString)){
            paramsNode.removeChild(displayPropertyNode);
            return null;
        }

		// look for the property key
		INEDElement propertyKeyNode = displayPropertyNode.getFirstChildWithAttribute(NED_PROPERTY_KEY, PropertyKeyNode.ATT_NAME, "");
		if (propertyKeyNode == null) {
			propertyKeyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY_KEY);
			displayPropertyNode.appendChild(propertyKeyNode);
		}

		// look up or create the LiteralNode
		LiteralNode literalNode = (LiteralNode)propertyKeyNode.getFirstChildWithTag(NED_LITERAL);
		if (literalNode == null)
			literalNode = (LiteralNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_LITERAL);

		// fill in the LiteralNode. if()'s are there to minimize the number of notifications
		if (literalNode.getType() != NED_CONST_STRING)
			literalNode.setType(NED_CONST_STRING);
		if (!displayString.equals(literalNode.getValue()))
			literalNode.setValue(displayString);
		// invalidate the text representation, so that next time the code will
		// be generated from VALUE, not from the text attribute
		if (literalNode.getText() != null)
			literalNode.setText(null);
		// if new, add it only here (also to minimize notifications)
		if (literalNode.getParent() == null)
			propertyKeyNode.appendChild(literalNode);
		return literalNode; 
	}

    /**
     * Returns the name of the first "extends" node (or null if none present)
     */
    public static String getFirstExtends(INEDElement node) {
        ExtendsNode extendsNode = (ExtendsNode)node.getFirstChildWithTag(NED_EXTENDS);
        return extendsNode == null ? null : extendsNode.getName();
    }

    /**
     * Sets the name of object that is extended by node. if we set it to null or "" the node will be removed
     * @param node The node which extends the provided type
     * @param ext The name of the type that is extended
     */
    public static void setFirstExtends(INEDElement node, String ext) {
        ExtendsNode extendsNode = (ExtendsNode)node.getFirstChildWithTag(NED_EXTENDS);
            if (extendsNode == null && ext != null && !"".equals(ext)) {
                extendsNode = (ExtendsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_EXTENDS);
                node.appendChild(extendsNode);
            }
            else if (extendsNode != null && (ext == null || "".equals(ext))) {
                // first set the name to "" so we will generate an attribute change event
                extendsNode.setName("");
                // remove the node if we would set the name to null or empty string
                node.removeChild(extendsNode);
            }
            if (extendsNode != null)
                extendsNode.setName(ext);
    }

	/**
	 * Returns the value of the given property (assigned to the first key (default value))
	 */
	public static String getPropertyValue(PropertyNode prop) {
		PropertyKeyNode pkn = prop.getFirstPropertyKeyChild();
		if (pkn == null ) return null;
		LiteralNode ln = pkn.getFirstLiteralChild();
		if (ln == null ) return null;

		return ln.getValue();
	}

    /**
     * Returns the name of component but stripped any digits from the right ie: name123 would be name
     */
    private static String getNameBase(String name) {
        int i = name.length()-1;
        while (i>=0 && Character.isDigit(name.charAt(i))) --i;
        // strip away the digits at the end
        return name.substring(0,i+1);
    }

    /**
     * Calculates a unique name for the provided model element
     * @param namedElement
     * @param contextCollection A collection of IHasName elements wich proviedes a context in which the name should be unique
     * @return The new unique name, or the original name if it was unique
     */
    public static String getUniqueNameFor(IHasName namedElement, Set<String> contextCollection) {

        String currentName = namedElement.getName();
        // if there is no name in the context with the same name we don't have to change the name
        if (!contextCollection.contains(currentName))
            return currentName;

        // there is an other module with the same name, so find a new name
        String baseName = getNameBase(currentName);
        int i = 1;
        while(contextCollection.contains(new String(baseName+i)))
            i++;

        // we've found a unique name
        return baseName+i;
    }

    /**
     * Calculates a unique name for the provided model element
     * @param namedElement
     * @param contextCollection A collection of strings which provides a context in which the name should be unique
     * @return The new unique name, or the original name if it was unique
     */
    public static String getUniqueNameFor(IHasName namedElement, Collection<? extends IHasName> contextCollection) {
        Set<String> nameSet = new HashSet<String>(contextCollection.size());
        // create a string set from the sibling submodules (except the node we want to make unique)
        for (IHasName sm : contextCollection)
            if (sm != namedElement)
                nameSet.add(sm.getName());
        return getUniqueNameFor(namedElement, nameSet);
    }

    /**
     * When the user renames a submodule, we need to update the connections in the 
     * same compound module (and its subclasses) accordingly, so that the model 
     * will remain consistent. This method performs this change, for one compound
     * module.
     */
    public static void renameSubmoduleInConnections(CompoundModuleNodeEx compoundModule, String oldSubmoduleName, String newSubmoduleName) {
		INEDElement connectionsNode = compoundModule.getFirstConnectionsChild();
		if (connectionsNode != null)
			doRenameSubmoduleInConnections(connectionsNode, oldSubmoduleName, newSubmoduleName);
	}

	protected static void doRenameSubmoduleInConnections(INEDElement parent, String oldName, String newName) {
		for (INEDElement child : parent) {
			if (child instanceof ConnectionNodeEx) {
				ConnectionNodeEx conn = (ConnectionNodeEx) child;
				if (conn.getSrcModule().equals(oldName))
					conn.setSrcModule(newName);
				if (conn.getDestModule().equals(oldName))
					conn.setDestModule(newName);
			}
			else if (child instanceof ConnectionGroupNode) {
				doRenameSubmoduleInConnections(child, oldName, newName);
			}
		}
	}

}
