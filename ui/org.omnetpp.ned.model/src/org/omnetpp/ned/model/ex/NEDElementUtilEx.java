package org.omnetpp.ned.model.ex;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IHasDisplayString;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.NEDElementException;
import org.omnetpp.ned.model.NEDElementUtil;
import org.omnetpp.ned.model.interfaces.IHasAncestors;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.interfaces.IModuleTypeNode;
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

public final class NEDElementUtilEx implements NEDElementTags, NEDElementUtil {
	public static final String DISPLAY_PROPERTY = "display";
	
	/**
	 * @param node The node in question
	 * @return The display string in unparesed form
	 */
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
     * Gets the effective displaystring (parsed )belonging to a node. It uses 
     * the node's type or the base node (extends) as a fallback displaystring.
     * BEWARE that this method adjusts the display string's 'Default' property,
     * ie. sets it to an other displayString object which represents the type
     * or base type's display string. 
     * @param node
     * @return The parsed display string 
     */
    public static DisplayString getEffectiveDisplayString(IHasDisplayString node) {
        DisplayString result = node.getDisplayString();
        NEDElement defaultNode = null;
        // if node supports typing use the type's diplay property
        if (node instanceof IHasType)
            defaultNode = ((IHasType)node).getEffectiveTypeRef();
        else if (node instanceof IHasAncestors)
            defaultNode = ((IHasAncestors)node).getFirstExtendsRef();
        // if we do not have type or do not extend anybody we return the same displaystring
        if (defaultNode == null) {
            // if no type info present or it has no ancestors, delete the display string default
            // ie. no inheritance from other displaystring
            result.setDefaults(null);
            return result;
        }

        Assert.isTrue(defaultNode instanceof IHasDisplayString);
        // otherwise set the default display string
        result.setDefaults(((IHasDisplayString)defaultNode).getEffectiveDisplayString());
        return result;
    }

    /**
     * Returns the effective display string for this submodule, assuming 
     * that the submodule's actual type is the compound or simple module type
     * passed in the <code>submoduleType</code> parameter. This is useful
     * when the submodule is a "like" submodule, whose the actual submodule
     * type (not the <code>likeType</code>) is known. The latter usually 
     * comes from an ini file or some other source outside the NEDElement tree. 
     * Used within the inifile editor.
     * 
     * @param submoduleNode  the submodule
     * @param submoduleType  a CompoundModuleNodeEx or a SimpleModuleNodeEx
     */
    public static DisplayString getEffectiveDisplayString(SubmoduleNodeEx submoduleNode, IModuleTypeNode submoduleType) {
        DisplayString result = submoduleNode.getDisplayString();
        Assert.isTrue(submoduleType instanceof SimpleModuleNodeEx || submoduleType instanceof CompoundModuleNodeEx);
        result.setDefaults(submoduleType.getEffectiveDisplayString());
        return result;
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
		
		boolean isNotifyEnabled = literalNode.getListeners().isEnabled();
		literalNode.getListeners().setEnabled(false);
		// finally set the value of display string
		literalNode.setType(NED_CONST_STRING);
		literalNode.setValue(dspString);
		// invalidate the text representation so next time the code will 
		// be generated from VALUE not from the text attribute
		literalNode.setText(null);
		// set notification back to the original state
		literalNode.getListeners().setEnabled(isNotifyEnabled);
	}
	
    /**
     * @return The name of the first extends nod (or null if none present)
     */
    public static String getFirstExtends(NEDElement node) {
        ExtendsNode extendsNode = (ExtendsNode)node.getFirstChildWithTag(NED_EXTENDS);
        if(extendsNode == null)
            return null;

        return extendsNode.getName();
    }

    /**
     * Sets the name of object that is extendes by node. if we set it to null or "" the node will be removed
     * @param node The node which extends the provided type
     * @param ext The name of the type that is extended
     */
    public static void setFirstExtends(NEDElement node, String ext) {
        ExtendsNode extendsNode = (ExtendsNode)node.getFirstChildWithTag(NED_EXTENDS);
            if (extendsNode == null && ext != null && !"".equals(ext)) {
                extendsNode = (ExtendsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_EXTENDS);
                node.appendChild(extendsNode);
            } else if (extendsNode != null && (ext == null || "".equals(ext))) {
                // first set the name to "" so we will generate an attribute change event
                extendsNode.setName("");
                // remove the node if we would set the name to null or empty string
                node.removeChild(extendsNode);
            }
            if (extendsNode != null)
                extendsNode.setName(ext);
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
     * @return The name of component but stripped any digits from the right ie: name123 would be name
     */
    private static String getNameBase(String name) {
        String nameBase = name;
        int i=nameBase.length()-1;
        while(i>=0 && Character.isDigit(nameBase.charAt(i))) --i;
        // strip away the digits at the end
        return nameBase.substring(0,i+1);
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
     * @param contextCollection A collection of strings wich proviedes a context in which the name should be unique
     * @return The new unique name, or the original name if it was unique
     */
    public static String getUniqueNameFor(IHasName namedElement, Collection<? extends IHasName> contextCollection) {
        Set<String> nameSet = new HashSet<String>(contextCollection.size());
        // create a string set from the sibling submodules (except the node we want to make unique)
        for(IHasName sm : contextCollection)
            if (sm != namedElement)
                nameSet.add(sm.getName());
        return getUniqueNameFor(namedElement, nameSet);
    }
    
}
