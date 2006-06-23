package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;
import org.jfree.data.xy.XYDataset;

public class PointsVectorPlotter implements IVectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart) {
		int n = dataset.getItemCount(series);
		for (int i=0; i<n; i++) {
			graphics.drawPoint(
					chart.toCanvasX(dataset.getXValue(series, i)),
					chart.toCanvasY(dataset.getYValue(series, i)));
		}
	}
}
