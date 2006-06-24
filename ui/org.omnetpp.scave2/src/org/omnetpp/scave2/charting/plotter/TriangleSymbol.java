package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;

/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public class TriangleSymbol extends ChartSymbol {
	public TriangleSymbol() {
	}

	public TriangleSymbol(int size) {
		super(size);
	}

	public void drawSymbol(Graphics graphics, int x, int y) {
		if (size<=0) {
			// nothing
		}
		else if (size==1) {
			graphics.drawPoint(x, y);
		}
		else if (size==2 || size==3) {
			graphics.drawPoint(x, y);
			graphics.drawPoint(x-1, y);
			graphics.drawPoint(x+1, y);
			graphics.drawPoint(x, y-1);
		}
		else {
			int d = size/2;
			int dd = (size*433+500)/1000;  // 0.433 = cos(30)/2
			int[] poly = new int[] {0,-d, -dd, d/2, dd, d/2};
			graphics.translate(x, y);
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillPolygon(poly); //XXX make filled/unfilled version
			graphics.translate(-x, -y);
		}
	}
}
