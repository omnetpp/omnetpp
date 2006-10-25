package org.omnetpp.ned2.model;

import org.omnetpp.common.displaymodel.ConnectionDisplayString;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayStringProvider;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;

public class ConnectionNodeEx extends ConnectionNode implements IStringTyped, IDisplayStringProvider {
	private ConnectionDisplayString displayString = null;

	ConnectionNodeEx() {
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}

	ConnectionNodeEx(NEDElement parent) {
		super(parent);
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}
    
    // helper functions to set the module names using references
    public IConnectable getSrcModuleRef() {
        if (getSrcModule() == null)
            return null;
        CompoundModuleNodeEx cm = getCompoundModule(); 
        // if the source is empty return the containing compound module
        if ("".equals(getSrcModule()))
            return cm;
        if (cm != null)
            return cm.getSubmoduleByName(getSrcModule());
        
        return null;
    }

    /**
     * Sets the source side module using reference instead of using name
     * @param srcModule 
     */
    public void setSrcModuleRef(IConnectable srcModule) {
        if (srcModule == null) {
            setSrcModule(null);
            return;
        }
        String newModule = (srcModule instanceof CompoundModuleNodeEx) ? "" : srcModule.getName();
        setSrcModule(newModule);
    }

    public IConnectable getDestModuleRef() {
        if (getDestModule() == null)
            return null;
        CompoundModuleNodeEx cm = getCompoundModule(); 
        // if the source is empty return the containing compound module
        if ("".equals(getDestModule()))
            return cm;
        if (cm != null)
            return cm.getSubmoduleByName(getDestModule());
        return null;
    }

    /**
     * Sets the source side module using reference instead of using name
     * @param srcModule 
     */
    public void setDestModuleRef(IConnectable destModule) {
        if (destModule == null) {
            setDestModule(null);
            return;
        }
        String newModule = (destModule instanceof CompoundModuleNodeEx) ? "" : destModule.getName();
        setDestModule(newModule);
    }
    
	/**
	 * @return Identifier of the source module instance the connection connected to
	 */
	public String getSrcModuleWithIndex() {
		String module = getSrcModule();
		if(getSrcModuleIndex() != null && !"".equals(getSrcModuleIndex()))
			module += "["+getSrcModuleIndex()+"]";

		return module;
	}

	/**
	 * @return Identifier of the destination gate instance the connection connected to
	 */
	public String getDestModuleWithIndex() {
		String module = getDestModule();
		if(getDestModuleIndex() != null && !"".equals(getDestModuleIndex()))
			module += "["+getDestModuleIndex()+"]";

		return module;
	}

	/**
	 * @return The fully qualified src gate name (with module, index, gate, index)
	 */
	public String getSrcGateFullyQualified() {
		String result = getSrcModuleWithIndex();
		if (!"".equals(result)) result += ".";
		result += getSrcGateWithIndex();
		return result;
	}
	/**
	 * @return The fully qualified dest gate name (with module, index, gate, index)
	 */
	public String getDestGateFullyQualified() {
		String result = getDestModuleWithIndex();
		if (!"".equals(result)) result += ".";
		result += getDestGateWithIndex();
		return result;
	}

	/**
	 * @return Identifier of the source gate instance the connection connected to
	 */
	public String getSrcGateWithIndex() {
		String gate = getSrcGate();
		if(getSrcGatePlusplus())
			gate += "++";
		if(getSrcGateIndex() != null && !"".equals(getSrcGateIndex()))
			gate += "["+getSrcGateIndex()+"]";
		if(getSrcGateSubg() == NED_SUBGATE_I) gate+="$i";
		if(getSrcGateSubg() == NED_SUBGATE_O) gate+="$o";
		return gate;
	}

	/**
	 * @return Identifier of the destination gate instance the connection connected to
	 */
	public String getDestGateWithIndex() {
		String gate = getDestGate();
		if(getDestGatePlusplus())
			gate += "++";
		if(getDestGateIndex() != null && !"".equals(getDestGateIndex()))
			gate += "["+getDestGateIndex()+"]";
		if(getDestGateSubg() == NED_SUBGATE_I) gate+="$i";
		if(getDestGateSubg() == NED_SUBGATE_O) gate+="$o";

		return gate;
	}

	public DisplayString getDisplayString() {
		if (displayString == null)
			displayString = new ConnectionDisplayString(this, NEDElementUtilEx.getDisplayString(this));
		return displayString;
	}

    public DisplayString getEffectiveDisplayString() {
        return NEDElementUtilEx.getEffectiveDisplayString(this);
    }

    public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model
		NEDElementUtilEx.setDisplayString(this, displayString.toString());
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

    /**
     * @return The compound module containing the definition of this connection
     */
    public CompoundModuleNodeEx getCompoundModule() {
        NEDElement parent = getParent(); 
        while (parent != null && !(parent instanceof CompoundModuleNodeEx)) 
            parent = parent.getParent();
        return (CompoundModuleNodeEx)parent;
    }
    
    /**
     * Checks if the current connection is valid (has valid submodules at both end) 
     * @return 
     */
    public boolean isValid() {
        return getSrcModuleRef() != null && getDestModuleRef() != null;
    }

    // type management
    
    public String getType() {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        if(channelSpecNode == null)
            return null;

        return channelSpecNode.getType();
    }

    public void setType(String type) {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        if (channelSpecNode == null) {
            channelSpecNode = (ChannelSpecNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
            appendChild(channelSpecNode);
        }
        channelSpecNode.setType(type);
    }

    public INEDTypeInfo getTypeNEDTypeInfo() {
        String typeName = getType(); 
        if ( typeName == null || "".equals(typeName))
            return null;

        return getContainerNEDTypeInfo().getResolver().getComponent(typeName);
    }

    public NEDElement getTypeRef() {
        INEDTypeInfo it = getTypeNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
    }
}
