package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IHasType;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * TODO add documentation
 *
 * @author rhornig, andras
 */
public class SubmoduleElementEx extends SubmoduleElement
                            implements IConnectableElement, IHasIndex, IHasType,
                                       IHasParameters, IHasGates {
    public static final String DEFAULT_NAME = "unnamed";

    protected DisplayString displayString = null;

    // cached return value of getNedTypeInfo()
    private INEDTypeInfo cachedTypeInfo;

    // the value of INEDTypeResolver.getLastChangeSerial() when cachedTypeInfo was calculated
    private long cachedTypeInfoSerial; 

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
    	displayString.setFallbackDisplayString(NEDElement.displayStringOf(getEffectiveTypeRef()));
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
    public DisplayString getDisplayString(IModuleKindTypeElement submoduleType) {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(submoduleType.getDisplayString());
    	return displayString;
    }

	/**
	 * Returns the compound module containing the definition of this submodule
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
	    if (cachedTypeInfoSerial != getDefaultNedTypeResolver().getLastChangeSerial()) {
	        // determine and cache typeInfo
	        INEDTypeInfo typeInfo = resolveTypeName(getEffectiveType(), getCompoundModule());
	        INedTypeElement typeElement = typeInfo==null ? null : typeInfo.getNEDElement();
	        cachedTypeInfo = (typeElement instanceof IModuleKindTypeElement) ? typeInfo : null;
	        cachedTypeInfoSerial = getDefaultNedTypeResolver().getLastChangeSerial(); 
	    }
	    return cachedTypeInfo;	        
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

    /**
     * Returns parameter assignments of this submodule, including those in the NED
     * type it instantiates. For "like" submodules the actual submodule type is unknown,
     * so the interface NED type is used.  
     */
    public Map<String, ParamElementEx> getParamAssignments() {
        return getParamAssignments(getNEDTypeInfo());
    }
    
    /**
     * Returns parameter assignments of this submodule, including those in the NED
     * type it instantiates, assuming that the submodule's actual type is the 
     * compound or simple module type passed in the <code>submoduleType</code> 
     * parameter. This is useful when the submodule is a "like" submodule, and the
     * caller knows the actual submodule type (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamAssignments(INEDTypeInfo submoduleType) {
        Map<String, ParamElementEx> result = new HashMap<String, ParamElementEx>();

        if (submoduleType != null)
        	result.putAll(submoduleType.getParamAssignments());
    	
        // add local parameter assignments
        for (ParamElementEx ownParam : getOwnParams())
            result.put(ownParam.getName(), ownParam);

        return result;
    }

    /**
     * Returns parameter declarations of this submodule, including those in the NED
     * type it instantiates. For "like" submodules the actual submodule type is unknown,
     * so the interface NED type is used.  
     */
    public Map<String, ParamElementEx> getParamDeclarations() {
        return getParamDeclarations(getNEDTypeInfo());
    }

    /**
     * Returns parameter declarations of this submodule, assuming that the submodule's 
     * actual type is the compound or simple module type passed in the 
     * <code>submoduleType</code> parameter. This is useful when the submodule is 
     * a "like" submodule, and the caller knows the actual submodule type 
     * (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamDeclarations(INEDTypeInfo submoduleType) {
        return submoduleType == null ? new HashMap<String, ParamElementEx>() : submoduleType.getParamDeclarations();
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

    /**
     * Returns the gate size assignments for this submodule, including those in the NED
     * type it instantiates. For "like" submodules the actual submodule type is unknown,
     * so the interface NED type is used.  
     */
    public Map<String, GateElementEx> getGateSizes() {
        return getGateSizes(getNEDTypeInfo());
    }
    
    /**
     * Returns gate size assignments of this submodule, including those in the NED
     * type it instantiates, assuming that the submodule's actual type is the 
     * compound or simple module type passed in the <code>submoduleType</code> 
     * parameter. This is useful when the submodule is a "like" submodule, and the
     * caller knows the actual submodule type (e.g. from an inifile).
     */
    public Map<String, GateElementEx> getGateSizes(INEDTypeInfo moduleType) {
        Map<String, GateElementEx> result = new HashMap<String, GateElementEx>();

        if (moduleType != null)
        	result.putAll(moduleType.getGateSizes());

        // add local gatesizes
        for (GateElementEx ownGate : getOwnGates())
            result.put(ownGate.getName(), ownGate);

        return result;
    }

    /**
     * Returns the gate declarations for this submodule, including those in the NED
     * type it instantiates. For "like" submodules the actual submodule type is unknown,
     * so the interface NED type is used.  
     */
    public Map<String, GateElementEx> getGateDeclarations() {
        return getGateDeclarations(getNEDTypeInfo());
    }

    /**
     * Returns gate declarations of this submodule, assuming that the submodule's 
     * actual type is the compound or simple module type passed in the 
     * <code>submoduleType</code> parameter. This is useful when the submodule is 
     * a "like" submodule, and the caller knows the actual submodule type 
     * (e.g. from an inifile).
     */
    public Map<String, GateElementEx> getGateDeclarations(INEDTypeInfo submoduleType) {
        return submoduleType == null ? new HashMap<String, GateElementEx>() : submoduleType.getGateDeclarations();
    }

}
