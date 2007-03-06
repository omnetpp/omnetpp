package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;

		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		//
		// Performance optimization: avoid painting the same pixels over and over 
		// when drawing vertical lines. This results in magnitudes faster
		// execution for large datasets.
		//
		int prevX = mapping.toCanvasX(dataset.getXValue(series, first));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, first));
		int maxY = prevY;
		int minY = prevY;

		// n>1
		for (int i=first+1; i<=last; i++) {
			//XXX note: the next 2 lines is about 90% of the runtime, gc.drawLine() is only 10%!
			//XXX so try to optimize using valueRange!
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));

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
			
			prevX = x;
			prevY = y;
		}

		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
