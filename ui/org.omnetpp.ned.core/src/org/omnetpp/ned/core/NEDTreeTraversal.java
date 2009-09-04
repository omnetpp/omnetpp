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
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;


/**
 * Traverses NED module type usage hierarchy.
 * @author Andras
 */
public class NEDTreeTraversal {
	private INEDTypeResolver resolver;
	private IModuleTreeVisitor visitor;
	private Stack<INEDTypeInfo> visitedTypes = new Stack<INEDTypeInfo>(); // cycle detection

	/**
	 * Constructor
	 */
	public NEDTreeTraversal(INEDTypeResolver resolver, IModuleTreeVisitor visitor) {
		this.resolver = resolver;
		this.visitor = visitor;
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public void traverse(String fullyQualifiedModuleTypeName, IProject contextProject) {
		INEDTypeInfo moduleType = resolver.getToplevelOrInnerNedType(fullyQualifiedModuleTypeName, contextProject);
		if (moduleType==null)
			visitor.unresolvedType(null, fullyQualifiedModuleTypeName);
		else
			traverse(moduleType);
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public void traverse(INEDTypeInfo moduleType) {
		visitedTypes.clear();
		doTraverse(null, moduleType);
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public void traverse(SubmoduleElementEx submodule) {
		visitedTypes.clear();
		String submoduleTypeName = resolveEffectiveTypeName(submodule);
		INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : resolver.lookupNedType(submoduleTypeName, submodule.getCompoundModule());
		if (submoduleType == null)
			visitor.unresolvedType(submodule, submoduleTypeName);
		else
			doTraverse(submodule, submoduleType);
	}

	protected void doTraverse(ISubmoduleOrConnection element, INEDTypeInfo effectiveTypeInfo) {
		// enter module
		visitedTypes.push(effectiveTypeInfo);
		boolean recurse = visitor.enter(element, effectiveTypeInfo);

		// traverse submodules
		if (recurse && effectiveTypeInfo.getNEDElement() instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx compoundModul = (CompoundModuleElementEx)effectiveTypeInfo.getNEDElement();
            
            List<SubmoduleElementEx> submodules = compoundModul.getSubmodules();
            ArrayList<ISubmoduleOrConnection> elements = new ArrayList<ISubmoduleOrConnection>();
            elements.addAll(submodules);
            elements.addAll(compoundModul.getSrcConnections());
            
            for (SubmoduleElementEx submodule : submodules)
                elements.addAll(compoundModul.getSrcConnectionsFor(submodule.getName()));

			for (ISubmoduleOrConnection childElement : elements) {
				// dig out type info (NED declaration)
				String typeName = resolveEffectiveTypeName(childElement);
				INEDTypeInfo typeInfo = StringUtils.isEmpty(typeName) ? null : resolver.lookupNedType(typeName, compoundModul);

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
		    if (element instanceof ConnectionElementEx)
		        // TODO: KLUDGE: this is not obviously the correct solution (but might be)
		        //       connection element's type is null when it is an net.IdealChannel
		        //       to avoid printing its type name at certain places.
		        //       This causes that a couple of functions should deal with the type
		        //       being null instead of always getting a type name.
		        return "ned.IdealChannel";

		    // resolve "like" type
			typeName = visitor.resolveLikeType(element);
			if (typeName == null)
				typeName = element.getLikeType();
			else {
			    // submoduleTypeName is likely an unqualified name -- look it up according to
			    // the "like" type name resolution rules
			    INEDTypeInfo interfaceType = resolver.lookupNedType(element.getLikeType(), element.getEnclosingLookupContext());
			    if (interfaceType != null) {
	                IProject context = resolver.getNedFile(element.getContainingNedFileElement()).getProject();
	                INEDTypeInfo actualType = resolver.lookupLikeType(typeName, interfaceType, context);
			        if (actualType != null)
			            typeName = actualType.getFullyQualifiedName();
			    }
			}
		}
		return typeName;
	}
}
