package org.omnetpp.ned.editor.graph.model;

import java.util.List;

/**
 * Ned model element that has children
 * @author rhornig
 *
 */
public interface INedContainer {
	
	/**
	 * Returns the primary child list used to display visual children.
	 * @param child
	 * @return
	 */
	public List<? extends INedModule> getModelChildren();
	
	/**
	 * Add a child to a container model.
	 * @param child
	 */
	public void addModelChild(INedModule child);

	/**
	 * Insert the child at the give position.
	 * @param child
	 * @param index
	 */
	public void insertModelChild(int index, INedModule child);
	
	/**
	 * Insert the child at the give position.
	 * @param child Child to be inserted
	 * @param insertBefore Sibling element where the child will be inserted
	 */
	public void insertModelChild(INedModule insertBefore, INedModule child);

	/**
	 * Remove a specfic child from the parent.
	 * @param child
	 */
	public void removeModelChild(INedModule child);

    /**
     * Removes this node from the parent
     */
    public void removeFromParent();
}
