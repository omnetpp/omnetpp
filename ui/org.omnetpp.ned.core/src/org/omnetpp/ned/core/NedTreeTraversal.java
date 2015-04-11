/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import org.eclipse.core.resources.IProject;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;


/**
 * Traverses NED module type usage hierarchy.
 * @author Andras
 */
public class NedTreeTraversal {
    private INedTypeResolver resolver;
    private IModuleTreeVisitor visitor;
    private IProject contextProject;  // for "like" type lookups
    private Stack<INedTypeInfo> visitedTypes = new Stack<INedTypeInfo>(); // cycle detection

    /**
     * Constructor
     */
    public NedTreeTraversal(INedTypeResolver resolver, IModuleTreeVisitor visitor, IProject contextProject) {
        this.resolver = resolver;
        this.visitor = visitor;
        this.contextProject = contextProject;
    }

    /**
     * Traverse the module usage hierarchy, and call methods for the visitor.
     */
    public void traverse(String fullyQualifiedModuleTypeName) {
        INedTypeInfo moduleType = resolver.getToplevelOrInnerNedType(fullyQualifiedModuleTypeName, contextProject);
        if (moduleType==null)
            visitor.unresolvedType(null, fullyQualifiedModuleTypeName);
        else
            traverse(moduleType);
    }

    /**
     * Traverse the module usage hierarchy, and call methods for the visitor.
     */
    public void traverse(INedTypeInfo moduleType) {
        visitedTypes.clear();
        doTraverse(null, moduleType);
    }

    /**
     * Traverse the module usage hierarchy, and call methods for the visitor.
     */
    public void traverse(ISubmoduleOrConnection element) {
        visitedTypes.clear();
        INedTypeInfo elementType = resolveEffectiveType(element, element.getEnclosingLookupContext());
        if (elementType != null)
            doTraverse(element, elementType);
    }

    protected void doTraverse(ISubmoduleOrConnection element, INedTypeInfo effectiveTypeInfo) {
        // enter module
        visitedTypes.push(effectiveTypeInfo);
        boolean recurse = visitor.enter(element, effectiveTypeInfo);

        // traverse submodules
        if (recurse && effectiveTypeInfo.getNedElement() instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)effectiveTypeInfo.getNedElement();

            List<SubmoduleElementEx> submodules = compoundModule.getSubmodules();
            ArrayList<ISubmoduleOrConnection> elements = new ArrayList<ISubmoduleOrConnection>();
            elements.addAll(submodules);
            elements.addAll(compoundModule.getSrcConnections());

            for (SubmoduleElementEx submodule : submodules)
                elements.addAll(compoundModule.getSrcConnectionsFor(submodule.getName()));

            for (ISubmoduleOrConnection childElement : elements) {
                // dig out type info (NED declaration)
                INedTypeInfo typeInfo = resolveEffectiveType(childElement, compoundModule);

                // recursive call
                if (typeInfo != null) {
                    if (visitedTypes.contains(typeInfo)) // cycle detection
                        visitor.recursiveType(childElement, typeInfo);
                    else
                        doTraverse(childElement, typeInfo);
                }
            }
        }

        // leave module
        visitor.leave();
        visitedTypes.pop();
    }

    /**
     * Return the actual type of a submodule or connection. If it is a concrete type,
     * it is looked up in the compound module as context (using imports, inner types, etc).
     * If it is a parametric type ("<param> like IInterface"), we obtain the value for
     * the parameter using visitor.resolveLikeType(), and choose a module with that
     * name from the ones that implement the given interface.
     *
     * The return value may be null ("could not be resolved"); in that case this method
     * already calls visitor.unresolvedType().
     */
    protected INedTypeInfo resolveEffectiveType(ISubmoduleOrConnection element, INedTypeLookupContext compoundModule) {
        INedTypeInfo result;
        if (StringUtils.isEmpty(element.getLikeType())) {
            result = element.getNedTypeInfo();  // note: this resolves connections with getType()==null as well (to ned.IdealChannel)
            if (result == null)
                visitor.unresolvedType(element, element.getType());
        }
        else
        {
            // resolve "like" type
            INedTypeInfo interfaceType = resolver.lookupNedType(element.getLikeType(), element.getEnclosingLookupContext());
            if (interfaceType == null) {
                visitor.unresolvedType(element, element.getLikeType()); // undefined interface
                result = null;
            }
            else {
                String effectiveTypeName = visitor.resolveLikeType(element);  // value of the "like" parameter
                if (effectiveTypeName == null) {
                    result = interfaceType; // no actual type given, return the interface
                }
                else {
                    // effectiveTypeName is likely unqualified -- look it up according to the "like" type name resolution rules
                    result = resolver.lookupLikeType(effectiveTypeName, interfaceType, contextProject); // actual type
                    if (result == null)
                        visitor.unresolvedType(element, effectiveTypeName); // no such type
                }
            }
        }
        return result;
    }

}
