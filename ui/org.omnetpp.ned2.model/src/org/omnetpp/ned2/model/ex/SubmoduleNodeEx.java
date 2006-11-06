package org.omnetpp.ned2.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.IGateContainer;
import org.omnetpp.ned2.model.interfaces.IIndexable;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.interfaces.INamed;
import org.omnetpp.ned2.model.interfaces.INamedGraphNode;
import org.omnetpp.ned2.model.interfaces.IParametrized;
import org.omnetpp.ned2.model.interfaces.IStringTyped;
import org.omnetpp.ned2.model.pojo.GatesNode;
import org.omnetpp.ned2.model.pojo.ParametersNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode
                            implements INamedGraphNode, IIndexable, IStringTyped, 
                                       IParametrized, IGateContainer {

    protected DisplayString displayString = null;

	SubmoduleNodeEx() {
        init();
	}

	SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(INamed.INITIAL_NAME);
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
        String typeName = getType();
        INEDTypeInfo typeInfo = getContainerNEDTypeInfo(); 
        if ( typeName == null || "".equals(typeName) || typeInfo == null)
            return null;

        return typeInfo.getResolver().getComponent(typeName);
    }

    public NEDElement getTypeRef() {
        INEDTypeInfo it = getTypeNEDTypeInfo();
        return it == null ? null : it.getNEDElement();
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
