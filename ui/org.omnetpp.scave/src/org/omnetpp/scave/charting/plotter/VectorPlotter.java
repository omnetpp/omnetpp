package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.dataset.DatasetUtils;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.IDatasetTransform;


/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public abstract class VectorPlotter implements IVectorPlotter {
	
	protected IDatasetTransform transform;
	
	public void setDatasetTransformation(IDatasetTransform transform) {
		this.transform = transform;
	}

	protected double transformX(double x) {
		return transform == null ? x : transform.transformX(x);
	}

	protected double transformY(double y) {
		return transform == null ? y :transform.transformY(y);
	}

	protected double inverseTransformX(double x) {
		return transform == null ? x :transform.inverseTransformX(x);
	}

	protected double inverseTransformY(double y) {
		return transform == null ? y :transform.inverseTransformY(y);
	}

	public int[] indexRange(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping) {
		int n = dataset.getItemCount(series);
		Rectangle clip = gc.getClipping();
		double left = inverseTransformX(mapping.fromCanvasX(clip.x));
		double right = inverseTransformY(mapping.fromCanvasX(clip.x+clip.width));
		int first = DatasetUtils.findXLowerLimit(dataset, series, left);
		int last = DatasetUtils.findXUpperLimit(dataset, series, right);
		first = first<=0 ? 0 : first-1;
		last = last>=n-1 ? n-1 : last+1;
		return new int[] {first, last};
	}

	public int getNumPointsInXRange(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping) {
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
	 * @param transform TODO
	 */
	protected void plotSymbols(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
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
		// symbols painted at the last x pixel coordinate. This easily results in 10x-100x
		// performance improvement.
		//
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevCanvasX = Integer.MIN_VALUE;
		for (int i = first; i <= last; i++) {
			double y = transformY(dataset.getY(series, i));
			if (y < lo || y > hi || Double.isNaN(y))  // even skip coord transform for off-screen values 
				continue;
			
			double x = transformX(dataset.getX(series, i));
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
