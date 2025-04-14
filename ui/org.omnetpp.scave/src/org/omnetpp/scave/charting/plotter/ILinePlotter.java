/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;

/**
 * Interface for line chart line plotters
 *
 * @author Andras
 */
public interface ILinePlotter {
    /**
     * Draw the given series of the plot on the graphics with the given coordinate mapping;
     * it must not take more than the given time limit.
     *
     * @return true if OK, false if the drawing is incomplete due to timeout
     */
    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IPlotSymbol symbol, int timeLimitMillis);

    /**
     * Calculates the sequence of vertices (in data coordinates) that form the visible
     * line segments between two consecutive data points (x1, y1) and (x2, y2),
     * based on the plotter's specific drawing style. Returns an array of doubles
     * representing the vertex coordinates [x1, y1, x2, y2, ...] (can be empty).
     */
    public double[] getSegmentPoints(double x1, double y1, double x2, double y2);
}
