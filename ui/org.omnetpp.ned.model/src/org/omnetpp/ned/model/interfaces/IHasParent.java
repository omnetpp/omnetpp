package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * An object that can be added / removed to/from the model tree
 * @author rhornig
 */
public interface IHasParent extends INEDElement {

	/**
	 * Removes this node from the parent
	 */
	public void removeFromParent();

	/**
	 * @return The parent of the given model element (if inserted into the model)
	 */
	public INEDElement getParent();
}