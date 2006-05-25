package org.omnetpp.experimental.seqchart.moduletree;

import java.util.StringTokenizer;

/**
 * Utility class to help build a ModuleTreeItem tree.
 * 
 * @author andras
 */
public class ModuleTreeBuilder {
	private ModuleTreeItem tree = new ModuleTreeItem();
	
	/**
	 * Adds a module to the current tree, by creating intervening levels if they
	 * don't exist yet. E.g. when adding "net.host[2].networklayer.ip", if only
	 * "net.host[2]" exists in the current tree, it will create "networklayer", 
	 * and then "ip". If everything already exists, the method does nothing.
	 * It returns the last submodule in the path (whether it already existed or not).
	 */
	public ModuleTreeItem addModule(String fullPath) {
		StringTokenizer st = new StringTokenizer(fullPath, ".");
		ModuleTreeItem currentParent = tree;
		while (st.hasMoreTokens()) {
			String submodName = st.nextToken();
			if (currentParent.getSubmodule(submodName)!=null) {
				// find existing
				currentParent = currentParent.getSubmodule(submodName);
			}
			else {
				// add new
				currentParent = new ModuleTreeItem(submodName, currentParent);
			}
			
		}
		return currentParent;
	}

	/**
	 * Same as addModule(fullPath), but sets the module class name as well on the 
	 * last submodule. The class name must be consistent with the already set ones,
	 * otherwise a RuntimeException will be thrown -- this allows for detecting
	 * inconsistencies in input data. 
	 */
	public ModuleTreeItem addModule(String fullPath, String moduleClassName) {
		ModuleTreeItem newItem = addModule(fullPath);
		if (newItem.getModuleClassName()!=null && !newItem.getModuleClassName().equals(moduleClassName))
			throw new RuntimeException("submodule '"+fullPath+"' already added with a different module class name ('"+newItem.getModuleClassName()+"')");
		newItem.setModuleClassName(moduleClassName);
		return newItem;
	}

	/**
	 * Same as addModule(fullPath), but sets the module class name and module Id
	 * * as well on the last submodule. The class name and Id must be consistent 
	 * with the already set ones, otherwise a RuntimeException will be thrown -- 
	 * this allows for detecting inconsistencies in input data. 
	 */
	public ModuleTreeItem addModule(String fullPath, String moduleClassName, int moduleId) {
		ModuleTreeItem newItem = addModule(fullPath, moduleClassName);
		if (newItem.getModuleId()!=-1 && newItem.getModuleId()!=moduleId)
			throw new RuntimeException("submodule '"+fullPath+"' already added with a different module Id ("+newItem.getModuleId()+")");
		newItem.setModuleId(moduleId);
		return newItem;
	}
	
	/**
	 * Returns the constructed tree.
	 */
	public ModuleTreeItem getModuleTree() {
		return tree;
	}
}
