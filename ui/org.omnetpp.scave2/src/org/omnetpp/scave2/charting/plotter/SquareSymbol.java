package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;

/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public class SquareSymbol extends ChartSymbol {
	public SquareSymbol() {
	}

	public SquareSymbol(int size) {
		super(size);
	}

	public void drawSymbol(Graphics graphics, int x, int y) {
		if (size<=0) {
			// nothing
		}
		else if (size==1) {
			graphics.drawPoint(x, y);
		}
		else if (size==2) {
			graphics.drawPoint(x, y);
			graphics.drawPoint(x-1, y); // note -1 not +1 to be consistent with size/2 for larger sizes
			graphics.drawPoint(x, y-1);
			graphics.drawPoint(x-1, y-1);
		}
		else {
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillRectangle(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
		}
	}
}
