package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave2.charting.VectorChart;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		int prevX = chart.toCanvasX(dataset.getXValue(series, 0));
		int prevY = chart.toCanvasY(dataset.getYValue(series, 0));
		if (n==1) {
			symbol.drawSymbol(graphics, prevX, prevY);
			return;
		}

		// n>1
		//XXX paint cliprect only
		for (int i=1; i<n; i++) {
			int currentX = chart.toCanvasX(dataset.getXValue(series, i));
			int currentY = chart.toCanvasY(dataset.getYValue(series, i));
			graphics.drawLine(prevX, prevY, currentX, currentY);
			symbol.drawSymbol(graphics, currentX, currentY);
			prevX = currentX;
			prevY = currentY;
		}
	}
}
