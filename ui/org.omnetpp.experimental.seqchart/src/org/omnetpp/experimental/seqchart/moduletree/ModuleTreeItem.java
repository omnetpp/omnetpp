package org.omnetpp.experimental.seqchart.moduletree;

import java.util.List;

/**
 * Stores a submodule tree for ModuleTreeDialog etc.
 * 
 * @author andras
 */
public class ModuleTreeItem {
	private int moduleId = -1;
	private String moduleName;
	private String moduleFullPath; //XXX maybe this could be cached as well?
	private String moduleClassName;
	private ModuleTreeItem parentModule;
	private ModuleTreeItem[] submodules = new ModuleTreeItem[0];

	/**
	 * Create a blank node. Can be used to create a tree root. 
	 */
	public ModuleTreeItem() {
		moduleName = "<root>";
	}

	/**
	 * Create an item with the given name and (optionally) parent.
	 *  
	 * @param moduleName cannot be null
	 * @param parent null is allowed
	 */
	public ModuleTreeItem(String moduleName, ModuleTreeItem parent) {
		setModuleName(moduleName);
		parentModule = parent;
		if (parentModule!=null)
			parentModule.addSubmodule(this);
	}
	
	private void addSubmodule(ModuleTreeItem item) {
		ModuleTreeItem[] newSubmodules = new ModuleTreeItem[submodules.length+1];
		System.arraycopy(submodules, 0, newSubmodules, 0, submodules.length);
		newSubmodules[submodules.length] = item;
		submodules = newSubmodules;
	}

	/**
	 * Returns the ModuleTreeItems from the tree root down to this element.
	 * Useful for creating TreePath object for use with ITreeSelection for TreeViewer.
	 */
	public ModuleTreeItem[] getPath() {
	    int count = 0;
	    for (ModuleTreeItem current=this; current!=null; current=current.getParentModule())
	    	count++;
	    ModuleTreeItem[] path = new ModuleTreeItem[count];
	    for (ModuleTreeItem current=this; current!=null; current=current.getParentModule())
	    	path[--count] = current;
	    return path;
	}

	/**
	 * Return the path as a dot-separared string.
	 */
	public String getFullPathName() {
	    String path = getModuleName();
	    for (ModuleTreeItem current=this; current!=null; current=current.getParentModule())
	    	path = current.getModuleName()+"."+path;
	    return path;
	}
	
	public String getModuleName() {
		return moduleName;
	}

	public void setModuleName(String moduleName) {
		if (moduleName==null)
			throw new IllegalArgumentException("null is not allowed as module name");
		this.moduleName = moduleName;
	}

	public int getModuleId() {
		return moduleId;
	}

	public void setModuleId(int moduleId) {
		this.moduleId = moduleId;
	}

	public String getModuleClassName() {
		return moduleClassName;
	}

	public void setModuleClassName(String moduleClassName) {
		this.moduleClassName = moduleClassName;
	}

	public ModuleTreeItem getParentModule() {
		return parentModule;
	}

	public ModuleTreeItem[] getSubmodules() {
		return submodules;
	}

	public ModuleTreeItem getSubmodule(String submoduleName) {
		for (ModuleTreeItem submodule : submodules)
			if (submodule.moduleName.equals(submoduleName))
				return submodule;
		return null;
	}
}
