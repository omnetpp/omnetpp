package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

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

		//
		// Performance optimization: avoid painting the same pixels over and over,
		// by maintaining prevX, minY and maxY. This results in magnitudes faster
		// execution for large datasets.
		//
		int refY = mapping.toCanvasY(referenceLevel);
		
		int prevX = -1;
		int maxY = refY;
		int minY = refY;
		
		for (int i=first; i<=last; i++) { 
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

		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
