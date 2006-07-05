package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Bendpoint;
import org.omnetpp.ned2.model.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.ChannelSpecNode;
import org.omnetpp.ned2.model.pojo.ConnectionNode;

public class ConnectionNodeEx extends ConnectionNode implements IDisplayStringProvider {
	private IConnectable srcModuleRef;
	private IConnectable destModuleRef;
	private ConnectionDisplayString displayString = null;

	public ConnectionNodeEx() {
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}

	public ConnectionNodeEx(NEDElement parent) {
		super(parent);
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}

    public String getAttributeDefault(int k) {
        // Override the default value of "src-module" and "dest-module" to null (==unset).
    	// The original value of "" causes an exception if we create a ConnectionNodeEx
    	// without a parent, because applyDefaults() in the ctor would try to set 
    	// srcModuleRef/destModuleRef to the enclosing compound module (which doesn't exist).
    	// Ctor without parent is needed by GEF's CreationTool, see org.eclipse.gef.requests.SimpleFactory.
    	assert(getAttributeName(0).equals(ATT_SRC_MODULE) && getAttributeName(6).equals(ATT_DEST_MODULE));
        switch (k) {
            case 0: return null;
            case 6: return null;
            default: return super.getAttributeDefault(k);
        }
    }
	
	public IConnectable getSrcModuleRef() {
		return srcModuleRef;
	}
	
	public void setSrcModuleRef(IConnectable srcModule) {
		if(srcModuleRef == srcModule) 
			return;
		
		if (srcModuleRef != null) 
			srcModuleRef.detachSrcConnection(this);
		srcModuleRef = srcModule;
        if(srcModuleRef != null)
            srcModuleRef.attachSrcConnection(this);
		
        fireAttributeChangedToAncestors(ATT_SRC_MODULE);
	}

	public IConnectable getDestModuleRef() {
		return destModuleRef;
	}

	public void setDestModuleRef(IConnectable destModule) {
		if (destModuleRef == destModule)
			return;
		
		if (destModuleRef != null) 
			destModuleRef.detachDestConnection(this);
		destModuleRef = destModule;
        if (destModuleRef != null)
            destModuleRef.attachDestConnection(this);
		
        fireAttributeChangedToAncestors(ATT_DEST_MODULE);
	}

	@Override
	public String getDestModule() {
		// if the destination is a submodule module return its name  
		if(destModuleRef instanceof SubmoduleNodeEx) 
			return ((INamed)destModuleRef).getName();
		// else (if the destination is a compound module) return empty as a name
		return "";
	}

	@Override
	public String getSrcModule() {
		// if the source is a submodule module return its name  
		if(srcModuleRef instanceof SubmoduleNodeEx) 
			return ((INamed)srcModuleRef).getName();
		// else (if the source is a compound module) return empty as a name
		return "";
	}

	@Override
	public void setDestModule(String val) {
		setDestModuleRef(getSubmoduleByName(val));
	}

	@Override
	public void setSrcModule(String val) {
		setSrcModuleRef(getSubmoduleByName(val));
	}
	
	/**
	 * @return Identifier of the source gate instance the connection connected to
	 */
	public String getSrcGateWithIndex() {
		String gate = getSrcGate();
		if(getSrcGateIndex() != null && !"".equals(getSrcGateIndex()))
			gate += "["+getSrcGateIndex()+"]";
		
		return gate;
	}
	
	/**
	 * @return Identifier of the destination gate instance the connection connected to
	 */
	public String getDestGateWithIndex() {
		String gate = getDestGate();
		if(getDestGateIndex() != null && !"".equals(getDestGateIndex()))
			gate += "["+getDestGateIndex()+"]";
		
		return gate;
	}

	private INamedGraphNode getSubmoduleByName(String moduleName) {
		CompoundModuleNodeEx compMod = (CompoundModuleNodeEx)getParentWithTag(NED_COMPOUND_MODULE);
		assert (compMod != null);
		// check if the module name is empty. we should return the parent compoud module
		if("".equals(moduleName)) 
			return compMod;
		else {
			INamedGraphNode subMod = compMod.getSubmoduleByName(moduleName);
			if (subMod == null) throw new NEDElementException(this, "'"+moduleName+"': undefined submodule");
			return subMod;
		}
	}

	public List getBendpoints() {
		// TODO Implement bendpoints model
		return new ArrayList();
	}

	public void insertBendpoint(int index, Bendpoint wbp) {
		// TODO Implement this
	}

	public void removeBendpoint(int index) {
		// TODO Implement this
	}

	public void setBendpoint(int index, Bendpoint bp) {
		// TODO Implement this
	}

	public DisplayString getDisplayString() {
		// TODO set the ancestor connection and container module correctly
		if (displayString == null)
			displayString = new ConnectionDisplayString(this, null,
											NedElementExUtil.getDisplayString(this));
		return displayString;
	}
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NedElementExUtil.setDisplayString(this, displayString.toString());
	}

    public String getChannelType() {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        if(channelSpecNode == null) 
            return "";
        
        return channelSpecNode.getType();
    }
    
    public void setChannelType(String type) {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
            if (channelSpecNode == null) {
                channelSpecNode = (ChannelSpecNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
                appendChild(channelSpecNode);
            }
            channelSpecNode.setType(type);
    }
}
