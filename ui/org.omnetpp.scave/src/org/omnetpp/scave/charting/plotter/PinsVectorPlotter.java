package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class PinsVectorPlotter extends VectorPlotter {

	public double referenceLevel = 0;  // baseline for the pins
	
	public void plot(XYDataset dataset, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol) {
		int refy = mapping.toCanvasY(referenceLevel);
		int n = dataset.getItemCount(series);
		//XXX paint cliprect only
		for (int i=0; i<n; i++) {
			int x = mapping.toCanvasX(dataset.getXValue(series, i));
			int y = mapping.toCanvasY(dataset.getYValue(series, i));
			graphics.drawLine(x, refy, x, y);
			symbol.drawSymbol(graphics, x, y);
		}
	}
}
