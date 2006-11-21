package org.omnetpp.ned2.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IHasGates;
import org.omnetpp.ned2.model.interfaces.IHasIndex;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.interfaces.IHasName;
import org.omnetpp.ned2.model.interfaces.INamedGraphNode;
import org.omnetpp.ned2.model.interfaces.IHasParameters;
import org.omnetpp.ned2.model.interfaces.IHasType;
import org.omnetpp.ned2.model.pojo.GatesNode;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public final class SubmoduleNodeEx extends SubmoduleNode
                            implements INamedGraphNode, IHasIndex, IHasType, 
                                       IHasParameters, IHasGates {

    protected DisplayString displayString = null;

    protected SubmoduleNodeEx() {
        init();
	}

    protected SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(IHasName.INITIAL_NAME);
        setType("node");
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getVectorSize() != null && !"".equals(getVectorSize()))
            result += "["+getVectorSize()+"]";
        return result;
    }


	public DisplayString getDisplayString() {
		if (displayString == null) {
			displayString = new DisplayString(this, NEDElementUtilEx.getDisplayString(this));
		}
		return displayString;
	}

    public DisplayString getEffectiveDisplayString() {
        return NEDElementUtilEx.getEffectiveDisplayString(this);
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
    
	// connection related methods
    
	/**
	 * @return All source connections that connect to this node and defined 
     * in the parent compound module connections defined in derived modules 
     * are NOT included here
	 */
	public List<ConnectionNodeEx> getSrcConnections() {
		return getCompoundModule().getSrcConnectionsFor(this);
	}

    /**
     * @return All connections that connect to this node and defined in the 
     * parent compound module connections defined in derived modules are 
     * NOT included here
     */
	public List<ConnectionNodeEx> getDestConnections() {
		return getCompoundModule().getDestConnectionsFor(this);
	}

	@Override
    public String debugString() {
        return "submodule: "+getName();
    }

    // type support
    public INEDTypeInfo getTypeNEDTypeInfo() {
        String typeName = getEffectiveType();
        INEDTypeInfo typeInfo = getContainerNEDTypeInfo(); 
        if ( typeName == null || "".equals(typeName) || typeInfo == null)
            return null;

        return typeInfo.getResolver().getComponent(typeName);
    }

    public NEDElement getEffectiveTypeRef() {
        INEDTypeInfo it = getTypeNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
    }

    public String getEffectiveType() {
        String type = getLikeType();
        // if it's not specified use the likeType instead
        if (type == null || "".equals(type))
            type = getType();

        return type;
    }
    
    /**
     * @return All parameters assigned in this submodule's body
     */
    public List<ParamNodeEx> getOwnParams() {
        // FIXME does not include parameters in param groups !!!
        List<ParamNodeEx> result = new ArrayList<ParamNodeEx>();
        ParametersNode parametersNode = getFirstParametersChild();
        if (parametersNode == null)
            return result;
        for(NEDElement currChild : parametersNode)
            if (currChild instanceof ParamNodeEx)
                result.add((ParamNodeEx)currChild);

        return result;
    }
    
    // parameter query support
    public Map<String, NEDElement> getParamValues() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        Map<String, NEDElement> result = 
            (info == null) ? new HashMap<String, NEDElement>() : new HashMap<String, NEDElement>(info.getParamValues());
        
        // add our own assigned parameters
        for (ParamNodeEx ownParam : getOwnParams()) 
            result.put(ownParam.getName(), ownParam);
        
        return result;
    }

    public Map<String, NEDElement> getParams() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        if (info == null)
            return new HashMap<String, NEDElement>();
        return info.getParams();
    }

    // gate support
    /**
     * @return All gates assigned in this submodule's body
     */
    public List<GateNodeEx> getOwnGates() {
        // FIXME does not include parameters in param groups !!!
        List<GateNodeEx> result = new ArrayList<GateNodeEx>();
        GatesNode gatesNode = getFirstGatesChild();
        if (gatesNode == null)
            return result;
        for(NEDElement currChild : gatesNode)
            if (currChild instanceof GateNodeEx)
                result.add((GateNodeEx)currChild);

        return result;
    }

    public Map<String, NEDElement> getGateSizes() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        Map<String, NEDElement> result = 
            (info == null) ? new HashMap<String, NEDElement>() : new HashMap<String, NEDElement>(info.getGateSizes());
        
        // add our own assigned parameters
        for (GateNodeEx ownGate : getOwnGates()) 
            result.put(ownGate.getName(), ownGate);
        
        return result;
    }

    public Map<String, NEDElement> getGates() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        if (info == null)
            return new HashMap<String, NEDElement>();
        return info.getGates();
    }

}
