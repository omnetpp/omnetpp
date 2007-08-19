package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.interfaces.IModuleTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INamedGraphNode;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.GatesNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.ParametersNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SubmoduleNodeEx extends SubmoduleNode
                            implements INamedGraphNode, IHasIndex, IHasType,
                                       IHasParameters, IHasGates {
    public static final String DEFAULT_TYPE = "Unknown";
    public static final String DEFAULT_NAME = "unnamed";

    protected DisplayString displayString = null;

    protected SubmoduleNodeEx() {
        init();
	}

    protected SubmoduleNodeEx(INEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(DEFAULT_NAME);
        setType(DEFAULT_TYPE);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getVectorSize() != null && !"".equals(getVectorSize()))
            result += "["+getVectorSize()+"]";
        return result;
    }

    @Override
    public void setName(String val) {
        if (getCompoundModule() != null) {
        	// if a submodule name has changed we must change all the connections in the same compound module
        	// that is attached to this module (so the model will remain consistent)
            for (ConnectionNodeEx conn : getCompoundModule().getSrcConnectionsFor(getName()))
                conn.setSrcModule(val);
            for (ConnectionNodeEx conn : getCompoundModule().getDestConnectionsFor(getName()))
                conn.setDestModule(val);
        }
        super.setName(val);
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
     * Returns the display string for this submodule, assuming
     * that the submodule's actual type is the compound or simple module type
     * passed in the <code>submoduleType</code> parameter. This is useful
     * when the submodule is a "like" submodule, whose the actual submodule
     * type (not the <code>likeType</code>) is known. The latter usually
     * comes from an ini file or some other source outside the INEDElement tree.
     * Used within the inifile editor.
     *
     * @param submoduleType  a CompoundModuleNodeEx or a SimpleModuleNodeEx
     */
    public DisplayString getDisplayString(IModuleTypeNode submoduleType) {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(submoduleType.getDisplayString());
    	return displayString;
    }

	/**
	 * Returns the compound module containing the definition of this connection
	 */
	public CompoundModuleNodeEx getCompoundModule() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof CompoundModuleNodeEx))
            parent = parent.getParent();
        return (CompoundModuleNodeEx)parent;
	}

	// connection related methods

	/**
	 * Returns the list of all source connections that connect to this node and defined
	 * in the parent compound module. connections defined in derived modules
	 * are NOT included here
	 */
	public List<ConnectionNodeEx> getSrcConnections() {
		return getCompoundModule().getSrcConnectionsFor(getName());
	}

    /**
     * Returns the list of all connections that connect to this node and defined in the
     * parent compound module. connections defined in derived modules are
     * NOT included here
     */
	public List<ConnectionNodeEx> getDestConnections() {
		return getCompoundModule().getDestConnectionsFor(getName());
	}

	@Override
    public String debugString() {
        return "submodule: "+getName();
    }

    // type support
    public INEDTypeInfo getTypeNEDTypeInfo() {
    	return resolveTypeName(getEffectiveType());
    }

    public INedTypeNode getEffectiveTypeRef() {
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
     * Returns the list of all parameters assigned in this submodule's body
     */
    public List<ParamNodeEx> getOwnParams() {
        List<ParamNodeEx> result = new ArrayList<ParamNodeEx>();
        ParametersNode parametersNode = getFirstParametersChild();
        if (parametersNode == null)
            return result;
        for (INEDElement currChild : parametersNode)
            if (currChild instanceof ParamNodeEx)
                result.add((ParamNodeEx)currChild);

        return result;
    }

    // parameter query support
    public Map<String, ParamNode> getParamValues() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        Map<String, ParamNode> result =
            (info == null) ? new HashMap<String, ParamNode>() : new HashMap<String, ParamNode>(info.getParamValues());

        // add our own assigned parameters
        for (ParamNodeEx ownParam : getOwnParams())   //FIXME what are we doing here???
            result.put(ownParam.getName(), ownParam);

        return result;
    }

    public Map<String, ParamNode> getParams() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        if (info == null)
            return new HashMap<String, ParamNode>(); //FIXME why lie???
        return info.getParams();
    }

    // gate support
    /**
     * Returns the list of all gates assigned in this submodule's body
     */
    public List<GateNodeEx> getOwnGates() {
        List<GateNodeEx> result = new ArrayList<GateNodeEx>();
        GatesNode gatesNode = getFirstGatesChild();
        if (gatesNode == null)
            return result;
        for (INEDElement currChild : gatesNode)
            if (currChild instanceof GateNodeEx)
                result.add((GateNodeEx)currChild);

        return result;
    }

    public Map<String, GateNode> getGateSizes() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        Map<String, GateNode> result =
            (info == null) ? new HashMap<String, GateNode>() : new HashMap<String, GateNode>(info.getGateSizes());

        // add our own assigned parameters
        for (GateNodeEx ownGate : getOwnGates()) //FIXME WTF is this???? why not from typeINfo???? Andras
            result.put(ownGate.getName(), ownGate);

        return result;
    }

    public Map<String, GateNode> getGates() {
        INEDTypeInfo info = getTypeNEDTypeInfo();
        if (info == null)
            return new HashMap<String, GateNode>(); //FIXME this check MUST BE THROWN OUT!!! WE SHOULS RATHER ASSERT THAT THE TYPE INFO EXISTS!!!!!
        return info.getGates();
    }

}
