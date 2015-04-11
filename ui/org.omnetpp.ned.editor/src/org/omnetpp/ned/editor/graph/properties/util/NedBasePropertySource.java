/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver.IPredicate;

/**
 * A property source that is the base of all ned element related property sources
 *
 * @author rhornig
 */
abstract public class NedBasePropertySource implements IPropertySource2 {
    private final INedElement nedModel;

    public NedBasePropertySource(INedElement model) {
        nedModel = model;
    }

    public Object getEditableValue() {
        return "";
    }

    public Object getPropertyValue(Object id) {
        return "";
    }

    public boolean isPropertyResettable(Object id) {
        return false;
    }

    public boolean isPropertySet(Object id) {
        return false;
    }

    public void resetPropertyValue(Object id) {
    }

    public void setPropertyValue(Object id, Object value) {
    }

    public abstract IPropertyDescriptor[] getPropertyDescriptors();

    INedElement getModel() {
        return nedModel;
    }

    /**
     * Turns a fully qualified type name into a display friendly "Name (package)" format.
     * Unqualified names come out unchanged
     */
    public static String convertQNameToDisplayName(String qname) {
        if (qname==null || !qname.contains("."))
            return qname;

        return StringUtils.substringAfterLast(qname, ".")+" ("+StringUtils.substringBeforeLast(qname, ".")+")";
    }

    /**
     * Returns all possible type values in a display friendly format "Name (packagename)"
     * that can be used in a given element for the type, likeType or extends attribute.
     * Removes the name corresponding to the element (to eliminate self recursion)
     * and also the name of the compound module containing the element (for submodules and inner types),
     * because that would cause loops in the type hierarchy.
     * Predicate can specify what kind of types to return (INedResources.MODULE_FILTER etc.)
     */
    public List<String> getPossibleTypeDisplayNames(INedElement element, IPredicate predicate) {
        INedResources res = NedResourcesPlugin.getNedResources();
        IProject project = res.getNedFile(element.getContainingNedFileElement()).getProject();
        List<String> typeNames = new ArrayList<String>(res.getToplevelNedTypeQNames(predicate, project));
        // if the context is a module (type list was requested for inner type, submodule or connection)
        INedTypeLookupContext context = element.getEnclosingLookupContext();
        if (context instanceof CompoundModuleElementEx) {
            typeNames.addAll(res.getLocalTypeNames(context, predicate));
            // remove the containing type
            typeNames.remove(((CompoundModuleElementEx)context).getNedTypeInfo().getFullyQualifiedName());
        }

        // remove the name of the element itself to avoid self recursion
        if (element instanceof INedTypeElement)
            typeNames.remove(((INedTypeElement)element).getNedTypeInfo().getFullyQualifiedName());

        // convert to display friendly names
        for(int i=0; i<typeNames.size(); ++i)
            typeNames.set(i, convertQNameToDisplayName(typeNames.get(i)));

        Collections.sort(typeNames);
        return typeNames;
    }

    /**
     * Returns the fully qualified name using the provided context from the simple name.
     * If the lookup is unsuccessful it returns the name unchanged.
     */
    public String lookupFullyQualifiedName(INedTypeLookupContext context, String name) {
        INedTypeInfo typeInfo = NedElement.resolveTypeName(name, context);
        if (typeInfo != null)
            name = typeInfo.getFullyQualifiedName();
        return name;
    }
}
