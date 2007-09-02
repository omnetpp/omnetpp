package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SubmoduleElementEx extends SubmoduleElement
                            implements IConnectableElement, IHasIndex, IHasType,
                                       IHasParameters, IHasGates {
    public static final String DEFAULT_NAME = "unnamed";

    protected DisplayString displayString = null;

    protected SubmoduleElementEx() {
        init();
	}

    protected SubmoduleElementEx(INEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        setName(DEFAULT_NAME);
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getVectorSize() != null && !"".equals(getVectorSize()))
            result += "["+getVectorSize()+"]";
        return result;
    }

    @Override
    public void fireModelEvent(NEDModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NEDElementUtilEx.isDisplayStringUpToDate(displayString, this))
    		displayString = null;
    	super.fireModelEvent(event);
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
     * @param submoduleType  a CompoundModuleElementEx or a SimpleModuleElementEx
     */
    public DisplayString getDisplayString(IModuleTypeElement submoduleType) {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(submoduleType.getDisplayString());
    	return displayString;
    }

	/**
	 * Returns the compound module containing the definition of this connection
	 */
	public CompoundModuleElementEx getCompoundModule() {
        INEDElement parent = getParent();
        while (parent != null && !(parent instanceof CompoundModuleElementEx))
            parent = parent.getParent();
        return (CompoundModuleElementEx)parent;
	}

	// connection related methods

	/**
	 * Returns the list of all source connections that connect to this node and defined
	 * in the parent compound module. Connections defined in derived modules
	 * are NOT included here
	 */
	public List<ConnectionElementEx> getSrcConnections() {
		return getCompoundModule().getSrcConnectionsFor(getName());
	}

    /**
     * Returns the list of all connections that connect to this node and defined in the
     * parent compound module. Connections defined in derived modules are
     * NOT included here
     */
	public List<ConnectionElementEx> getDestConnections() {
		return getCompoundModule().getDestConnectionsFor(getName());
	}

	@Override
    public String debugString() {
        return "submodule: "+getName();
    }

    // type support
	public String getEffectiveType() {
		String likeType = getLikeType();
		return StringUtils.isEmpty(likeType) ? getType() : likeType;
	}

	public INEDTypeInfo getNEDTypeInfo() {
    	INEDTypeInfo typeInfo = resolveTypeName(getEffectiveType(), getCompoundModule());
    	INedTypeElement typeElement = typeInfo==null ? null : typeInfo.getNEDElement();
		return (typeElement instanceof IModuleTypeElement || typeElement instanceof ModuleInterfaceElementEx) ? typeInfo : null;
    }

    public INedTypeElement getEffectiveTypeRef() {
        INEDTypeInfo info = getNEDTypeInfo();
        return info == null ? null : info.getNEDElement();
    }


    /**
     * Returns the list of all parameters assigned in this submodule's body
     */
    public List<ParamElementEx> getOwnParams() {
        List<ParamElementEx> result = new ArrayList<ParamElementEx>();

        ParametersElement parametersElement = getFirstParametersChild();
        if (parametersElement != null)
        	for (INEDElement currChild : parametersElement)
        		if (currChild instanceof ParamElementEx)
        			result.add((ParamElementEx)currChild);

        return result;
    }

    // parameter query support
    
    public Map<String, ParamElementEx> getParamAssignments() {
        Map<String, ParamElementEx> result = new HashMap<String, ParamElementEx>();

        INEDTypeInfo info = getNEDTypeInfo();
        if (info != null)
        	result.putAll(info.getParamAssignments());
    	
        // add local parameter assignments
        for (ParamElementEx ownParam : getOwnParams())
            result.put(ownParam.getName(), ownParam);

        return result;
    }

    public Map<String, ParamElementEx> getParamDeclarations() {
        INEDTypeInfo info = getNEDTypeInfo();
        return info == null ? new HashMap<String, ParamElementEx>() : info.getParamDeclarations();
    }

    // gate support
    
    /**
     * Returns the list of all gates assigned in this submodule's body
     */
    public List<GateElementEx> getOwnGates() {
        List<GateElementEx> result = new ArrayList<GateElementEx>();

        GatesElement gatesElement = getFirstGatesChild();
        if (gatesElement != null)
        	for (INEDElement currChild : gatesElement)
        		if (currChild instanceof GateElementEx)
        			result.add((GateElementEx)currChild);
        
        return result;
    }

    public Map<String, GateElementEx> getGateSizes() {
        Map<String, GateElementEx> result = new HashMap<String, GateElementEx>();

        INEDTypeInfo info = getNEDTypeInfo();
        if (info != null)
        	result.putAll(info.getGateSizes());

        // add local gatesizes
        for (GateElementEx ownGate : getOwnGates())
            result.put(ownGate.getName(), ownGate);

        return result;
    }

    public Map<String, GateElementEx> getGateDeclarations() {
        INEDTypeInfo info = getNEDTypeInfo();
        return info == null ? new HashMap<String, GateElementEx>() : info.getGateDeclarations();
    }

}
