package org.omnetpp.ned.editor.graph.model;

import java.util.List;

public interface INedContainer {
	
	/**
	 * Returns the primary child list used to display visual children.
	 * @param child
	 * @return
	 */
	public List<? extends INedNode> getModelChildren();
	
	/**
	 * Add a child to a container model.
	 * @param child
	 */
	public void addModelChild(INedNode child);

	/**
	 * Insert the child at the give position.
	 * @param child
	 * @param index
	 */
	public void insertModelChild(int index, INedNode child);
	
	/**
	 * Insert the child at the give position.
	 * @param child Child to be inserted
	 * @param insertBefore Sibling element where the child will be inserted
	 */
	public void insertModelChild(INedNode insertBefore, INedNode child);

	/**
	 * Remove a specfic child from the parent.
	 * @param child
	 */
	public void removeModelChild(INedNode child);
}
