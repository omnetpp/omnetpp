/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a "plus" symbol.
 *
 * @author andras
 */
public class PlusSymbol extends PlotSymbol {
    private int size;

    public PlusSymbol() {
    }

    public PlusSymbol(int size) {
        super(size);
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        size = (134*sizeHint+50)/100; // 3/sqrt(5) ~ 1.34
        size |= 1; // make it an odd number
    }

    public void drawSymbol(Graphics graphics, long x, long y) {
        if (size<=0) {
            // nothing
        }
        else if (size==1) {
            LargeGraphics.drawPoint(graphics, x, y);
        }
        else if (size==2 || size==3) {
            LargeGraphics.drawPoint(graphics, x, y);
            LargeGraphics.drawPoint(graphics, x-1, y);
            LargeGraphics.drawPoint(graphics, x+1, y);
            LargeGraphics.drawPoint(graphics, x, y-1);
            LargeGraphics.drawPoint(graphics, x, y+1);
        }
        else if (size<8) {
            int d = size/2;
            LargeGraphics.drawLine(graphics, x-d, y, x+d, y);
            LargeGraphics.drawLine(graphics, x, y-d, x, y+d);
        }
        else {
            int saved = graphics.getLineWidth();
            graphics.setLineWidth(size/4);
            //graphics.setLineCap(SWT.FLAT);
            int d = size/2;
            LargeGraphics.drawLine(graphics, x-d, y, x+d, y);
            LargeGraphics.drawLine(graphics, x, y-d, x, y+d);
            graphics.setLineWidth(saved);
        }
    }
}
