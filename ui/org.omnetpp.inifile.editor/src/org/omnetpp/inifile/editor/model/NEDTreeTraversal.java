package org.omnetpp.inifile.editor.model;

import java.util.Stack;

import org.eclipse.core.resources.IProject;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;


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

	protected void doTraverse(SubmoduleElementEx module, INEDTypeInfo moduleEffectiveType) {
		// enter module
		visitedTypes.push(moduleEffectiveType);
		boolean recurse = visitor.enter(module, moduleEffectiveType);

		// traverse submodules
		if (recurse && moduleEffectiveType.getNEDElement() instanceof CompoundModuleElementEx) {
			for (SubmoduleElementEx submodule : moduleEffectiveType.getSubmodules().values()) {
				// dig out type info (NED declaration)
				String submoduleTypeName = resolveEffectiveTypeName(submodule);
				INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : resolver.lookupNedType(submoduleTypeName, submodule.getCompoundModule());

				// recursive call
				if (submoduleType == null)
					visitor.unresolvedType(submodule, submoduleTypeName);
				else if (visitedTypes.contains(submoduleType)) // cycle detection
					visitor.recursiveType(submodule, submoduleType);
				else
					doTraverse(submodule, submoduleType);
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
	protected String resolveEffectiveTypeName(SubmoduleElementEx submodule) {
		String submoduleTypeName = submodule.getType();
		if (StringUtils.isEmpty(submoduleTypeName)) {
		    // resolve "like" submodule
			submoduleTypeName = visitor.resolveLikeType(submodule);
			if (submoduleTypeName == null)
				submoduleTypeName = submodule.getLikeType();
			else {
			    // submoduleTypeName is likely an unqualified name -- look it up according to
			    // the "like" type name resolution rules
			    INEDTypeInfo interfaceType = resolver.lookupNedType(submodule.getLikeType(), submodule.getEnclosingLookupContext());
			    if (interfaceType != null) {
	                IProject context = resolver.getNedFile(submodule.getContainingNedFileElement()).getProject();
	                INEDTypeInfo actualType = resolver.lookupLikeType(submoduleTypeName, interfaceType, context);
			        if (actualType != null)
			            submoduleTypeName = actualType.getFullyQualifiedName();
			    }
			}
		}
		return submoduleTypeName;
	}
}
