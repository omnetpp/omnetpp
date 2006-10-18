package org.omnetpp.experimental.seqchart.moduletree;

import java.util.Hashtable;

/**
 * Utility class to help build a ModuleTreeItem tree.
 * 
 * @author andras
 */
public class ModuleTreeBuilder {
	private Hashtable<Integer, ModuleTreeItem> moduleIdToModuleMap = new Hashtable<Integer, ModuleTreeItem>();

	private ModuleTreeItem treeRoot;
	
	public ModuleTreeItem addModule(int parentModuleId, int moduleId, String moduleClassName, String moduleFullName) {
		ModuleTreeItem parentModule = (ModuleTreeItem)moduleIdToModuleMap.get(parentModuleId);
		ModuleTreeItem module = new ModuleTreeItem(moduleFullName, parentModule);
		module.setModuleId(moduleId);
		module.setModuleClassName(moduleClassName);
		moduleIdToModuleMap.put(moduleId, module);
		
		if (treeRoot == null)
			treeRoot = module;

		return module;
	}
	
	/**
	 * Returns the constructed tree.
	 */
	public ModuleTreeItem getModuleTree() {
		return treeRoot;
	}
}
