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
//TODO: backward-sample-hold
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
		//FIXME handle: Double.isNaN(value) || Double.isInfinite(value)
		boolean prevIsNaN = Double.isNaN(dataset.getYValue(series, first));
		int prevX = mapping.toCanvasX(dataset.getXValue(series, first));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, first));
		int maxY = prevY;
		int minY = prevY;
		
		int[] dots = new int[] {1,2};
		gc.setLineDash(dots);
		//gc.setLineStyle(SWT.LINE_DOT);  // faster, but doesn't look as good

		// n>1
		for (int i = first+1; i <= last; i++) {
			double value = dataset.getYValue(series, i);
			boolean isNaN = Double.isNaN(value); // see isNaN handling later

			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(value); // note: this maps +-INF to +-MAXPIX, which works out just fine here
			
			if (x != prevX) {
				if (!prevIsNaN) {
					gc.setLineStyle(SWT.LINE_SOLID);
					gc.drawLine(prevX, prevY, x, prevY); // horizontal

					gc.setLineDash(dots);
					if (!isNaN)
						gc.drawLine(x, prevY, x, y); // vertical
				}
				minY = maxY = y;
			}
			else if (!isNaN) {
				// same x coord, only vertical line needs to be drawn
				if (y < minY) {
					gc.drawLine(x, minY, x, y);  // in lineDash(dots) mode 
					minY = y;
				}
				else if (y > maxY) {
					gc.drawLine(x, maxY, x, y);  // in lineDash(dots) mode
					maxY = y;
				}
			}
			
			prevIsNaN = isNaN;
			if (!isNaN) {  // condition is to handle case when first value on this x is NaN
				prevX = x;
				prevY = y;
			}
		}

		if (!prevIsNaN)
			gc.drawPoint(prevX, prevY);

		// draw symbols
		gc.setLineStyle(SWT.LINE_SOLID);
		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
