package org.omnetpp.runtimeenv.editors;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.experimental.simkernel.swig.cObject;

/**
 * Represents a container for inspectors. The container handles selection,
 * closing / maximizing / changing Z-order of inspectors, and other tasks.
 * 
 * Selection-related are typically called by the inspector on mouse clicks, 
 * and the methods request that some objects be added or removed from the 
 * canvas selection. Highlighting the selected objects takes place afterwards,
 * in the selectionChanged() method being called by the canvas on all inspectors. 
 * 
 * @author Andras
 */
public interface IInspectorContainer {
	/**
	 * Closes the inspector.
	 */
	void close(IInspectorPart inspector);

	/**
	 * Returns the SWT control (likely a canvas) that contains the
	 * inspector figures. This can be useful for adding SWT listeners
	 * or creating inspectors that contain SWT controls. 
	 */
	Composite getControl();

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
