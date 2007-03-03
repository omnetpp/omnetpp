package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;


/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public abstract class VectorPlotter implements IVectorPlotter {
	
	/**
	 * Utility function to plot the symbols
	 */
	protected void plotSymbols(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		//
		// Performance optimization: with large datasets it occurs that the same symbol
		// on the screen is painted over and over. We eliminate this by keeping track of
		// symbols painted at the last x pixel coordinate. This easily results in 10x
		// or more performance improvement.
		//
		int n = dataset.getItemCount(series);
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevX = -1;
		for (int i=0; i<n; i++) { //XXX TODO: paint cliprect only, not the whole dataset!
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
