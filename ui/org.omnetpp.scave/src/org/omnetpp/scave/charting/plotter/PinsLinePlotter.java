/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.dataset.IXYDataset;

/**
 * Pins line plotter
 *
 * @author Andras
 */
public class PinsLinePlotter extends LinePlotter {

    protected double referenceLevel = 0;  // baseline for the pins

    public double getReferenceLevel() {
        return referenceLevel;
    }

    public void setReferenceLevel(double referenceLevel) {
        this.referenceLevel = referenceLevel;
    }

    public boolean plot(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol, int timeLimitMillis) {
        // dataset index range to iterate over
        int[] range = indexRange(plot, series, graphics, mapping);
        int first = range[0], last = range[1];

        // value range on the chart
        double[] valueRange = valueRange(graphics, mapping, symbol);
        double lo = valueRange[0], hi = valueRange[1];

        // chart y range in canvas coordinates
        //int[] yrange = canvasYRange(graphics, symbol);
        //int top = yrange[0], bottom = yrange[1];  // top < bottom

        //
        // Performance optimization: avoid painting the same pixels over and over,
        // by maintaining prevX, minY and maxY. This results in magnitudes faster
        // execution for large datasets.
        //
        // Note that we could spare a few extra cycles by merging in symbol drawing
        // (instead of calling drawSymbols()), but since "pin" mode doesn't make much
        // sense (doesn't show much) for huge amounts of data points, we don't bother.
        //
        Double transformedReferenceLevel = plot.transformY(referenceLevel);
        long refY = Double.isInfinite(transformedReferenceLevel) || Double.isNaN(transformedReferenceLevel) ?
                    plot.getPlotRectangle().bottom() :
                    mapping.toCanvasY(referenceLevel);

        long prevX = -1;
        long maxY = refY;
        long minY = refY;

        // We are drawing solid vertical lines, so antialiasing does not improve
        // the plot much, but it slows down the plotting by a factor of 2.
        int origAntialias = graphics.getAntialias();
        graphics.setAntialias(SWT.OFF);

        // draw pins
        IXYDataset dataset = plot.getDataset();
        long startTime = System.currentTimeMillis();

        for (int i = first; i <= last; i++) {
            if ((i & 255)==0 && System.currentTimeMillis() - startTime > timeLimitMillis)
                return false; // timed out

            double value = plot.transformY(dataset.getY(series, i));
            if ((transformedReferenceLevel < lo && value < lo) || (transformedReferenceLevel > hi && value > hi) || Double.isNaN(value) )
                continue; // pin is off-screen

            long x = mapping.toCanvasX(plot.transformX(dataset.getX(series, i)));
            long y = mapping.toCanvasY(value); // note: this maps +-INF to +-MAXPIX, which works out just fine here

            if (prevX != x) {
                LargeGraphics.drawLine(graphics, x, refY, x, y);
                prevX = x;
                minY = Math.min(y, refY);
                maxY = Math.max(y, refY);
            }
            else if (y < minY) {
                LargeGraphics.drawLine(graphics, x, minY, x, y);
                minY = y;
            }
            else if (y > maxY) {
                LargeGraphics.drawLine(graphics, x, maxY, x, y);
                maxY = y;
            }
        }

        // restore original antialias mode
        graphics.setAntialias(origAntialias);

        // and draw symbols
        int remainingTime = Math.max(0, timeLimitMillis - (int)(System.currentTimeMillis()-startTime));
        return plotSymbols(plot, series, graphics, mapping, symbol, remainingTime);
    }
}
