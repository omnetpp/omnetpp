package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public class SampleHoldVectorPlotter extends VectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol) {
		int n = dataset.getItemCount(series);
		if (n==0)
			return;
		
		int prevX = mapping.toCanvasX(dataset.getXValue(series, 0));
		int prevY = mapping.toCanvasY(dataset.getYValue(series, 0));
		symbol.drawSymbol(graphics, prevX, prevY);

		// n>1
		//XXX paint cliprect only
		int[] dots = new int[] {1,2};
		for (int i=1; i<n; i++) {
			int currentX = mapping.toCanvasX(dataset.getXValue(series, i));
			int currentY = mapping.toCanvasY(dataset.getYValue(series, i));
			
			graphics.setLineStyle(SWT.LINE_SOLID);
			symbol.drawSymbol(graphics, currentX, currentY);
			graphics.drawLine(prevX, prevY, currentX, prevY);
			//graphics.setLineStyle(SWT.LINE_DOT);
			//graphics.setLineStyle(SWT.LINE_DASHDOTDOT);
			graphics.setLineDash(dots); // looks better, but much slower
			graphics.drawLine(currentX, prevY, currentX, currentY);
			
			prevX = currentX;
			prevY = currentY;
		}
		//graphics.drawPoint(prevX, prevY); XXX needed?
	}
}
