package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;

/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public class CircleSymbol extends ChartSymbol {
	public CircleSymbol() {
	}

	public CircleSymbol(int size) {
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
			graphics.drawPoint(x-1, y);
			graphics.drawPoint(x+1, y);
			graphics.drawPoint(x, y-1);
			graphics.drawPoint(x, y+1);
		}
		else if (size<8) {
			graphics.drawOval(x-size/2, y-size/2, size, size);
		}
		else {
			int saved = graphics.getLineWidth();
			graphics.setLineWidth(size/4);
			graphics.drawOval(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
			graphics.setLineWidth(saved);
		}
	}
}
