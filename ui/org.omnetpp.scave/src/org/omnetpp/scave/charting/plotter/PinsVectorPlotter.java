package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class PinsVectorPlotter extends VectorPlotter {

	public double referenceLevel = 0;  // baseline for the pins
	
	public void plot(XYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		int refy = mapping.toCanvasY(referenceLevel);
		int n = dataset.getItemCount(series);
		//XXX paint cliprect only
		for (int i=0; i<n; i++) {
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));
			gc.drawLine(x, refy, x, y);
			symbol.drawSymbol(gc, x, y);
		}
	}
}
