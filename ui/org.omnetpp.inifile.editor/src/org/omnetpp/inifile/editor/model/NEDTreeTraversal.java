package org.omnetpp.inifile.editor.model;

import java.util.Stack;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.SubmoduleNode;


/**
 * Traverses NED module type usage hierarchy.
 * @author Andras
 */
public class NEDTreeTraversal {
	private INEDTypeResolver nedResources;
	private IModuleTreeVisitor visitor;
	private Stack<INEDTypeInfo> visitedTypes = new Stack<INEDTypeInfo>(); // circle detection

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
	public void traverse(String moduleTypeName) {
		INEDTypeInfo moduleType = nedResources.getComponent(moduleTypeName);
		if (moduleType==null)
			visitor.unresolvedType(null, moduleTypeName);
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
	public void traverse(SubmoduleNode submodule) {
		visitedTypes.clear();
		String submoduleTypeName = resolveTypeName(submodule);
		INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : nedResources.getComponent(submoduleTypeName);
		if (submoduleType == null)
			visitor.unresolvedType(submodule, submoduleTypeName);
		else
			doTraverse(submodule, submoduleType);
	}

	protected void doTraverse(SubmoduleNode module, INEDTypeInfo moduleType) {
		// enter module
		visitedTypes.push(moduleType);
		visitor.enter(module, moduleType);

		// traverse submodules
		for (INEDElement node : moduleType.getSubmodules().values()) {
			// dig out type info (NED declaration)
			SubmoduleNode submodule = (SubmoduleNode) node;
			String submoduleTypeName = resolveTypeName(submodule);
			INEDTypeInfo submoduleType = StringUtils.isEmpty(submoduleTypeName) ? null : nedResources.getComponent(submoduleTypeName);

			// recursive call
			if (submoduleType == null)
				visitor.unresolvedType(submodule, submoduleTypeName);
			else if (visitedTypes.contains(submoduleType)) // circle detection
				visitor.recursiveType(submodule, submoduleType);
			else
				doTraverse(submodule, submoduleType);
		}

		// leave module
		visitor.leave();
		visitedTypes.pop();
	}

	protected String resolveTypeName(SubmoduleNode submodule) {
		String submoduleTypeName = submodule.getType();
		if (StringUtils.isEmpty(submoduleTypeName)) {
			submoduleTypeName = visitor.resolveLikeType(submodule);
			if (submoduleTypeName==null) {
				submoduleTypeName = submodule.getLikeType(); //XXX why???? use the interface if actual module type is not available
			}
		}
		return submoduleTypeName;
	}
}
