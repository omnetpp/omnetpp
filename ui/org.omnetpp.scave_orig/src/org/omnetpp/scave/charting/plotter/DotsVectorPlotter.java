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
 * Vector plotter drawing symbols only (and no lines).
 *
 * @author Andras
 */
public class DotsVectorPlotter extends VectorPlotter {

    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol, int timeLimitMillis) {
        return plotSymbols(plot, series, graphics, mapping, symbol, timeLimitMillis);
    }
}
