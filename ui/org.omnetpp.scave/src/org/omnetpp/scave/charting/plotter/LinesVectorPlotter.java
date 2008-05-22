package org.omnetpp.scave.charting.plotter;

import static org.omnetpp.common.canvas.ICoordsMapping.NAN_PIX;

import java.util.HashSet;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.dataset.IXYDataset;

/**
 * Vector plotter that connects data points with lines.
 * 
 * @author Andras
 */
public class LinesVectorPlotter extends VectorPlotter {

	public void plot(ILinePlot plot, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		IXYDataset dataset = plot.getDataset();
		int n = dataset.getItemCount(series);
		if (n==0)
			return;

		// Note: for performance (this is most commonly used plotter), we don't use plotSymbols() 
		// from the base class, but draw lines and symbols in a single loop instead
		
		// dataset index range to iterate over 
		int[] range = indexRange(plot, series, gc, mapping);
		int first = range[0], last = range[1];

		// chart y range in canvas coordinates 
		int[] yrange = canvasYRange(gc, symbol);
		int top = yrange[0], bottom = yrange[1];  // top < bottom
		
		
		// Performance optimization: avoid painting the same pixels over and over 
		// when drawing vertical lines. This results in magnitudes faster
		// execution for large datasets.
		//
		int prevX = Integer.MIN_VALUE;
		int prevY = NAN_PIX;
		int maxY = prevY;
		int minY = prevY;

		// turn off antialias for vertical lines
		int origAntialias = gc.getAntialias();
		
		// used for preventing painting the same symbol on the same pixels over and over.
		HashSet<Integer> yset = new HashSet<Integer>();
		int prevSymbolX = Integer.MIN_VALUE;
		
		for (int i = first; i <= last; i++) {
			int x = mapping.toCanvasX(plot.transformX(dataset.getX(series, i)));
			int y = mapping.toCanvasY(plot.transformY(dataset.getY(series, i))); // note: this maps +-INF to +-MAXPIX, which works out just fine here

			// for testing: 
			// if (i%5==0) y = NANPIX;
			// if (i%5==2 && prevX!=Integer.MIN_VALUE) x = prevX;
			
 			// draw line
			if (y != NAN_PIX) {
				if (x != prevX) {
					if (prevY != NAN_PIX)
						gc.drawLine(prevX, prevY, x, y);
					minY = maxY = y;
				}
				else if (y < minY) {
					gc.setAntialias(SWT.OFF);
					gc.drawLine(x, minY, x, y);
					gc.setAntialias(origAntialias);
					minY = y;
				}
				else if (y > maxY) {
					gc.setAntialias(SWT.OFF);
					gc.drawLine(x, maxY, x, y);
					gc.setAntialias(origAntialias);
					maxY = y;
				}
				prevX = x;
			}
			else {
				prevX = Integer.MIN_VALUE; // invalidate minX/maxX
			}
			prevY = y;
			
  			// draw symbol (see VectorPlotter.plotSymbols() for explanation on yset-based optimization)
			// note: top <= y <= bottom condition also filters out NaNs
			if (symbol != null && top <= y && y <= bottom) {
				if (prevSymbolX != x) {
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
			prevSymbolX = x;
		}
	}
}
