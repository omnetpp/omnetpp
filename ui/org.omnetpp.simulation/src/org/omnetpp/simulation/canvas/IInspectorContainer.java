package org.omnetpp.simulation.canvas;

import java.util.List;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Layer;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.simulation.editors.SimulationEditor;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.model.cObject;

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
public interface IInspectorContainer extends ISelectionProvider {
    /**
     * Returns the SimulationEditor that contains this inspector container, or
     * null if this editor is not contained in a SimulationEditor.
     */
    SimulationEditor getEditor();

    /**
     * TEMPORARY: open an inspector for this object
     * todo what kind, etc
     * @return
     */
    IInspectorPart inspect(cObject object);

    /**
     * Closes the given inspector.
     */
    void close(IInspectorPart inspector);

    /**
     * Returns the list of all currently open inspectors. The list must not
     * be modified by the caller.
     */
    List<IInspectorPart> getInspectors();

    /**
     * Returns the SWT control (a FigureCanvas) that contains the
     * inspector figures. This can be useful for adding SWT listeners
     * or creating inspectors that contain SWT controls.
     */
    FigureCanvas getControl();

    /**
     * Return the layer to which inspector figures are added.
     */
    Figure getInspectorsLayer();

    /**
     * Return the layer to which inspectors should add transient controls
     * (close button, toolbar, etc) that appear when the mouse hovers over
     * the inspector.
     */
    Layer getControlsLayer();

    /**
     * TODO
     */
    void openFloatingToolbarFor(IInspectorPart inspector);

    /**
     * TODO
     */
    void closeFloatingToolbar();

    /**
     * TODO
     */
    Control getFloatingToolbar();

    /**
     * TODO
     */
    IInspectorPart getFloatingToolbarOwner();

    /**
     * TODO
     */
    void updateFloatingToolbarActions();

    /**
     * TODO
     * note: works in conjunction with IInspectorPart.getDragOperation()
     */
    void addMoveResizeSupport(IFigure figure);

    /**
     * TODO
     */
    void removeMoveResizeSupport(IFigure figure);

    /**
     * TODO
     */
    void addMoveResizeSupport(Control control);

    /**
     * TODO
     */
    void removeMoveResizeSupport(Control control);

    /**
     * TODO
     */
    org.eclipse.swt.graphics.Point translateAbsoluteFigureCoordinatesToCanvas(int x, int y);

    org.eclipse.draw2d.geometry.Point translateCanvasToAbsoluteFigureCoordinates(int x, int y);

    /**
     * Request selecting this object, and optionally deselecting all others.
     */
    void select(Object object, boolean deselectOthers);

    /**
     * Request selecting the given objects, and optionally deselecting all others.
     */
    void select(Object[] objects, boolean deselectOthers);

    /**
     * Select this object if it was not selected, and vica versa. Selection
     * state of other objects remain unchanged.
     */
    void toggleSelection(Object object);

    /**
     * Remove this object from the selection
     */
    void deselect(Object object);

    /**
     * Remove all objects from the selection.
     */
    void deselectAll();

}
