package org.omnetpp.ned2.model;

public interface IParentable {

	/**
	 * Removes this node from the parent
	 */
	public void removeFromParent();

	/**
	 * @return The parent of the given model element (if inserted into the model)
	 */
	public NEDElement getParent();
}