package org.omnetpp.ned2.model.ex;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementException;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.interfaces.IDerived;
import org.omnetpp.ned2.model.interfaces.INamed;
import org.omnetpp.ned2.model.interfaces.ITyped;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;
import org.omnetpp.ned2.model.pojo.LiteralNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.PropertyKeyNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

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
     * @param node
     * @return The parsed displaystring (with defaults set to 
     */
    public static DisplayString getEffectiveDisplayString(IDisplayStringProvider node) {
        DisplayString result = node.getDisplayString();
        NEDElement defaultNode = null;
        // if node supports typing use the type's diplay property
        if (node instanceof ITyped)
            defaultNode = ((ITyped)node).getTypeRef();
        else if (node instanceof IDerived)
            defaultNode = ((IDerived)node).getFirstExtendsRef();
        // if we do not have type or do not extend anybody we return the same displaystring
        if (defaultNode == null)
            return result;

        Assert.isTrue(defaultNode instanceof IDisplayStringProvider);
        // otherwise set the default display string
        result.setDefaults(((IDisplayStringProvider)defaultNode).getEffectiveDisplayString());
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
     * @param contextCollection A collection of INamed elements wich proviedes a context in which the name should be unique
     * @return The new unique name, or the original name if it was unique
     */
    public static String getUniqueNameFor(INamed namedElement, Set<String> contextCollection) {

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
    public static String getUniqueNameFor(INamed namedElement, Collection<? extends INamed> contextCollection) {
        Set<String> nameSet = new HashSet<String>(contextCollection.size());
        // create a string set from the sibling submodules (except the node we want to make unique)
        for(INamed sm : contextCollection)
            if (sm != namedElement)
                nameSet.add(sm.getName());
        return getUniqueNameFor(namedElement, nameSet);
    }
    
}
