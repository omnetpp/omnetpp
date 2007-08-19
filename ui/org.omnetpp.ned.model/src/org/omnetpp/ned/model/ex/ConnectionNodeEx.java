package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasConnections;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ChannelSpecNode;
import org.omnetpp.ned.model.pojo.ConnectionNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ConnectionNodeEx extends ConnectionNode implements IHasType, IHasDisplayString, IHasParameters {
	
	private DisplayString displayString = null;

    protected ConnectionNodeEx() {
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}

    protected ConnectionNodeEx(INEDElement parent) {
		super(parent);
		setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
	}

    public IHasConnections getSrcModuleRef() {
    	return resolveConnectedModule(getSrcModule());
    }

    public IHasConnections getDestModuleRef() {
    	return resolveConnectedModule(getDestModule());
    }

    /**
     * Sets the source module of the connection
     */
    public void setSrcModuleRef(IHasConnections srcModule) {
    	setSrcModule(connectedModuleName(srcModule));
    }

    /**
     * Sets the destination module of the connection
     */
    public void setDestModuleRef(IHasConnections destModule) {
    	setDestModule(connectedModuleName(destModule));
    }

    protected String connectedModuleName(IHasConnections module) {
        return module == null ? null : (module instanceof CompoundModuleNodeEx) ? "" : module.getName();
    }

    // helper functions to set the module names using references
    protected IHasConnections resolveConnectedModule(String moduleName) {
        if (moduleName == null)
            return null; // not connected

        CompoundModuleNodeEx compoundModule = getCompoundModule();
        if (compoundModule == null)
        	return null; // this is a detached connection
        else
        	return moduleName.equals("") ? compoundModule : compoundModule.getSubmoduleByName(moduleName);
    }

    /**
	 * Returns the identifier of the source module instance the connection connected to
	 */
	//FIXME factor out common part with next one
	public String getSrcModuleWithIndex() {
		String module = getSrcModule();
		if (getSrcModuleIndex() != null && !"".equals(getSrcModuleIndex()))
			module += "["+getSrcModuleIndex()+"]";

		return module;
	}

	/**
	 * Returns the identifier of the destination gate instance the connection connected to
	 */
	public String getDestModuleWithIndex() {
		String module = getDestModule();
		if (getDestModuleIndex() != null && !"".equals(getDestModuleIndex()))
			module += "["+getDestModuleIndex()+"]";

		return module;
	}

	/**
	 * Returns the fully qualified src gate name (with module, index, gate, index)
	 */
	public String getSrcGateFullyQualified() {
		String result = getSrcModuleWithIndex();
		if (!"".equals(result)) result += ".";
		result += getSrcGateWithIndex();
		return result;
	}
	/**
	 * Returns the fully qualified dest gate name (with module, index, gate, index)
	 */
	public String getDestGateFullyQualified() {
		String result = getDestModuleWithIndex();
		if (!"".equals(result)) result += ".";
		result += getDestGateWithIndex();
		return result;
	}

	/**
	 * Returns the identifier of the source gate instance the connection connected to
	 */
	//FIXME factor out common part with next one
	public String getSrcGateWithIndex() {
		String gate = getSrcGate();
		if (getSrcGatePlusplus())
			gate += "++";
		if (getSrcGateIndex() != null && !"".equals(getSrcGateIndex()))
			gate += "["+getSrcGateIndex()+"]";
		if (getSrcGateSubg() == NED_SUBGATE_I) gate+="$i";
		if (getSrcGateSubg() == NED_SUBGATE_O) gate+="$o";
		return gate;
	}

	/**
	 * Returns the identifier of the destination gate instance the connection connected to
	 */
	public String getDestGateWithIndex() {
		String gate = getDestGate();
		if (getDestGatePlusplus())
			gate += "++";
		if (getDestGateIndex() != null && !"".equals(getDestGateIndex()))
			gate += "["+getDestGateIndex()+"]";
		if (getDestGateSubg() == NED_SUBGATE_I) gate+="$i";
		if (getDestGateSubg() == NED_SUBGATE_O) gate+="$o";

		return gate;
	}

    @Override
    public void fireModelChanged(NEDModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NEDElementUtilEx.isDisplayStringUpToDate(displayString, this))
    		displayString = null;
    	super.fireModelChanged(event);
    }

    public DisplayString getDisplayString() {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(NEDElementUtilEx.displayStringOf(getEffectiveTypeRef()));
    	return displayString;
    }

    /**
     * Returns the compound module containing this connection, or null if the
     * connection is not part of the model (i.e. has no compound module parent).
     */
    public CompoundModuleNodeEx getCompoundModule() {
    	return (CompoundModuleNodeEx)getEnclosingTypeNode();
    }

    /**
     * Checks whether the current connection is valid (has valid submodules at both end)
     */
    public boolean isValid() {
        return getSrcModuleRef() != null && getDestModuleRef() != null;
    }

    // type management

    public String getType() {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        return channelSpecNode == null ? null : channelSpecNode.getType();
    }

    public void setType(String type) {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        if (channelSpecNode == null) {
            channelSpecNode = (ChannelSpecNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
            appendChild(channelSpecNode);
        }
        channelSpecNode.setType(type);
    }

    public String getLikeType() {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        return channelSpecNode == null ? null : channelSpecNode.getLikeType();
    }

    public void setLikeType(String type) {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);

        if (channelSpecNode == null) {
            channelSpecNode = (ChannelSpecNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
            appendChild(channelSpecNode);
        }
        channelSpecNode.setLikeType(type);
    }

    public String getLikeParam() {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);
        return channelSpecNode == null ? null : channelSpecNode.getLikeParam();
    }

    public void setLikeParam(String param) {
        ChannelSpecNode channelSpecNode = (ChannelSpecNode)getFirstChildWithTag(NED_CHANNEL_SPEC);

        if (channelSpecNode == null) {
            channelSpecNode = (ChannelSpecNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_CHANNEL_SPEC);
            appendChild(channelSpecNode);
        }
        channelSpecNode.setLikeParam(param);
    }

    public String getEffectiveType() {
        String likeType = getLikeType();
        return StringUtils.isEmpty(likeType) ? getType() : likeType;
    }

    public INEDTypeInfo getNEDTypeInfo() {
        return resolveTypeName(getEffectiveType());
    }

    public INedTypeNode getEffectiveTypeRef() {
        INEDTypeInfo info = getNEDTypeInfo();
        return info == null ? null : info.getNEDElement();
    }

    /**
     * Returns a list of all parameters assigned in this module (inside the channel spec element)
     */
    public List<ParamNodeEx> getOwnParams() {
        List<ParamNodeEx> result = new ArrayList<ParamNodeEx>();

        ChannelSpecNode channelSpecNode = getFirstChannelSpecChild();;
        if (channelSpecNode == null)
            return result;

        ParametersNode parametersNode = channelSpecNode.getFirstParametersChild();
        if (parametersNode != null)
        	for (INEDElement currChild : parametersNode)
        		if (currChild instanceof ParamNodeEx)
        			result.add((ParamNodeEx)currChild);

        return result;
    }

    // parameter query support
    
    public Map<String, ParamNode> getParamValues() {
    	Map<String, ParamNode> result = new HashMap<String, ParamNode>();

    	INEDTypeInfo info = getNEDTypeInfo();
    	if (info != null) 
    		result.putAll(info.getParamValues());

        // add our own assigned parameters
        for (ParamNodeEx ownParam : getOwnParams())
            result.put(ownParam.getName(), ownParam);

        return result;
    }

    public Map<String, ParamNode> getParams() {
        INEDTypeInfo info = getNEDTypeInfo();
        return info == null ? new HashMap<String, ParamNode>() : info.getParams();
    }

}
