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
import org.omnetpp.ned.model.interfaces.IChannelKindTypeElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ParametersElement;

/**
 * Represents a connection in a compound module.
 *
 * @author rhornig
 */
public class ConnectionElementEx extends ConnectionElement implements ISubmoduleOrConnection
{
    private INedTypeResolver resolver;
    protected DisplayString displayString = null;

    protected ConnectionElementEx(INedTypeResolver resolver) {
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
    }

    protected ConnectionElementEx(INedTypeResolver resolver, INedElement parent) {
        super(parent);
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
    }

    public INedTypeResolver getResolver() {
        return resolver;
    }

    public IConnectableElement getSrcModuleRef() {
        return resolveConnectedModule(getSrcModule());
    }

    public IConnectableElement getDestModuleRef() {
        return resolveConnectedModule(getDestModule());
    }

    /**
     * Sets the source module of the connection
     */
    public void setSrcModuleRef(IConnectableElement srcModule) {
        setSrcModule(connectedModuleName(srcModule));
    }

    /**
     * Sets the destination module of the connection
     */
    public void setDestModuleRef(IConnectableElement destModule) {
        setDestModule(connectedModuleName(destModule));
    }

    protected String connectedModuleName(IConnectableElement module) {
        return module == null ? null : (module instanceof CompoundModuleElementEx) ? "" : module.getName();
    }

    // helper functions to set the module names using references
    protected IConnectableElement resolveConnectedModule(String moduleName) {
        if (moduleName == null)
            return null; // not connected

        CompoundModuleElementEx compoundModule = getCompoundModule();
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
     * Returns the identifier of the source gate instance the connection is connected to
     */
    public String getSrcGateWithIndex() {
        return getGateNameWithIndex(getSrcGate(), getSrcGateSubg(), getSrcGateIndex(), getSrcGatePlusplus());
    }

    /**
     * Returns the identifier of the destination gate instance the connection is connected to
     */
    public String getDestGateWithIndex() {
        return getGateNameWithIndex(getDestGate(), getDestGateSubg(), getDestGateIndex(), getDestGatePlusplus());
    }

    public static String getGateNameWithIndex(String name, int subgate, String index, boolean isPlusPlus) {
        String gate = name;
        if (subgate == NED_SUBGATE_I) gate += "$i";
        if (subgate == NED_SUBGATE_O) gate += "$o";
        if (isPlusPlus) gate += "++";
        if (StringUtils.isNotEmpty(index)) gate += "["+index+"]";
        return gate;
    }

    /**
     * Returns the name of the source gate (gate name plus subgate, but without index)
     */
    public String getSrcGateEx() {
        return getGateName(getSrcGate(), getSrcGateSubg());
    }

    /**
     * Returns the name of the source gate (gate name plus subgate, but without index)
     */
    public String getDestGateEx() {
        return getGateName(getDestGate(), getDestGateSubg());
    }

    public static String getGateName(String name, int subgate) {
        String gate = name;
        if (subgate == NED_SUBGATE_I) gate += "$i";
        if (subgate == NED_SUBGATE_O) gate += "$o";
        return gate;
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
     * Returns the compound module containing this connection, or null if the
     * connection is not part of the model (i.e. has no compound module parent).
     */
    public CompoundModuleElementEx getCompoundModule() {
        return (CompoundModuleElementEx)getEnclosingTypeElement();
    }

    /**
     * Checks whether the current connection is valid (has valid submodules at both end)
     */
    public boolean isValid() {
        return getSrcModuleRef() != null && getDestModuleRef() != null;
    }

    // type management

    public String getEffectiveType() {
        String likeType = getLikeType();
        if (StringUtils.isEmpty(likeType)) {
            String type = getType();
            if (StringUtils.isEmpty(type)) {
                Map<String, ParamElementEx> paramAssignments = getParamAssignments(null);
                int delayChannelParameterCount = 0;
                if (paramAssignments.containsKey("delay"))
                    delayChannelParameterCount++;
                if (paramAssignments.containsKey("disabled"))
                    delayChannelParameterCount++;
                if (paramAssignments.size() == 0)
                    return "ned.IdealChannel";
                else if (paramAssignments.size() == delayChannelParameterCount)
                    return "ned.DelayChannel";
                else
                    return "ned.DatarateChannel";
            }
            else
                return getType();
        }
        else
            return likeType;
    }

    public INedTypeInfo getNedTypeInfo() {
        INedTypeInfo typeInfo = resolveTypeName(getEffectiveType(), getCompoundModule());
        INedTypeElement typeElement = typeInfo==null ? null : typeInfo.getNedElement();
        return (typeElement instanceof IChannelKindTypeElement) ? typeInfo : null;
    }

    public INedTypeElement getEffectiveTypeRef() {
        INedTypeInfo info = getNedTypeInfo();
        return info == null ? null : info.getNedElement();
    }

    /**
     * Returns a list of all parameters assigned in this module
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

    // Parameter query support. Note: code is similar to SubmoduleElementEx

    /**
     * Returns parameter assignments of this channel, including those in the NED
     * type it instantiates. For "like" channels the actual channel type is unknown,
     * so the interface NED type is used.
     */
    public Map<String, ParamElementEx> getParamAssignments() {
        return getParamAssignments(getNedTypeInfo());
    }

    /**
     * Returns parameter assignments of this channel, including those in the NED
     * type it instantiates, assuming that the channel's actual type is the
     * compound or simple module type passed in the <code>channelType</code>
     * parameter. This is useful when the channel is a "like" channel, and the
     * caller knows the actual channel type (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamAssignments(INedTypeInfo channelType) {
        Map<String, ParamElementEx> result = new HashMap<String, ParamElementEx>();

        if (channelType != null)
            result.putAll(channelType.getParamAssignments());

        // add local parameter assignments
        for (ParamElementEx ownParam : getOwnParams())
            result.put(ownParam.getName(), ownParam);

        return result;
    }

    /**
     * Returns parameter declarations of this channel, including those in the NED
     * type it instantiates. For "like" channels the actual channel type is unknown,
     * so the interface NED type is used.
     */
    public Map<String, ParamElementEx> getParamDeclarations() {
        return getParamDeclarations(getNedTypeInfo());
    }

    /**
     * Returns parameter declarations of this channel, assuming that the channel's
     * actual type is the compound or simple module type passed in the
     * <code>channelType</code> parameter. This is useful when the channel is
     * a "like" channel, and the caller knows the actual channel type
     * (e.g. from an inifile).
     */
    public Map<String, ParamElementEx> getParamDeclarations(INedTypeInfo channelType) {
        return channelType == null ? new HashMap<String, ParamElementEx>() : channelType.getParamDeclarations();
    }

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName) {
        List<ParamElementEx> chain = getNedTypeInfo().getParameterInheritanceChain(parameterName);

        for (ParamElementEx param : getOwnParams()) {
            if (parameterName.equals(param.getName())) {
                chain.add(0, param);
                break;
            }
        }

        return chain;
    }

    @Override
    public String toString() {
        String arrow = getIsBidirectional() ? "<-->" : "-->";
        String type = StringUtils.nullToEmpty(getType()) + " " + StringUtils.nullToEmpty(getLikeType());
        if (!StringUtils.isBlank(type))
            arrow += type + arrow;
        return getSrcGateFullyQualified() + arrow + getDestGateFullyQualified();
    }
}
