package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;

/**
 * Draws a diamond symbol.
 * 
 * @author andras
 */
public class DiamondSymbol extends ChartSymbol {
	private int size;
	private int[] poly;
	
	public DiamondSymbol() {
	}

	public DiamondSymbol(int size) {
		super(size);
	}

	@Override
	public void setSizeHint(int sizeHint) {
		super.setSizeHint(sizeHint);
		size = (sizeHint*141+50)/100;  // make same area as square; 1.41=sqrt(2)
		size |= 1; // make it an odd number
		int d = size/2;
		poly = new int[] {-d+1,0,0,-d,d,0,0,d}; // +1: magic to make it look symmetric
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
			graphics.translate(x, y);
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillPolygon(poly);
			graphics.translate(-x, -y);
		}
	}
}
