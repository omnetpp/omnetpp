/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.canvas.LargeGraphics;

/**
 * Draws a polygon symbol.
 *
 * @author andras
 */
public abstract class PolygonSymbol extends PlotSymbol {
    protected int[] points; // x, y repeating
    protected long[] work;


    public PolygonSymbol(int size) {
        super(size);
    }

    protected abstract int[] getPoints(int sizeHint);

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
        points = getPoints(sizeHint);
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
            LargeGraphics.fillPolygon(graphics, work);  //XXX make filled/unfilled version
        }
    }
}
