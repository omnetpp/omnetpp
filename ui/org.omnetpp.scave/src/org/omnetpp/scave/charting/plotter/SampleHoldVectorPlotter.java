package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class SampleHoldVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		//
		// Performance optimization: avoid painting the same pixels over and over,
		// by maintaining prevX, minY and maxY.
		//
		int prevX = mapping.toCanvasX(dataset.getXValue(series, first));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, first));
		int maxY = prevY;
		int minY = prevY;

		int[] dots = new int[] {1,2};
		//graphics.setLineStyle(SWT.LINE_DOT);
		gc.setLineDash(dots); // looks better, but much slower

		// n>1
		//XXX paint cliprect only
		for (int i=first+1; i<=last; i++) {
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));
			
			if (x != prevX) {
				gc.setLineStyle(SWT.LINE_SOLID);
				gc.drawLine(prevX, prevY, x, prevY);

				gc.setLineDash(dots); 
				gc.drawLine(x, prevY, x, y);
				
				minY = maxY = y;
			}
			else if (y < minY) {
				gc.drawLine(x, minY, x, y);  // in "dots" mode
				minY = y;
			}
			else if (y > maxY) {
				gc.drawLine(x, maxY, x, y);  // in "dots" mode
				maxY = y;
			}
			
			prevX = x;
			prevY = y;
		}
		
		gc.drawPoint(prevX, prevY);

		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
