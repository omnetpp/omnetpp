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
public class DotsLinePlotter extends LinePlotter {

    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IPlotSymbol symbol, int timeLimitMillis) {
        return plotSymbols(plot, series, graphics, mapping, symbol, timeLimitMillis);
    }
}
