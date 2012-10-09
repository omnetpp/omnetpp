package org.omnetpp.figures.misc;

import org.eclipse.draw2d.geometry.Rectangle;

/**
 * Interface to support modified selection handle rectangles. It has the same purpose as
 * IHandleBounds from the GEF plugin.
 */
public interface ISelectionHandleBounds {
    /**
     * The selection rectangle that should be used around the figure.
     */
    Rectangle getHandleBounds();
}
