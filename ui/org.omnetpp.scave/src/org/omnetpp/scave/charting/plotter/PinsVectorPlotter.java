package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

/**
 * Pins vector plotter
 * 
 * @author Andras
 */
public class PinsVectorPlotter extends VectorPlotter {

	protected double referenceLevel = 0;  // baseline for the pins
	
	public double getReferenceLevel() {
		return referenceLevel;
	}

	public void setReferenceLevel(double referenceLevel) {
		this.referenceLevel = referenceLevel;
	}

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		// value range on the chart
		double[] valueRange = valueRange(gc, mapping, symbol);
		double lo = valueRange[0], hi = valueRange[1];

		//
		// Performance optimization: avoid painting the same pixels over and over,
		// by maintaining prevX, minY and maxY. This results in magnitudes faster
		// execution for large datasets.
		//
		// Note that we could spare a few extra cycles by merging in symbol drawing
		// (instead of calling drawSymbols()), but since "pin" mode doesn't make much
		// sense (doesn't show much) for huge amounts of data points, we don't bother.
		//
		int refY = mapping.toCanvasY(referenceLevel);
		
		int prevX = -1;
		int maxY = refY;
		int minY = refY;
		
		// draw pins
		for (int i = first; i <= last; i++) {
			double value = dataset.getYValue(series, i);
			if ((referenceLevel < lo && value < lo) || (referenceLevel > hi && value > hi)) 
				continue; // pin completely off the screen
			
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));

			if (prevX != x) {
				gc.drawLine(x, refY, x, y);
				prevX = x;
				minY = Math.min(y, refY);
				maxY = Math.max(y, refY);
			}
			else if (y < minY) {
				gc.drawLine(x, minY, x, y);
				minY = y;
			}
			else if (y > maxY) {
				gc.drawLine(x, maxY, x, y);
				maxY = y;
			}
		}

		// and draw symbols
		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
