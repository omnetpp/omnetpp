package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		int prevX = mapping.toCanvasX(dataset.getXValue(series, 0));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, 0));
		symbol.drawSymbol(gc, prevX, prevY);

		// n>1
		//XXX paint cliprect only
		for (int i=1; i<n; i++) {
			int currentX = mapping.toCanvasX(dataset.getXValue(series, i));
			int currentY = mapping.toCanvasY(dataset.getYValue(series, i));
			gc.drawLine(prevX, prevY, currentX, currentY);
			symbol.drawSymbol(gc, currentX, currentY);
			prevX = currentX;
			prevY = currentY;
		}
	}
}
