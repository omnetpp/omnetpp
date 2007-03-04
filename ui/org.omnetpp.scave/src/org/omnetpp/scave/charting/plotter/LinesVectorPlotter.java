package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		//
		// Performance optimization: avoid painting the same pixels over and over 
		// when drawing vertical lines. This results in magnitudes faster
		// execution for large datasets.
		//
		int prevX = mapping.toCanvasX(dataset.getXValue(series, 0));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, 0));
		int maxY = prevY;
		int minY = prevY;

		// n>1
		for (int i=1; i<n; i++) { //XXX paint cliprect only
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
