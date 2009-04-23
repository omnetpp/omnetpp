package org.omnetpp.runtimeenv.editors;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.experimental.simkernel.swig.cObject;


/**
 * Represents an inspector. An inspector presents a simulation object in the 
 * runtime GUI. The visual representation of an inspector is a Draw2D figure
 * that can be placed on a canvas.
 * 
 * The inspector is also a selection provider. When this inspector is selected 
 * (see isSelected()), the selection should include this inspector (or its 
 * inspected object) as well. 
 */
public interface IInspectorPart {

	/**
	 * Returns the inspected object.
	 */
	cObject getObject();

	/**
	 * Must be called when the inspector is no longer needed
	 */
	void dispose();

    /**
     * Returns the corresponding figure.
     */
    IInspectorFigure getFigure();

    /**
     * Returns whether this inspector can be maximized to fill the canvas.
     */
    boolean isMaximizable();
    
    /**
     * Returns true if this inspector is selected.
     */
    boolean isSelected();

    /**
     * Returns container for the inspector. The container handles selection,
     * closing / maximizing / changing Z-order of inspectors, and other tasks.
     */ 
    IInspectorContainer getContainer();
    
    /**
     * Called when the canvas selection changes. The inspector part should
     * look at the objects in the selection, and highlight the relevant ones
     * (the ones it displays) on the canvas
     */
    void selectionChanged(IStructuredSelection selection);

    void populateContextMenu(MenuManager contextMenuManager, Point p);
}
