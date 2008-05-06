package org.omnetpp.common.eventlog;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;


/**
 * Stores a submodule tree for ModuleTreeDialog etc.
 * 
 * @author andras
 */
public class ModuleTreeItem implements Comparable<ModuleTreeItem> {
	private int moduleId = -1;
	
	private boolean isCompoundModule;

	private String moduleName;
	
	private String moduleClassName;
	
	private ModuleTreeItem parentModule;
	
	private ArrayList<ModuleTreeItem> submodules = new ArrayList<ModuleTreeItem>();

	public interface IModuleTreeItemVisitor
	{
		public void visit(ModuleTreeItem treeItem);
	}
	
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
	public ModuleTreeItem(String moduleName, ModuleTreeItem parent, boolean isCompoundModule) {
		setModuleName(moduleName);
		this.isCompoundModule = isCompoundModule;
		parentModule = parent;
		if (parentModule != null)
			parentModule.addSubmodule(this);
	}
	
	public boolean isCompoundModule() {
	    return isCompoundModule;
	}
	
	private void addSubmodule(ModuleTreeItem submodule) {
		submodules.add(submodule);
	}
	
	public void remove() {
        parentModule.submodules.remove(this);
	    parentModule = null;
	}

	public ModuleTreeItem addDescendantModule(int parentModuleId, int moduleId, String moduleClassName, String moduleFullName, boolean isCompoundModule) {
	    Assert.isTrue(findDescendantModule(moduleId) == null);
	    ModuleTreeItem parentModule = findDescendantModule(parentModuleId);
	    ModuleTreeItem module = new ModuleTreeItem(moduleFullName, parentModule, isCompoundModule);
	    module.setModuleId(moduleId);
	    module.setModuleClassName(moduleClassName);

	    return module;
	}

	public void visitLeaves(IModuleTreeItemVisitor visitor)
	{
		visitLeaves(visitor, this);
	}
	
	private void visitLeaves(IModuleTreeItemVisitor visitor, ModuleTreeItem treeItem)
	{
		visitor.visit(treeItem);

		for (ModuleTreeItem childTreeItem : treeItem.getSubmodules())
			visitLeaves(visitor, childTreeItem);
	}

	/**
	 * Returns the ModuleTreeItems from the tree root down to this element.
	 */
	public ModuleTreeItem[] getTreePath() {
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
	public String getModuleFullPath() {
	    String path = getModuleName();
	    for (ModuleTreeItem current=getParentModule(); current!=null && current.getParentModule()!=null /*omit <root>*/; current=current.getParentModule())
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

	public ModuleTreeItem getRootModule() {
		ModuleTreeItem ancestorModule = parentModule;
		
		while (ancestorModule.parentModule != null)
			ancestorModule = ancestorModule.parentModule;
		
		return ancestorModule;
	}
	
	public ModuleTreeItem getAncestorModuleUnder(ModuleTreeItem ancestorParentModule) {
		if (parentModule == ancestorParentModule)
			return this;
		else if (parentModule != null)
			return parentModule.getAncestorModuleUnder(ancestorParentModule);
		else
			return null;
	}

	public ModuleTreeItem[] getSubmodules() {
		return submodules.toArray(new ModuleTreeItem[0]);
	}

	public ModuleTreeItem getSubmodule(String submoduleName) {
		for (ModuleTreeItem submodule : submodules)
			if (submodule.moduleName.equals(submoduleName))
				return submodule;
		return null;
	}

	public int compareTo(ModuleTreeItem item) {
		return moduleName.compareTo(item.getModuleName());
	}

	public ModuleTreeItem findDescendantModule(final int id) {
		final ModuleTreeItem[] descendant = new ModuleTreeItem[1];

		visitLeaves(new IModuleTreeItemVisitor() {
			public void visit(ModuleTreeItem treeItem) {
				if (treeItem.getModuleId() == id)
					descendant[0] = treeItem;
			}
		});

		return descendant[0];
	}
}
