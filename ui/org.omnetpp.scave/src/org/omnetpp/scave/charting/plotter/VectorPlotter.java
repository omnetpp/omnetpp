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
	
	/**
	 * Utility function to plot the symbols
	 */
	protected void plotSymbols(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		//
		// Performance optimization: with large datasets it occurs that the same symbol
		// on the screen is painted over and over. We eliminate this by keeping track of
		// symbols painted at the last x pixel coordinate. This easily results in 10x
		// or more performance improvement.
		//
		int n = dataset.getItemCount(series);
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevX = -1;
		for (int i=first; i<=last; i++) {
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));
			
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
