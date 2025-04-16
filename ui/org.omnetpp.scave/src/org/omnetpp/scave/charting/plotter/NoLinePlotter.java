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
 * Line plotter drawing symbols only (and no lines).
 *
 * @author Andras
 */
public class NoLinePlotter extends LinePlotter {

    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IPlotSymbol symbol, int timeLimitMillis) {
        return plotSymbols(plot, series, graphics, mapping, symbol, timeLimitMillis);
    }

    public double[] getSegmentPoints(double x1, double y1, double x2, double y2) {
        return new double[0]; // no line segments
    }
}
