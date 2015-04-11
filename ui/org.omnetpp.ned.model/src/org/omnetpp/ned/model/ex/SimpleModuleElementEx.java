/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;

/**
 * Represents a simple module type
 *
 * @author rhornig
 */
public class SimpleModuleElementEx extends SimpleModuleElement implements IModuleTypeElement {

    private INedTypeResolver resolver;
    private INedTypeInfo typeInfo;
    protected DisplayString displayString = null;

    protected SimpleModuleElementEx(INedTypeResolver resolver) {
        init(resolver);
    }

    protected SimpleModuleElementEx(INedTypeResolver resolver, INedElement parent) {
        super(parent);
        init(resolver);
    }

    private void init(INedTypeResolver resolver) {
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
        typeInfo = getResolver().createTypeInfoFor(this);
        setName("Unnamed");
    }

    public INedTypeResolver getResolver() {
        return resolver;
    }

    public INedTypeInfo getNedTypeInfo() {
        return typeInfo;
    }

    @Override
    public void fireModelEvent(NedModelEvent event) {
        // invalidate cached display string because NED tree may have changed outside the DisplayString class
        if (!NedElementUtilEx.isDisplayStringUpToDate(displayString, this))
            displayString = null;
        super.fireModelEvent(event);
    }

    public boolean isNetwork() {
        // this isNetwork property should not be inherited so we look only among the local properties
        PropertyElementEx networkPropertyElementEx = getNedTypeInfo().getLocalProperty(IS_NETWORK_PROPERTY, null);
        if (networkPropertyElementEx == null)
            return false;
        String propValue = NedElementUtilEx.getPropertyValue(networkPropertyElementEx);
        return !StringUtils.equalsIgnoreCase("false", propValue);
    }

    public void setIsNetwork(boolean value) {
        NedElementUtilEx.setBooleanProperty(this, IModuleTypeElement.IS_NETWORK_PROPERTY, value);
    }

    public DisplayString getDisplayString() {
        if (displayString == null)
            displayString = new DisplayString(this, NedElementUtilEx.getDisplayStringLiteral(this));
        displayString.setFallbackDisplayString(NedElement.displayStringOf(getSuperType()));
        return displayString;
    }

    // "extends" support
    public String getFirstExtends() {
        return NedElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NedElementUtilEx.setFirstExtends(this, ext);
    }

    public INedTypeElement getSuperType() {
        return getNedTypeInfo().getSuperType();
    }

    public List<ExtendsElement> getAllExtends() {
        return getAllExtendsFrom(this);
    }

    public INedTypeLookupContext getParentLookupContext() {
        return getParentLookupContextFor(this);
    }

    // parameter query support

    public Map<String, ParamElementEx> getParamAssignments() {
        return getNedTypeInfo().getParamAssignments();
    }

    public Map<String, ParamElementEx> getParamDeclarations() {
        return getNedTypeInfo().getParamDeclarations();
    }

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName) {
        return getNedTypeInfo().getParameterInheritanceChain(parameterName);
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        return getNedTypeInfo().getProperties();
    }

    // gate support
    public Map<String, GateElementEx> getGateSizes() {
        return getNedTypeInfo().getGateSizes();
    }

    public Map<String, GateElementEx> getGateDeclarations() {
        return getNedTypeInfo().getGateDeclarations();
    }

    public Set<INedTypeElement> getLocalUsedTypes() {
        return getNedTypeInfo().getLocalUsedTypes();
    }
}
