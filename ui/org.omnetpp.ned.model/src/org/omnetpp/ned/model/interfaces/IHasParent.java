package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.NEDElement;

/**
 * @author rhornig
 * An object that can be added / removed to/from the model tree
 */
public interface IHasParent {

	/**
	 * Removes this node from the parent
	 */
	public void removeFromParent();

	/**
	 * @return The parent of the given model element (if inserted into the model)
	 */
	public NEDElement getParent();
}