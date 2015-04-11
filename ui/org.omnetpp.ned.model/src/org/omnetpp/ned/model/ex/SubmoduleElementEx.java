/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasIndex;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Represents a submodule.
 *
 * @author rhornig, andras
 */
public class SubmoduleElementEx extends SubmoduleElement
    implements ISubmoduleOrConnection, IConnectableElement, IHasIndex, IHasGates
{
    private static final String PROP_DYNAMIC = "dynamic";

    public static final String DEFAULT_NAME = "unnamed";

    private INedTypeResolver resolver;
    protected DisplayString displayString = null;

    // cached return value of getNedTypeInfo()
    private INedTypeInfo cachedTypeInfo;

    // the value of INedTypeResolver.getLastChangeSerial() when cachedTypeInfo was calculated
    private long cachedTypeInfoSerial;

    protected SubmoduleElementEx(INedTypeResolver resolver) {
        init(resolver);
    }

    protected SubmoduleElementEx(INedTypeResolver resolver, INedElement parent) {
        super(parent);
        init(resolver);
    }

    private void init(INedTypeResolver resolver) {
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
        setName(DEFAULT_NAME);
    }

    public INedTypeResolver getResolver() {
        return resolver;
    }

    public String getNameWithIndex() {
        String result = getName();
        if (getVectorSize() != null && !"".equals(getVectorSize()))
            result += "["+getVectorSize()+"]";
        return result;
    }

    @Override
    public void fireModelEvent(NedModelEvent event) {
        // invalidate cached display string because NED tree may have changed outside the DisplayString class
        if (!NedElementUtilEx.isDisplayStringUpToDate(displayString, this))
            displayString = null;
        super.fireModelEvent(event);
    }

    public DisplayString getDisplayString() {
        if (displayString == null)
            displayString = new DisplayString(this, NedElementUtilEx.getDisplayStringLiteral(this));
        displayString.setFallbackDisplayString(NedElement.displayStringOf(getEffectiveTypeRef()));
        return displayString;
    }

    public DisplayString getDisplayString(INedTypeElement actualType) {
        if (displayString == null)
            displayString = new DisplayString(this, NedElementUtilEx.getDisplayStringLiteral(this));
        displayString.setFallbackDisplayString(actualType.getDisplayString());
        return displayString;
    }

    /**
     * Returns the compound module containing the definition of this submodule
     */
    public CompoundModuleElementEx getCompoundModule() {
        INedElement parent = getParent();
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

    public INedTypeInfo getNedTypeInfo() {
        if (cachedTypeInfoSerial != getResolver().getLastChangeSerial()) {
            // determine and cache typeInfo
            INedTypeInfo typeInfo = resolveTypeName(getEffectiveType(), getCompoundModule());
            INedTypeElement typeElement = typeInfo==null ? null : typeInfo.getNedElement();
            cachedTypeInfo = (typeElement instanceof IModuleKindTypeElement) ? typeInfo : null;
            cachedTypeInfoSerial = getResolver().getLastChangeSerial();
        }
        return cachedTypeInfo;
    }

    public INedTypeElement getEffectiveTypeRef() {
        INedTypeInfo info = getNedTypeInfo();
        return info == null ? null : info.getNedElement();
    }


    /**
     * Returns the list of all parameters assigned in this submodule's body
     */
    public List<ParamElementEx> getOwnParams() {
        List<ParamElementEx> result = new ArrayList<ParamElementEx>();

        ParametersElement parametersElement = getFirstParametersChild();
        if (parametersElement != null)
            for (INedElement currChild : parametersElement)
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
        return getParamAssignments(getNedTypeInfo());
    }

    /**
     * Returns parameter assignments of this submodule, including those in the NED
     * type it instantiates, assuming that the submodule's actual type is the
     * compound or simple module type passed in the <code>submoduleType</code>
     * parameter. This is useful when the submodule is a "like" submodule, and the
     * caller knows the actual submodule type (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamAssignments(INedTypeInfo submoduleType) {
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
        return getParamDeclarations(getNedTypeInfo());
    }

    /**
     * Returns parameter declarations of this submodule, assuming that the submodule's
     * actual type is the compound or simple module type passed in the
     * <code>submoduleType</code> parameter. This is useful when the submodule is
     * a "like" submodule, and the caller knows the actual submodule type
     * (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamDeclarations(INedTypeInfo submoduleType) {
        return submoduleType == null ? new HashMap<String, ParamElementEx>() : submoduleType.getParamDeclarations();
    }

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName) {
        INedTypeInfo typeInfo = getNedTypeInfo();
        List<ParamElementEx> chain = typeInfo == null ? new ArrayList<ParamElementEx>() : typeInfo.getParameterInheritanceChain(parameterName);

        for (ParamElementEx param : getOwnParams()) {
            if (parameterName.equals(param.getName())) {
                chain.add(0, param);
                break;
            }
        }

        return chain;
    }

    // gate support

    /**
     * Returns the list of all gates assigned in this submodule's body
     */
    public List<GateElementEx> getOwnGates() {
        List<GateElementEx> result = new ArrayList<GateElementEx>();

        GatesElement gatesElement = getFirstGatesChild();
        if (gatesElement != null)
            for (INedElement currChild : gatesElement)
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
        return getGateSizes(getNedTypeInfo());
    }

    /**
     * Returns gate size assignments of this submodule, including those in the NED
     * type it instantiates, assuming that the submodule's actual type is the
     * compound or simple module type passed in the <code>submoduleType</code>
     * parameter. This is useful when the submodule is a "like" submodule, and the
     * caller knows the actual submodule type (e.g. from an inifile).
     */
    public Map<String, GateElementEx> getGateSizes(INedTypeInfo moduleType) {
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
        return getGateDeclarations(getNedTypeInfo());
    }

    /**
     * Returns gate declarations of this submodule, assuming that the submodule's
     * actual type is the compound or simple module type passed in the
     * <code>submoduleType</code> parameter. This is useful when the submodule is
     * a "like" submodule, and the caller knows the actual submodule type
     * (e.g. from an inifile).
     */
    public Map<String, GateElementEx> getGateDeclarations(INedTypeInfo submoduleType) {
        return submoduleType == null ? new HashMap<String, GateElementEx>() : submoduleType.getGateDeclarations();
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        HashMap<String, Map<String, PropertyElementEx>> map = new HashMap<String, Map<String, PropertyElementEx>>();
        NedElementUtilEx.collectProperties(this, map);
        return map;
    }

    public boolean isDynamic() {
        PropertyElementEx property = getLocalProperty(PROP_DYNAMIC);
        return property != null && !"false".equals(property.getSimpleValue());

    }

    protected PropertyElementEx getLocalProperty(String name) {
        INedElement section = getFirstChildWithTag(NedElementTags.NED_PARAMETERS);
        if (section != null) {
            for (INedElement node : section) {
                if (node instanceof PropertyElementEx) {
                    PropertyElementEx property = (PropertyElementEx)node;
                    if (name.equals(property.getName()))
                        return property;
                }
            }
        }

        return null;
    }

    public void setIsDynamic(boolean value) {
        NedElementUtilEx.setBooleanProperty(this, PROP_DYNAMIC, value);
    }
}
