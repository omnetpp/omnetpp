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
		
		int prevX = mapping.toCanvasX(dataset.getXValue(series, 0));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, 0));
		int maxY = prevY;
		int minY = prevY;

		int[] dots = new int[] {1,2};
		//graphics.setLineStyle(SWT.LINE_DOT);
		gc.setLineDash(dots); // looks better, but much slower

		// n>1
		//XXX paint cliprect only
		for (int i=1; i<n; i++) {
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
		
		//graphics.drawPoint(prevX, prevY); XXX needed?

		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
