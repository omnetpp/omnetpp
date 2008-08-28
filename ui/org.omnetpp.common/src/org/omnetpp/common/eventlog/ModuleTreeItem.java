package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;


/**
 * Stores a module tree.
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

	private Map<Integer, ModuleTreeItem> moduleIdToModuleMap;

	public interface IModuleTreeItemVisitor
	{
		public void visit(ModuleTreeItem treeItem);
	}

	/**
	 * Create an item with the given name and (optionally) parent.
	 *  
	 * @param moduleName cannot be null
	 * @param parent null is allowed
	 */
	public ModuleTreeItem(int moduleId, String moduleName, String moduleClassName, ModuleTreeItem parent, boolean isCompoundModule) {
        setModuleId(moduleId);
		setModuleName(moduleName);
		setModuleClassName(moduleClassName);
		this.isCompoundModule = isCompoundModule;
		parentModule = parent;
		if (parentModule != null)
			parentModule.addSubmodule(this);
		else
		    getModuleIdToModuleMap().put(moduleId, this);
	}
	
	public boolean isCompoundModule() {
	    return isCompoundModule;
	}
	
	private void addSubmodule(ModuleTreeItem submodule) {
        getRootModule().getModuleIdToModuleMap().put(submodule.getModuleId(), submodule);
		submodules.add(submodule);
	}
	
	public void remove() {
        getRootModule().getModuleIdToModuleMap().remove(moduleId);
        parentModule.submodules.remove(this);
	    parentModule = null;
	}

	public ModuleTreeItem addDescendantModule(int parentModuleId, int moduleId, String moduleClassName, String moduleFullName, boolean isCompoundModule) {
	    Assert.isTrue(findDescendantModule(moduleId) == null);
	    ModuleTreeItem parentModule = findDescendantModule(parentModuleId);
	    Assert.isTrue(parentModuleId == -1 || parentModule != null);
	    ModuleTreeItem module = new ModuleTreeItem(moduleId, moduleFullName, moduleClassName, parentModule, isCompoundModule);

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
	 * Return the path as a dot-separated string.
	 */
	public String getModuleFullPath() {
	    String path = getModuleName();
	    for (ModuleTreeItem current = getParentModule(); current != null; current=current.getParentModule())
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
		ModuleTreeItem ancestorModule = this;
		
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
	
	private Map<Integer, ModuleTreeItem> getModuleIdToModuleMap() {
	    if (moduleIdToModuleMap == null)
	        moduleIdToModuleMap = new HashMap<Integer, ModuleTreeItem>();
	    
	    return moduleIdToModuleMap;
	}

	public ModuleTreeItem findDescendantModule(final int id) {
	    ModuleTreeItem descendantModule = getRootModule().getModuleIdToModuleMap().get(id);
	    
	    if (isDescendantModule(descendantModule))
	        return descendantModule;
	    else
	        return null;
	}
	
    public boolean isDescendantModule(ModuleTreeItem descendantModule) {
        while (descendantModule != null)
            if (descendantModule == this)
                return true;
            else
                descendantModule = descendantModule.parentModule;

        return false;
    }

    public ModuleTreeItem findDescendantModule(final String fullPath) {
        // TODO: put a hash table in the root to make this search fast
        final ModuleTreeItem[] descendant = new ModuleTreeItem[1];

        visitLeaves(new IModuleTreeItemVisitor() {
            public void visit(ModuleTreeItem treeItem) {
                if (treeItem.getModuleFullPath().equals(fullPath))
                    descendant[0] = treeItem;
            }
        });

        return descendant[0];
    }

    @Override
	public String toString() {
	    return getModuleFullPath();
	}
}
