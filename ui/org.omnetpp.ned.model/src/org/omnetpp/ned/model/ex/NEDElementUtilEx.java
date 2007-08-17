package org.omnetpp.ned.model.ex;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElementException;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.LiteralNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.PropertyKeyNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public final class NEDElementUtilEx implements NEDElementTags, NEDElementUtil {
	public static final String DISPLAY_PROPERTY = "display";

	/**
	 * Returns the display string of the given node (node.getDisplayString()),
	 * or null if node==null.
	 */
	public static DisplayString displayStringOf(IHasDisplayString node) {
		return node == null ? null : node.getDisplayString();
	}
	
	/**
	 * Returns the display string of the given element (submodule, connection or
	 * toplevel type) in an unparsed form, by looking up the "@display" property
	 * in the "parameters:" section.
	 */
	public static String getDisplayString(IHasDisplayString node) {
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
	 * Sets the display property of a given node.
	 * PATH: PARAMETERS -> PROPERTY -> PROPERTYKEY -> LITERAL.VALUE
	 * @param node Component node that can have display string
	 * @param dspString
	 */
	public static void setDisplayString(INEDElement node, String dspString) {
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
            INEDElement channelSpecNode = node.getFirstChildWithTag(NED_CHANNEL_SPEC);
            if (channelSpecNode == null) {
                channelSpecNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
                node.appendChild(channelSpecNode);
            }
            // use the new channel spec node as the container of display string
            node = channelSpecNode;
        }


		// look for the parameters block (in theory it must be present)
		INEDElement paramsNode = node.getFirstChildWithTag(NED_PARAMETERS);
		if (paramsNode == null) {
			paramsNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PARAMETERS);
            ((ParametersNode)paramsNode).setIsImplicit(true);
			node.appendChild(paramsNode);
		}

		// look for the first property parameter, named as display
		INEDElement displayPropertyNode = paramsNode.getFirstChildWithAttribute(NED_PROPERTY, PropertyNode.ATT_NAME, DISPLAY_PROPERTY);
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

		// look for the property key
		INEDElement propertyKeyNode = displayPropertyNode.getFirstChildWithAttribute(NED_PROPERTY_KEY, PropertyKeyNode.ATT_NAME, "");
		if (propertyKeyNode == null) {
			propertyKeyNode = NEDElementFactoryEx.getInstance().createNodeWithTag(NED_PROPERTY_KEY);
			displayPropertyNode.appendChild(propertyKeyNode);
		}

		// look for literal
		LiteralNode literalNode = (LiteralNode)propertyKeyNode.getFirstChildWithTag(NED_LITERAL);
		if (literalNode == null) {
			literalNode = (LiteralNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_LITERAL);
	        literalNode.setType(NED_CONST_STRING);
			propertyKeyNode.appendChild(literalNode);
		}

		boolean isNotifyEnabled = literalNode.isNotificationEnabled();
		literalNode.setNotificationEnabled(false);
		// finally set the value of display string
		literalNode.setType(NED_CONST_STRING);
		literalNode.setValue(dspString);
		// invalidate the text representation so next time the code will
		// be generated from VALUE not from the text attribute
		literalNode.setText(null);
		// set notification back to the original state
		literalNode.setNotificationEnabled(isNotifyEnabled);
	}

    /**
     * Returns the name of the first "extends" node (or null if none present)
     */
    public static String getFirstExtends(INEDElement node) {
        ExtendsNode extendsNode = (ExtendsNode)node.getFirstChildWithTag(NED_EXTENDS);
        if (extendsNode == null)
            return null;

        return extendsNode.getName();
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
     * Returns the element type as a lower case string suitable for displaying
     * in the UI; e.g. for a SimpleModuleNode it returns "simple module".
     */
    public static String getReadableTagName(INEDElement node) {
    	// this is a simple solution, replace with more sophisticated code if needed
    	return node.getTagName().replace('-', ' ');
    }

	public static boolean isDisplayStringUpToDate(IHasDisplayString node) {
		String displayString = getDisplayString(node);
		return node.getDisplayString().toString().equals(displayString);
	}
}
