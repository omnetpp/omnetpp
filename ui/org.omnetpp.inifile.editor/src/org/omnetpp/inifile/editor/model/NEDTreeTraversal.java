package org.omnetpp.inifile.editor.model;

import java.util.Stack;

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
	private INEDTypeResolver nedResources;
	private IModuleTreeVisitor visitor;
	private Stack<INEDTypeInfo> visitedTypes = new Stack<INEDTypeInfo>(); // cycle detection

	/**
	 * Constructor
	 */
	public NEDTreeTraversal(INEDTypeResolver nedResources, IModuleTreeVisitor visitor) {
		this.nedResources = nedResources;
		this.visitor = visitor;
	}

	/**
	 * Traverse the module usage hierarchy, and call methods for the visitor.
	 */
	public void traverse(String fullyQualifiedModuleTypeName) {
		INEDTypeInfo moduleType = nedResources.getToplevelOrInnerNedType(fullyQualifiedModuleTypeName);
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
		INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : nedResources.lookupNedType(submoduleTypeName, submodule.getCompoundModule());
		if (submoduleType == null)
			visitor.unresolvedType(submodule, submoduleTypeName);
		else
			doTraverse(submodule, submoduleType);
	}

	protected void doTraverse(SubmoduleElementEx module, INEDTypeInfo moduleEffectiveType) {
		// enter module
		visitedTypes.push(moduleEffectiveType);
		visitor.enter(module, moduleEffectiveType);

		// traverse submodules
		if (moduleEffectiveType.getNEDElement() instanceof CompoundModuleElementEx) {
			CompoundModuleElementEx moduleAsCompound = (CompoundModuleElementEx)moduleEffectiveType.getNEDElement();
			for (SubmoduleElementEx submodule : moduleEffectiveType.getSubmodules().values()) {
				// dig out type info (NED declaration)
				String submoduleTypeName = resolveEffectiveTypeName(submodule);
				INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : nedResources.lookupNedType(submoduleTypeName, moduleAsCompound);

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
	 */
	protected String resolveEffectiveTypeName(SubmoduleElementEx submodule) {
		String submoduleTypeName = submodule.getType();
		if (StringUtils.isEmpty(submoduleTypeName)) {
			submoduleTypeName = visitor.resolveLikeType(submodule);
			if (submoduleTypeName == null)
				submoduleTypeName = submodule.getLikeType();
		}
		return submoduleTypeName;
	}
}
