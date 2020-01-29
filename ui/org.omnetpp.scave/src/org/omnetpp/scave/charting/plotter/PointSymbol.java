/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a pixel. Size hints are ignored.
 *
 * @author Andras
 */
public class PointSymbol extends PlotSymbol {

    @Override
    public void drawSymbol(Graphics graphics, long x, long y) {
        LargeGraphics.drawPoint(graphics, x, y);
    }
}
