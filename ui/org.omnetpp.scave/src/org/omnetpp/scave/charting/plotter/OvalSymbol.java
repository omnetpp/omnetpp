/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a "oval" symbol.
 *
 * @author andras
 */
public class OvalSymbol extends PlotSymbol {
    private int size;

    public OvalSymbol() {
    }

    public OvalSymbol(int size) {
        super(size);
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        size = (sizeHint*113+50)/100;  // make same area as square; 1.13=2/sqrt(pi)
        size |= 1;  // make an odd number
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
            LargeGraphics.fillOval(graphics, x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
        }
    }
}
