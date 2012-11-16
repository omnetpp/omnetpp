package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.HoverInfo;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.figures.IInspectorFigure;
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
     * TODO
     */
    InspectorDescriptor getDescriptor();

    /**
     * Return container for the inspector. The container handles selection,
     * closing / maximizing / changing Z-order of inspectors, and other tasks.
     */
    IInspectorContainer getContainer();

    /**
     * Return the inspected object.
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
     * Bring this inspector to the front of all inspectors in Z-order.
     */
    void raiseToTop();

    /**
     * Give the inspector keyboard focus.
     */
    void setFocus();

    /**
     * Return the inspector's root figure.
     */
    IInspectorFigure getFigure();

    /**
     * Used for getting the target figure of callouts, see CalloutSupport.
     *
     * Return the figure within this inspector that contains (directly or
     * indirectly) the given object. It may also return the main figure of this
     * inspector, or null. Example: when called on a compound module inspector
     * with object being a cPar: if the cPar belongs to the inspected module it
     * should return the main figure; if the cPar belongs to a submodule it
     * should return the submodule figure; if the cPar belongs to some
     * sub-submodule, it should return the figure of the submodule that contains
     * them; and for cPars outside the inspected module it should return null.
     */
    IFigure findFigureContaining(cObject object);

    /**
     * TODO
     * null unless inspector is an SWT inspector
     */
    Control getSWTControl();

    /**
     * Return true if this inspector can be maximized to fill the canvas.
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
     * Add actions to the inspector's context menu for the given x,y inspector layer coordinates.
     */
    void populateContextMenu(MenuManager menu, int x, int y);

    /**
     * Add actions to the floating toolbar.
     */
    void populateFloatingToolbar(ToolBarManager manager);

    /**
     * Return hover information for what is at the given x,y inspector layer coordinates.
     * (The point falls inside the inspector area.)
     */
    HoverInfo getHoverFor(int x, int y);

    /**
     * If there's a move/resize handle in the given figure at the given x,y inspector layer
     * coordinates, return a binary OR of the appropriate SWT.TOP, SWT.BOTTOM, SWT.LEFT or
     * SWT.RIGHT constants, otherwise return 0. For move operation it should set all four.
     *
     * See FigureUtils.getXxxDragOperation() methods for many useful implementations.
     */
    int getDragOperation(IFigure figure, int x, int y);

    /**
     * For SWT-based inspectors. If there's a move/resize handle in the given control
     * at the given x,y control coordinates, return a binary OR of the appropriate
     * SWT.TOP, SWT.BOTTOM, SWT.LEFT, SWT.RIGHT constants, otherwise return 0.
     * For move operation it should set all four.
     *
     * See FigureUtils.getXxxDragOperation() methods for many useful implementations.
     */
    int getDragOperation(Control control, int x, int y);
}
