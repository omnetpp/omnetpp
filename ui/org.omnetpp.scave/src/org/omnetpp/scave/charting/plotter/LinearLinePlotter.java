/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import static org.omnetpp.common.canvas.ICoordsMapping.NAN_PIX;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.dataset.IXYDataset;

/**
 * Line plotter that connects data points with lines.
 *
 * @author Andras
 */
public class LinearLinePlotter extends LinePlotter {

    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IPlotSymbol symbol, int timeLimitMillis) {
        IXYDataset dataset = plot.getDataset();
        int n = dataset.getItemCount(series);
        if (n==0)
            return true;

        // Note: for performance (this is most commonly used plotter), we don't use plotSymbols()
        // from the base class, but draw lines and symbols in a single loop instead

        // dataset index range to iterate over
        int[] range = indexRange(plot, series, graphics, mapping);
        int first = range[0], last = range[1];

        // Performance optimization: avoid painting the same pixels over and over
        // when drawing vertical lines. This results in magnitudes faster
        // execution for large datasets.
        //
        long prevX = Long.MIN_VALUE;
        long prevY = NAN_PIX;
        long maxY = prevY;
        long minY = prevY;

        // turn off antialias for vertical lines
        int origAntialias = graphics.getAntialias();
        int antialias = origAntialias;

        long startTime = System.currentTimeMillis();

        for (int i = first; i <= last; i++) {
            if ((i & 255)==0 && System.currentTimeMillis() - startTime > timeLimitMillis)
                return false; // timed out

            long x = mapping.toCanvasX(plot.transformX(dataset.getX(series, i)));
            long y = mapping.toCanvasY(plot.transformY(dataset.getY(series, i))); // note: this maps +-INF to +-MAXPIX, which works out just fine here

            // for testing:
            // if (i%5==0) y = NANPIX;
            // if (i%5==2 && prevX!=Integer.MIN_VALUE) x = prevX;

            // draw line
            if (y != NAN_PIX) {
                if (x != prevX) {
                    if (prevY != NAN_PIX) {
                        if (antialias != origAntialias) graphics.setAntialias(antialias= origAntialias);
                        LargeGraphics.drawLine(graphics, prevX, prevY, x, y);
                    }
                    minY = maxY = y;
                }
                else if (y < minY) {
                    if (antialias != SWT.OFF) graphics.setAntialias(antialias= SWT.OFF);
                    LargeGraphics.drawLine(graphics, x, minY, x, y);
                    minY = y;
                }
                else if (y > maxY) {
                    if (antialias != SWT.OFF) graphics.setAntialias(antialias= SWT.OFF);
                    LargeGraphics.drawLine(graphics, x, maxY, x, y);
                    maxY = y;
                }
                prevX = x;
            }
            else {
                prevX = Long.MIN_VALUE; // invalidate minX/maxX
            }
            prevY = y;
        }

        graphics.setAntialias(origAntialias);

        // and draw symbols
        int remainingTime = Math.max(0, timeLimitMillis - (int)(System.currentTimeMillis()-startTime));
        return plotSymbols(plot, series, graphics, mapping, symbol, remainingTime);
    }
}
