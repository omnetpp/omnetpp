/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;


/**
 * Traverses NED module type usage hierarchy.
 * @author Andras
 */
public class NedTreeTraversal {
	private INedTypeResolver resolver;
	private IModuleTreeVisitor visitor;
	private Stack<INedTypeInfo> visitedTypes = new Stack<INedTypeInfo>(); // cycle detection

	/**
	 * Constructor
	 */
	public NedTreeTraversal(INedTypeResolver resolver, IModuleTreeVisitor visitor) {
		this.resolver = resolver;
		this.visitor = visitor;
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public void traverse(String fullyQualifiedModuleTypeName, IProject contextProject) {
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
		String elementTypeName = resolveEffectiveTypeName(element);
		INedTypeInfo elementType = StringUtils.isEmpty(elementTypeName) ? null : resolver.lookupNedType(elementTypeName, element.getEnclosingLookupContext());
		if (elementType == null)
			visitor.unresolvedType(element, elementTypeName);
		else
			doTraverse(element, elementType);
	}

	protected void doTraverse(ISubmoduleOrConnection element, INedTypeInfo effectiveTypeInfo) {
		// enter module
		visitedTypes.push(effectiveTypeInfo);
		boolean recurse = visitor.enter(element, effectiveTypeInfo);

		// traverse submodules
		if (recurse && effectiveTypeInfo.getNedElement() instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx compoundModul = (CompoundModuleElementEx)effectiveTypeInfo.getNedElement();

            List<SubmoduleElementEx> submodules = compoundModul.getSubmodules();
            ArrayList<ISubmoduleOrConnection> elements = new ArrayList<ISubmoduleOrConnection>();
            elements.addAll(submodules);
            elements.addAll(compoundModul.getSrcConnections());

            for (SubmoduleElementEx submodule : submodules)
                elements.addAll(compoundModul.getSrcConnectionsFor(submodule.getName()));

			for (ISubmoduleOrConnection childElement : elements) {
				// dig out type info (NED declaration)
				String typeName = resolveEffectiveTypeName(childElement);
				INedTypeInfo typeInfo = StringUtils.isEmpty(typeName) ? null : resolver.lookupNedType(typeName, compoundModul);

				// recursive call
				if (typeInfo == null)
					visitor.unresolvedType(childElement, typeName);
				else if (visitedTypes.contains(typeInfo)) // cycle detection
					visitor.recursiveType(childElement, typeInfo);
				else
					doTraverse(childElement, typeInfo);
			}
		}

		// leave module
		visitor.leave();
		visitedTypes.pop();
	}

	/**
	 * The returned type name can be null or empty, and can be a short name which
	 * needs to be looked up in the enclosing compound module's context.
	 * However, if the module is a "like" submodule, resolving the type to fully
	 * qualified name needs to be done here.
	 */
	protected String resolveEffectiveTypeName(ISubmoduleOrConnection element) {
		String typeName = element.getType();
		if (StringUtils.isEmpty(typeName)) {
		    // resolve "like" type
			typeName = visitor.resolveLikeType(element);
			if (typeName == null)
				typeName = element.getEffectiveType();
			else {
			    // submoduleTypeName is likely an unqualified name -- look it up according to
			    // the "like" type name resolution rules
			    INedTypeInfo interfaceType = resolver.lookupNedType(element.getLikeType(), element.getEnclosingLookupContext());
			    if (interfaceType != null) {
	                IProject context = resolver.getNedFile(element.getContainingNedFileElement()).getProject();
	                INedTypeInfo actualType = resolver.lookupLikeType(typeName, interfaceType, context);
			        if (actualType != null)
			            typeName = actualType.getFullyQualifiedName();
			    }
			}
		}
		return typeName;
	}
}
