package org.omnetpp.ned2.model;

import java.util.List;

/**
 * Ned model element that has children
 * @author rhornig
 *
 */
public interface ISubmoduleContainer {
	
	/**
	 * Returns all submodule containd in the module.
	 * @param child
	 * @return
	 */
	public List<? extends INamedGraphNode> getSubmodules();
	
	/**
	 * Add a child to a container model.
	 * @param child
	 */
	public void addSubmodule(INamedGraphNode child);

	/**
	 * Insert the child at the give position.
	 * @param child
	 * @param index
	 */
	public void insertSubmodule(int index, INamedGraphNode child);
	
	/**
	 * Insert the child at the give position.
	 * @param child Child to be inserted
	 * @param insertBefore Sibling element where the child will be inserted
	 */
	public void insertSubmodule(INamedGraphNode insertBefore, INamedGraphNode child);

	/**
	 * Remove a specfic child from the parent.
	 * @param child
	 */
	public void removeSubmodule(INamedGraphNode child);

}
