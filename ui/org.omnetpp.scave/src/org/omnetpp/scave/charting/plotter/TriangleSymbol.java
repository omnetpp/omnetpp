package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.graphics.Color;

/**
 * Draws a triangle symbol.
 * 
 * @author andras
 */
public class TriangleSymbol extends ChartSymbol {
	private int height;
	private int[] poly;
	
	public TriangleSymbol() {
	}

	public TriangleSymbol(int size) {
		super(size);
	}

	@Override
	public void setSizeHint(int sizeHint) {
		super.setSizeHint(sizeHint);
		// equal-sized triangle whose area is sizeHint^2
		height = (132*sizeHint+50)/100; // 1.32 = sqrt4(3)
		int halfside = (76*sizeHint+50)/100; // 0.76 = 1 / sqrt4(3)
		int off = (84*sizeHint+50)/100;
		poly = new int[] {-halfside, (height-off), 0, -off, halfside, (height-off)};
	}
	
	public void drawSymbol(Graphics graphics, int x, int y) {
		if (sizeHint<=0) {
			// nothing
		}
		else if (sizeHint==1) {
			graphics.drawPoint(x, y);
		}
		else if (sizeHint==2 || sizeHint==3) {
			graphics.drawPoint(x, y);
			graphics.drawPoint(x-1, y);
			graphics.drawPoint(x+1, y);
			graphics.drawPoint(x, y-1);
		}
		else {
			graphics.translate(x, y);
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillPolygon(poly);
			graphics.translate(-x, -y);
		}
	}
}
