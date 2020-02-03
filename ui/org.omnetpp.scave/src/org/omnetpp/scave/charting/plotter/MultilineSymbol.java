/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a multiline symbol.
 *
 * @author andras
 */
public abstract class MultilineSymbol extends PlotSymbol {
    protected int[] points;  // x1, y1, x2, y2 repeating
    protected long[] work;

    public MultilineSymbol(int size) {
        super(size);
    }

    protected abstract int[] getLines(int sizeHint);

    protected static void rotate90(int[] points) {
        for (int i = 0; i < points.length; i+=2) {
            int x = points[i];
            points[i] = points[i+1];
            points[i+1] = -x;
        }
    }

    @Override
    public void setSizeHint(int sizeHint) {
        super.setSizeHint(sizeHint);
        points = getLines(sizeHint);
        work = new long[points.length];
    }

    public void drawSymbol(Graphics graphics, long x, long y) {
        if (sizeHint<=0) {
            // nothing
        }
        else if (sizeHint==1) {
            LargeGraphics.drawPoint(graphics, x, y);
        }
        else {
            // manual translation; XXX try gc.setTransform(), maybe it's faster?
            for (int i = 0; i < points.length; i+=2) {
                work[i] = x + points[i];
                work[i+1] = y + points[i+1];
            }
            graphics.setBackgroundColor(graphics.getForegroundColor());
            for (int i = 0; i < work.length; i += 4)
                LargeGraphics.drawLine(graphics, work[i], work[i+1], work[i+2], work[i+3]);
        }
    }
}
