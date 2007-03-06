package org.omnetpp.scave.charting.plotter;

import java.util.HashSet;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;

		// Note: for performance (this is most commonly used plotter), we don't use plotSymbols() 
		// from the base class, but draw lines and symbols in a single loop instead
		
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		// calculating y range 
		int[] yrange = canvasYRange(gc, symbol);
		int top = yrange[0], bottom = yrange[1];  // top < bottom
		
		//
		// Performance optimization: avoid painting the same pixels over and over 
		// when drawing vertical lines. This results in magnitudes faster
		// execution for large datasets.
		//
		int prevX = mapping.toCanvasX(dataset.getXValue(series, first));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, first));
		int maxY = prevY;
		int minY = prevY;

		// used for preventing painting the same symbol on the same pixels over and over.
		HashSet<Integer> yset = new HashSet<Integer>();
		
		// n>1
		for (int i=first+1; i<=last; i++) {
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));

 			// draw line
			if (x != prevX) {
				gc.drawLine(prevX, prevY, x, y);
				minY = maxY = y;
			}
			else if (y < minY) {
				gc.drawLine(x, minY, x, y);
				minY = y;
			}
			else if (y > maxY) {
				gc.drawLine(x, maxY, x, y);
				maxY = y;
			}

  			// draw symbol (see VectorPlotter.plotSymbols() for explanation)
			if (symbol != null && top <= y && y <= bottom) {
				if (prevX != x) {
					yset.clear();
					symbol.drawSymbol(gc, x, y);
					yset.add(y);
				}
				else if (!yset.contains(y)) {
					symbol.drawSymbol(gc, x, y);
					yset.add(y);
				} 
				else {
					// already plotted
				}
			}
			
			prevX = x;
			prevY = y;
		}
	}
}
