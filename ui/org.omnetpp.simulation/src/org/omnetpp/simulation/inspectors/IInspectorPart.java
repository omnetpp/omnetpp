package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.simulation.model.cObject;


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
     * Returns container for the inspector. The container handles selection,
     * closing / maximizing / changing Z-order of inspectors, and other tasks.
     */
    IInspectorContainer getContainer();

    /**
     * Returns the inspected object.
     */
    cObject getObject();

    /**
     * Refresh the inspector's contents. Should only be called when the
     * inspector is already installed (the figure is on a draw2d canvas,
     * and setContainer() was already called). The client is responsible
     * for calling this method frequently enough to keep the inspector's
     * content up to date.
     */
    void refresh();

    /**
     * Must be called when the inspector is no longer needed
     */
    void dispose();

    /**
     * TODO
     */
    boolean isDisposed();

    /**
     * Brings this inspector to the front of all inspectors in Z-order.
     */
    void raiseToTop();

    /**
     * Returns the inspector's root figure.
     */
    IInspectorFigure getFigure();

    /**
     * TODO
     * null unless inspector is an SWT inspector
     */
    Control getSWTControl();

    /**
     * Returns whether this inspector can be maximized to fill the canvas.
     */
    boolean isMaximizable();

    /**
     * Called by the inspector container (which manages the selection) when the
     * selection changes. The inspector part should look at the objects in the
     * selection, and highlight the relevant ones (the ones it displays) on the
     * canvas.
     */
    void selectionChanged(IStructuredSelection selection);

    /**
     * TODO
     */
    void populateContextMenu(MenuManager manager, Point p);

    /**
     * TODO
     */
    void populateFloatingToolbar(ToolBarManager manager);

    /**
     * If there's a move/resize handle in the given figure at the given
     * coordinates (figure coordinates), return a binary OR of the appropriate
     * SWT.TOP, SWT.BOTTOM, SWT.LEFT, SWT.RIGHT constants, otherwise return 0.
     * For move operation it should set all four.
     * 
     * See FigureUtils.getXxxDragOperation() methods for many useful implementations.
     */
    int getDragOperation(IFigure figure, int x, int y);

    /**
     * For SWT-based inspectors. If there's a move/resize handle in the given control 
     * at the given coordinates (control coordinates), return a binary OR of the appropriate
     * SWT.TOP, SWT.BOTTOM, SWT.LEFT, SWT.RIGHT constants, otherwise return 0.
     * For move operation it should set all four.
     * 
     * See FigureUtils.getXxxDragOperation() methods for many useful implementations.
     */
    int getDragOperation(Control control, int x, int y);

}
