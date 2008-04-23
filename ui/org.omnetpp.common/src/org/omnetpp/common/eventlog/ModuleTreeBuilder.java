package org.omnetpp.common.eventlog;

import java.util.Hashtable;

/**
 * Utility class to help build a ModuleTreeItem tree.
 * 
 * @author andras
 */
public class ModuleTreeBuilder {
	private Hashtable<Integer, ModuleTreeItem> moduleIdToModuleMap = new Hashtable<Integer, ModuleTreeItem>();

	private ModuleTreeItem treeRoot = new ModuleTreeItem();
	
	public ModuleTreeItem addModule(int parentModuleId, int moduleId, String moduleClassName, String moduleFullName, boolean isCompoundModule) {
		ModuleTreeItem parentModule = (ModuleTreeItem)moduleIdToModuleMap.get(parentModuleId);
		
		if (parentModule == null)
			parentModule = treeRoot;
		
		ModuleTreeItem module = new ModuleTreeItem(moduleFullName, parentModule, isCompoundModule);
		module.setModuleId(moduleId);
		module.setModuleClassName(moduleClassName);
		moduleIdToModuleMap.put(moduleId, module);
		
		return module;
	}
	
	/**
	 * Returns the constructed tree.
	 */
	public ModuleTreeItem getModuleTree() {
		return treeRoot;
	}
}
