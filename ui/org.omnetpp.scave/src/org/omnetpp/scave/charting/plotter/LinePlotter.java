/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.dataset.DatasetUtils;
import org.omnetpp.scave.charting.dataset.IXYDataset;


/**
 * Declares utility functions for subclasses
 *
 * @author andras
 */
public abstract class LinePlotter implements ILinePlotter {

    public int[] indexRange(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping) {
        IXYDataset dataset = plot.getDataset();
        int n = dataset.getItemCount(series);
        Rectangle clip = GraphicsUtils.getClip(graphics);
        double left = plot.inverseTransformX(mapping.fromCanvasX(clip.x));
        double right = plot.inverseTransformX(mapping.fromCanvasX(clip.x+clip.width));
        int first = DatasetUtils.findXLowerLimit(dataset, series, left);
        int last = DatasetUtils.findXUpperLimit(dataset, series, right);
        first = first<=0 ? 0 : first-1;
        last = last>=n-1 ? n-1 : last+1;
        return new int[] {first, last};
    }

    public int[] canvasYRange(Graphics graphics, IChartSymbol symbol) {
        Rectangle clip = GraphicsUtils.getClip(graphics);
        int extra = symbol==null ? 0 : 2*symbol.getSizeHint(); // to be safe
        int top = clip.y - extra;
        int bottom = clip.y + clip.height + extra;
        return new int[] {top, bottom};
    }

    public double[] valueRange(Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol) {
        Rectangle clip = GraphicsUtils.getClip(graphics);
        int extra = symbol==null ? 0 : 2*symbol.getSizeHint(); // to be safe
        double hi = mapping.fromCanvasY(clip.y - extra);
        double lo = mapping.fromCanvasY(clip.y + clip.height + extra);
        return new double[] {lo, hi};
    }

    /**
     * Utility function to plot the symbols
     */
    protected boolean plotSymbols(ILinePlot plot, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol, int timeLimitMillis) {
        if (symbol == null)
            return true;

        // dataset index range to iterate over
        IXYDataset dataset = plot.getDataset();
        int[] range = indexRange(plot, series, graphics, mapping);
        int first = range[0], last = range[1];

        // value range on the chart
        double[] valueRange = valueRange(graphics, mapping, symbol);
        double lo = valueRange[0], hi = valueRange[1];

        //
        // Performance optimization: with large datasets it occurs that the same symbol
        // on the screen is painted over and over. We eliminate this by keeping track of
        // symbols painted at the last x pixel coordinate. This easily results in 10x-100x
        // performance improvement.
        //
        long startTime = System.currentTimeMillis();
        HashSet<Long> yset = new HashSet<Long>();
        long prevCanvasX = Long.MIN_VALUE;
        for (int i = first; i <= last; i++) {
            if ((i & 255)==0 && System.currentTimeMillis() - startTime > timeLimitMillis)
                return false; // timed out

            double y = plot.transformY(dataset.getY(series, i));
            if (y < lo || y > hi || Double.isNaN(y))  // even skip coord transform for off-screen values
                continue;

            double x = plot.transformX(dataset.getX(series, i));
            long canvasX = mapping.toCanvasX(x);
            long canvasY = mapping.toCanvasY(y);

            if (prevCanvasX != canvasX) {
                yset.clear();
                prevCanvasX = canvasX;
                symbol.drawSymbol(graphics, canvasX, canvasY);
                yset.add(canvasY);
            }
            else {
                if (!yset.contains(canvasY)) {
                    symbol.drawSymbol(graphics, canvasX, canvasY);
                    yset.add(canvasY);
                }
            }
        }
        return true;
    }
}
