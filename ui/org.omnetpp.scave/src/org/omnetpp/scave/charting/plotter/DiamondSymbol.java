/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.omnetpp.common.canvas.LargeGraphics;


/**
 * Draws a diamond symbol.
 *
 * @author andras
 */
public class DiamondSymbol extends PlotSymbol {
    private int size;
    private int[] poly;
    private long[] work = new long[8];

    public DiamondSymbol() {
    }

    public DiamondSymbol(int size) {
        super(size);
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        size = (sizeHint*141+50)/100;  // make same area as square; 1.41=sqrt(2)
        size |= 1; // make it an odd number
        int d = (sizeHint*71+50)/100;
        poly = new int[] {-d,0,0,-d,d,0,0,d};
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
        else {
            // manual translation; XXX try gc.setTransform(), maybe it's faster?
            work[0] = x + poly[0] + (graphics.getAntialias() == SWT.OFF ? 1 : 0); // with anti-alias OFF, +1 magic is needed to make it look symmetric
            work[1] = y + poly[1];
            work[2] = x + poly[2];
            work[3] = y + poly[3];
            work[4] = x + poly[4];
            work[5] = y + poly[5];
            work[6] = x + poly[6];
            work[7] = y + poly[7];
            graphics.setBackgroundColor(graphics.getForegroundColor());
            LargeGraphics.fillPolygon(graphics, work); //XXX make filled/unfilled version
        }
    }
}
