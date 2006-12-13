package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.VectorChart;

public class PinsVectorPlotter extends VectorPlotter {

	public double referenceLevel = 0;  // baseline for the pins
	
	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart, IChartSymbol symbol) {
		int refy = chart.toCanvasY(referenceLevel);
		int n = dataset.getItemCount(series);
		//XXX paint cliprect only
		for (int i=0; i<n; i++) {
			int x = chart.toCanvasX(dataset.getXValue(series, i));
			int y = chart.toCanvasY(dataset.getYValue(series, i));
			graphics.drawLine(x, refy, x, y);
			symbol.drawSymbol(graphics, x, y);
		}
	}
}
