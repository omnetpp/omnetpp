/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a square symbol.
 *
 * @author andras
 */
public class SquareSymbol extends PlotSymbol {
    private int size;

    public SquareSymbol() {
    }

    public SquareSymbol(int size) {
        super(size);
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        size = sizeHint|1; // make odd number
    }


    public void drawSymbol(Graphics graphics, long x, long y) {
        if (size<=0) {
            // nothing
        }
        else if (size==1) {
            LargeGraphics.drawPoint(graphics, x, y);
        }
        else {
            graphics.setBackgroundColor(graphics.getForegroundColor());
            LargeGraphics.fillRectangle(graphics, x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
        }
    }
}
