package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.common.canvas.ICoordsMapping;

/**
 * Vector plotter simply drawing pixels (and no lines or symbols). Note that despite 
 * common sense, LinesVectorPlotter is *faster* than this one, probably due to the
 * HashSet in plotSymbols().
 * 
 * @author Andras
 */
public class PointsVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		//
		// Note: profiling shows that substituting the gc.drawPoint() call
		// into the plotSymbols() would make no measurable difference in
		// performance, so we just invoke the stock drawSymbol() here.
		//
		plotSymbols(dataset, series, gc, mapping, new ChartSymbol() {
			public void drawSymbol(GC gc, int x, int y) {
				gc.drawPoint(x, y);
			}
		});
		
	}
}
