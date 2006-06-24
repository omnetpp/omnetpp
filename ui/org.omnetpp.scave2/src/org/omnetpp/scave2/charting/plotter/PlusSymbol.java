package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;

/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public class PlusSymbol extends ChartSymbol {
	public PlusSymbol() {
	}

	public PlusSymbol(int size) {
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
			graphics.drawPoint(x, y+1);
		}
		else if (size<8) {
			int d = size/2;
			graphics.drawLine(x-d, y, x+d, y);
			graphics.drawLine(x, y-d, x, y+d);
		}
		else {
			int saved = graphics.getLineWidth();
			graphics.setLineWidth(size/4);
			//graphics.setLineCap(SWT.FLAT);
			int d = size/2;
			graphics.drawLine(x-d, y, x+d, y);
			graphics.drawLine(x, y-d, x, y+d);
			graphics.setLineWidth(saved);
		}
	}
}
