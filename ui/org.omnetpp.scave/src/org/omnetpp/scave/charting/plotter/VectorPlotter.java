package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.DatasetUtils;
import org.omnetpp.scave.charting.ICoordsMapping;


/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public abstract class VectorPlotter implements IVectorPlotter {

	public int[] indexRange(XYDataset dataset, int series, GC gc, ICoordsMapping mapping) {
		int n = dataset.getItemCount(series);
		Rectangle clip = gc.getClipping();
		int first = DatasetUtils.findXLowerLimit(dataset, series, mapping.fromCanvasX(clip.x));
		int last = DatasetUtils.findXUpperLimit(dataset, series, mapping.fromCanvasX(clip.x+clip.width));
		first = first<=0 ? 0 : first-1;
		last = last>=n-1 ? n-1 : last+1;
		return new int[] {first, last};
	}

	public int getNumPointsInXRange(XYDataset dataset, int series, GC gc, ICoordsMapping mapping) {
		int range[] = indexRange(dataset, series, gc, mapping);
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
	protected void plotSymbols(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		if (symbol == null)
			return;
		
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		// value range on the chart
		double[] valueRange = valueRange(gc, mapping, symbol);
		double lo = valueRange[0], hi = valueRange[1];
		
		//
		// Performance optimization: with large datasets it occurs that the same symbol
		// on the screen is painted over and over. We eliminate this by keeping track of
		// symbols painted at the last x pixel coordinate. This easily results in 10x
		// or more performance improvement.
		//
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevX = -1;
		for (int i = first; i <= last; i++) {
			double value = dataset.getYValue(series, i);
			if (value < lo || value > hi || Double.isNaN(value) || Double.isInfinite(value)) 
				continue; // value off-screen
			
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(value);
			
			if (prevX != x) {
				yset.clear();
				prevX = x;
				symbol.drawSymbol(gc, x, y);
				yset.add(y);
			}
			else {
				if (!yset.contains(y)) {
					symbol.drawSymbol(gc, x, y);
					yset.add(y);
				}
			}
		}
	}
}
