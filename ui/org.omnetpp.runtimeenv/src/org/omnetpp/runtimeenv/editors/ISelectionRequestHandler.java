package org.omnetpp.runtimeenv.editors;

import org.omnetpp.experimental.simkernel.swig.cObject;

public interface ISelectionRequestHandler {
	/** 
	 * Request selecting this object, and optionally deselecting all others.
	 */
	void select(cObject object, boolean deselectOthers);

	/** 
	 * Select this object if it was not selected, and vica versa. Selection 
	 * state of other objects remain unchanged.
	 */
	void toggleSelection(cObject object);
	
	/**
	 * Remove this object from the selection
	 */
	void deselect(cObject object);
	
	/**
	 * Remove all objects from the selection.
	 */
	void deselectAll();
}
