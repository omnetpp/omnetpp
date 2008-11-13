package org.omnetpp.inifile.editor.model;

import java.util.Stack;

import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * Visitor interface for use with NEDTreeTraversal.
 * 
 * @author Andras
 */
public interface IModuleTreeVisitor {
	/**
	 * Enter a submodule or start recursion. Should return false if submodules
	 * of this module should be skipped.
	 * 
	 * @param submodule      null at the root only
	 * @param submoduleType  type of the submodule or root type, never null
	 * @return true          go into submodules
	 */
	boolean enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType);
	
	/**
	 * Leave the module last entered.
	 */
	void leave();
	
	/**
	 * Encountered a submodule whose module type is unresolved. No leave() will follow.
	 */
	void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName);

	/**
	 * Encountered a submodule which introduces type recursion, and traversal is
	 * pruned here. No leave() will follow.
	 */
	void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType);

	/**
	 * Resolve the "like" parameter of a submodule. The method should return the
	 * actual (resolved) module type, or null if it cannot (or does not wish to) 
	 * resolve it. 
	 */
	String resolveLikeType(SubmoduleElementEx submodule);
	
	
	/**
	 * Example visitor 1: builds a tree
	 */
	public static class TreeBuilder implements IModuleTreeVisitor {
		private GenericTreeNode root = new GenericTreeNode("root");
		private GenericTreeNode current = root;
		
		public boolean enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
			GenericTreeNode child = new GenericTreeNode("("+submoduleType.getName()+")"+(submodule==null ? "" : submodule.getName()));
			current.addChild(child);
			current = child;
			return true;
		}
		
		public void leave() {
			current = current.getParent();
		}
		
		public void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName) {
		}
		
		public void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
		}
		
		public String resolveLikeType(SubmoduleElementEx submodule) {
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
		
		public boolean enter(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
			//fullPath.push("("+submoduleType.getName()+")"+(submodule==null ? "" : submodule.getName()));
			fullPath.push(submodule==null ? submoduleType.getName() : submodule.getName());
			Debug.println(StringUtils.join(fullPath.toArray(), "."));
			return true;
		}
		
		public void leave() {
			fullPath.pop();
		}
		
		public void unresolvedType(SubmoduleElementEx submodule, String submoduleTypeName) {
		}
		
		public void recursiveType(SubmoduleElementEx submodule, INEDTypeInfo submoduleType) {
		}
		
		public String resolveLikeType(SubmoduleElementEx submodule) {
			return null;
		}
	}
	
}