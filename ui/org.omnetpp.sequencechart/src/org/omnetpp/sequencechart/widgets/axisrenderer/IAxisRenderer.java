/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;

/**
 * An axis renderer draws the visual representation of a sequence chart axis.
 */
public interface IAxisRenderer {
    /**
     * The height in pixels of the visual representation to be reserved for this axis.
     */
    public int getHeight();

    /**
     * Does the actual drawing lazily on the given graphics in the provided event number interval.
     * Implementors should use the sequence chart coordinate transformations to get further details.
     */
    public void drawAxis(Graphics graphics, long startEventPtr, long endEventPtr);
}