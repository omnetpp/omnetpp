package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.dataset.IXYDataset;

/**
 * Sample-hold vector plotter
 * 
 * @author Andras
 */
public class SampleHoldVectorPlotter extends VectorPlotter {
	
	boolean backward;
	
	public SampleHoldVectorPlotter(boolean backward) {
		this.backward = backward;
	}

	public void plot(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
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
		// Note that we could spare a few extra cycles by merging in symbol drawing
		// (instead of calling drawSymbols()), but since "pin" mode doesn't make much
		// sense (doesn't show much) for huge amounts of data points, we don't bother.
		//
		int prevX = mapping.toCanvasX(transformX(dataset.getX(series, first)));
		int prevY = mapping.toCanvasY(transformY(dataset.getY(series, first)));
		boolean prevIsNaN = Double.isNaN(transformY(dataset.getY(series, first)));
		int maxY = prevY;
		int minY = prevY;
		
		int[] dots = new int[] {1,2};
		gc.setLineDash(dots);
		//gc.setLineStyle(SWT.LINE_DOT);  // faster, but doesn't look as good
		
		if (!prevIsNaN && backward)
			gc.drawPoint(prevX, prevY);

		for (int i = first+1; i <= last; i++) {
			double value = transformY(dataset.getY(series, i));

			// for testing: 
			//if (i%5==0) value = 0.0/0.0; //NaN
			
			boolean isNaN = Double.isNaN(value); // see isNaN handling later

			int x = mapping.toCanvasX(transformX(dataset.getX(series, i)));
			int y = mapping.toCanvasY(value); // note: this maps +-INF to +-MAXPIX, which works out just fine here

			// for testing:
			//if (i%5==1) x = prevX;
			
			if (x != prevX) {
				if (!isNaN && backward) {
					gc.setLineStyle(SWT.LINE_SOLID);
					gc.drawLine(prevX, y, x, y); // horizontal

					gc.setLineDash(dots);
					if (!prevIsNaN) {
						gc.drawLine(prevX, prevY, prevX, y); // vertical
					}
				}
				else if (!prevIsNaN && !backward) { // forward
					gc.setLineStyle(SWT.LINE_SOLID);
					gc.drawLine(prevX, prevY, x, prevY); // horizontal

					gc.setLineDash(dots);
					if (!isNaN) {
						gc.drawLine(x, prevY, x, y); // vertical
					}
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

			if (!isNaN) {  // condition is to handle case when first value on this x is NaN
				prevX = x;
				prevY = y;
			}
			prevIsNaN = isNaN;
		}

		if (!prevIsNaN && !backward)
			gc.drawPoint(prevX, prevY);

		// draw symbols
		gc.setLineStyle(SWT.LINE_SOLID);
		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
