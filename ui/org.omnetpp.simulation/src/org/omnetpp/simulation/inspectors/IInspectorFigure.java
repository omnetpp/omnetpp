package org.omnetpp.simulation.inspectors;

import org.eclipse.draw2d.IFigure;

/**
 * Graphical representation of inspectors.
 *
 * @author Andras
 */
public interface IInspectorFigure extends IFigure {
    /**
     * Display/hide the selection border around the inspector
     */
    void setSelectionBorder(boolean isSelected);

}
