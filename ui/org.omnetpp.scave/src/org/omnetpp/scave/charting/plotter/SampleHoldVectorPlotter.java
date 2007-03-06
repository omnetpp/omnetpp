package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

/**
 * Sample-hold vector plotter
 * 
 * @author Andras
 */
public class SampleHoldVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		// dataset index range to iterate over 
		int[] range = indexRange(dataset, series, gc, mapping);
		int first = range[0], last = range[1];

		// value range on the chart
		double[] valueRange = valueRange(gc, mapping, symbol);
		double lo = valueRange[0], hi = valueRange[1];

		//
		// Performance optimization: avoid painting the same pixels over and over,
		// by maintaining prevX, minY and maxY.
		//
		// Note that we could spare a few extra cycles by merging in symbol drawing
		// (instead of calling drawSymbols()), but since "pin" mode doesn't make much
		// sense (doesn't show much) for huge amounts of data points, we don't bother.
		//
		int prevX = mapping.toCanvasX(dataset.getXValue(series, first));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, first));
		int maxY = prevY;
		int minY = prevY;

		int[] dots = new int[] {1,2};
		gc.setLineDash(dots);
		//gc.setLineStyle(SWT.LINE_DOT);  // faster, but doesn't look as good

		// n>1
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
				gc.drawLine(x, minY, x, y);  // in lineDash(dots) mode 
				minY = y;
			}
			else if (y > maxY) {
				gc.drawLine(x, maxY, x, y);  // in lineDash(dots) mode
				maxY = y;
			}
			
			prevX = x;
			prevY = y;
		}
		gc.drawPoint(prevX, prevY);

		// draw symbols
		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
