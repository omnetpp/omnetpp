package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;

/**
 * @author andras
 */
public class DiamondSymbol extends ChartSymbol {
	public DiamondSymbol() {
	}

	public DiamondSymbol(int size) {
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
		else {
			int d = size/2;
			int[] poly = new int[] {-d,0,0,-d,d,0,0,d}; //XXX cache it
			graphics.translate(x, y);
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillPolygon(poly); //XXX make filled/unfilled version
			graphics.translate(-x, -y);
		}
	}
}
