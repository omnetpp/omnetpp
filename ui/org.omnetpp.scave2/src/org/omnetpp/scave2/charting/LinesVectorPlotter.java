package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;
import org.jfree.data.xy.XYDataset;

public class LinesVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		int prevX = chart.toCanvasX(dataset.getXValue(series, 0));
		int prevY = chart.toCanvasY(dataset.getYValue(series, 0));
		if (n==1) {
			graphics.drawPoint(prevX, prevY);
			return;
		}

		// n>1
		//XXX paint cliprect only
		for (int i=1; i<n; i++) {
			int currentX = chart.toCanvasX(dataset.getXValue(series, i));
			int currentY = chart.toCanvasY(dataset.getYValue(series, i));
			graphics.drawLine(prevX, prevY, currentX, currentY);
			prevX = currentX;
			prevY = currentY;
		}
	}
}
