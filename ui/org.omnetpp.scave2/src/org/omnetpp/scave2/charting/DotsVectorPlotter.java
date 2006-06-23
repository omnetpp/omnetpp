package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.jfree.data.xy.XYDataset;

public class DotsVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart) {
		int n = dataset.getItemCount(series);
		//XXX paint cliprect only
		for (int i=0; i<n; i++) {
			int x = chart.toCanvasX(dataset.getXValue(series, i));
			int y = chart.toCanvasY(dataset.getYValue(series, i));
			graphics.drawPoint(x, y-1);
			graphics.drawPoint(x, y+1);
			graphics.drawPoint(x+1, y);
			graphics.drawPoint(x-1, y);
			graphics.drawPoint(x, y);
		}
	}
}
