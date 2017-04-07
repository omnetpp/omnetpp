/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a triangle symbol.
 *
 * @author andras
 */
public class TriangleSymbol extends ChartSymbol {
    private int height;
    private int[] poly;
    private long[] work = new long[6];

    public TriangleSymbol() {
    }

    public TriangleSymbol(int size) {
        super(size);
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        // equal-sized triangle whose area is sizeHint^2
        height = (132*sizeHint+50)/100; // 1.32 = sqrt4(3)
        int halfside = (76*sizeHint+50)/100; // 0.76 = 1 / sqrt4(3)
        int off = (84*sizeHint+50)/100;
        poly = new int[] {-halfside, (height-off), 0, -off, halfside, (height-off)};
    }

    public void drawSymbol(Graphics graphics, long x, long y) {
        if (sizeHint<=0) {
            // nothing
        }
        else if (sizeHint==1) {
            LargeGraphics.drawPoint(graphics, x, y);
        }
        else if (sizeHint==2 || sizeHint==3) {
            LargeGraphics.drawPoint(graphics, x, y);
            LargeGraphics.drawPoint(graphics, x-1, y);
            LargeGraphics.drawPoint(graphics, x+1, y);
            LargeGraphics.drawPoint(graphics, x, y-1);
        }
        else {
            // manual translation; XXX try gc.setTransform(), maybe it's faster?
            work[0] = x + poly[0];
            work[1] = y + poly[1];
            work[2] = x + poly[2];
            work[3] = y + poly[3];
            work[4] = x + poly[4];
            work[5] = y + poly[5];
            graphics.setBackgroundColor(graphics.getForegroundColor());
            LargeGraphics.fillPolygon(graphics, work);  //XXX make filled/unfilled version
        }
    }
}
