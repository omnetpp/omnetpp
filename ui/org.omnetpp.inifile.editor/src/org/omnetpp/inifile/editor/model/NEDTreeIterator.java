package org.omnetpp.inifile.editor.model;

import java.util.Stack;

import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.pojo.SubmoduleNode;


/**
 * Traverses NED module type usage hierarchy.
 * @author Andras
 */
public class NEDTreeIterator {
	private INEDTypeResolver nedResources;
	private IModuleTreeVisitor visitor;
	private Stack<INEDTypeInfo> visitedTypes = new Stack<INEDTypeInfo>(); // circle detection
		
	/**
	 * Visitor interface.
	 */
	public static interface IModuleTreeVisitor {
		/**
		 * Enter a submodule or start recursion
		 * @param submodule      null at the root only
		 * @param submoduleType  type of the submodule or root type, never null
		 */
		void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType);
		
		/**
		 * Leave the module last entered.
		 */
		void leave();
		
		/**
		 * Encountered a submodule whose module type is unresolved. No leave() will follow.
		 */
		void unresolvedType(SubmoduleNode submodule, String submoduleTypeName);

		/**
		 * Encountered a submodule which introduces type recursion, and traversal is
		 * pruned here. No leave() will follow.
		 */
		void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType);
	
		/**
		 * Resolve the "like" parameter of a submodule. The method should return the
		 * actual (resolved) module type, or null if it cannot (or does not wish to) 
		 * resolve it. 
		 */
		String resolveLikeType(SubmoduleNode submodule);
	}

	/**
	 * Example visitor 1: builds a tree
	 */
	public static class TreeBuilder implements IModuleTreeVisitor {
		private GenericTreeNode root = new GenericTreeNode("root");
		private GenericTreeNode current = root;
		public void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
			GenericTreeNode child = new GenericTreeNode("("+submoduleType.getName()+")"+(submodule==null ? "" : submodule.getName()));
			current.addChild(child);
			current = child;
		}
		public void leave() {
			current = current.getParent();
		}
		public void unresolvedType(SubmoduleNode submodule, String submoduleTypeName) {
		}
		public void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
		}
		public String resolveLikeType(SubmoduleNode submodule) {
			return null;
		}
		public GenericTreeNode getResult() {
			return root;
		}
	}

	/**
	 * Example visitor 2: prints module full paths
	 */
	public static class FullPathBuilder implements IModuleTreeVisitor {
		Stack<String> fullPath = new Stack<String>();
		public void enter(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
			//fullPath.push("("+submoduleType.getName()+")"+(submodule==null ? "" : submodule.getName()));
			fullPath.push(submodule==null ? submoduleType.getName() : submodule.getName());
			System.out.println(StringUtils.join(fullPath.toArray(), "."));
		}
		public void leave() {
			fullPath.pop();
		}
		public void unresolvedType(SubmoduleNode submodule, String submoduleTypeName) {
		}
		public void recursiveType(SubmoduleNode submodule, INEDTypeInfo submoduleType) {
		}
		public String resolveLikeType(SubmoduleNode submodule) {
			return null;
		}
	}

	/**
	 * Constructor 
	 */
	public NEDTreeIterator(INEDTypeResolver nedResources, IModuleTreeVisitor visitor) {
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
		for (NEDElement node : moduleType.getSubmods().values()) {
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
				submoduleTypeName = submodule.getLikeType(); // use the interface if actual module type is not available
			}
		}
		return submoduleTypeName;
	}
}
