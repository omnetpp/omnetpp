package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.dataset.DatasetUtils;
import org.omnetpp.scave.charting.dataset.IXYDataset;


/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public abstract class VectorPlotter implements IVectorPlotter {
	
	public int[] indexRange(ILinePlot plot, int series, GC gc, ICoordsMapping mapping) {
		IXYDataset dataset = plot.getDataset();
		int n = dataset.getItemCount(series);
		Rectangle clip = gc.getClipping();
		double left = plot.inverseTransformX(mapping.fromCanvasX(clip.x));
		double right = plot.inverseTransformY(mapping.fromCanvasX(clip.x+clip.width));
		int first = DatasetUtils.findXLowerLimit(dataset, series, left);
		int last = DatasetUtils.findXUpperLimit(dataset, series, right);
		first = first<=0 ? 0 : first-1;
		last = last>=n-1 ? n-1 : last+1;
		return new int[] {first, last};
	}

	public int getNumPointsInXRange(ILinePlot plot, int series, GC gc, ICoordsMapping mapping) {
		int range[] = indexRange(plot, series, gc, mapping);
		return range[1] - range[0] + 1;
	}

	public int[] canvasYRange(GC gc, IChartSymbol symbol) {
		Rectangle clip = gc.getClipping();
		int extra = symbol==null ? 0 : 2*symbol.getSizeHint(); // to be safe
		int top = clip.y - extra;
		int bottom = clip.y + clip.height + extra;
		return new int[] {top, bottom};
	}

	public double[] valueRange(GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		Rectangle clip = gc.getClipping();
		int extra = symbol==null ? 0 : 2*symbol.getSizeHint(); // to be safe
		double hi = mapping.fromCanvasY(clip.y - extra);
		double lo = mapping.fromCanvasY(clip.y + clip.height + extra);
		return new double[] {lo, hi};
	}
	              
	/**
	 * Utility function to plot the symbols
	 */
	protected void plotSymbols(ILinePlot plot, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		if (symbol == null)
			return;
		
		// dataset index range to iterate over 
		IXYDataset dataset = plot.getDataset();
		int[] range = indexRange(plot, series, gc, mapping);
		int first = range[0], last = range[1];

		// value range on the chart
		double[] valueRange = valueRange(gc, mapping, symbol);
		double lo = valueRange[0], hi = valueRange[1];
		
		//
		// Performance optimization: with large datasets it occurs that the same symbol
		// on the screen is painted over and over. We eliminate this by keeping track of
		// symbols painted at the last x pixel coordinate. This easily results in 10x-100x
		// performance improvement.
		//
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevCanvasX = Integer.MIN_VALUE;
		for (int i = first; i <= last; i++) {
			double y = plot.transformY(dataset.getY(series, i));
			if (y < lo || y > hi || Double.isNaN(y))  // even skip coord transform for off-screen values 
				continue;
			
			double x = plot.transformX(dataset.getX(series, i));
			int canvasX = mapping.toCanvasX(x);
			int canvasY = mapping.toCanvasY(y);
			
			if (prevCanvasX != canvasX) {
				yset.clear();
				prevCanvasX = canvasX;
				symbol.drawSymbol(gc, canvasX, canvasY);
				yset.add(canvasY);
			}
			else {
				if (!yset.contains(canvasY)) {
					symbol.drawSymbol(gc, canvasX, canvasY);
					yset.add(canvasY);
				}
			}
		}
	}
}
