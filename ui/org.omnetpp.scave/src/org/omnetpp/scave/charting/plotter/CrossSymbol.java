/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;

/**
 * Draws a "cross" symbol.
 *
 * @author andras
 */
public class CrossSymbol extends ChartSymbol {
	private int size;

	public CrossSymbol() {
	}

	public CrossSymbol(int size) {
		super(size);
	}

	@Override
	public void setSizeHint(int sizeHint) {
		super.setSizeHint(sizeHint);
		size = (120*sizeHint+50)/100;
		size |= 1; // make it an odd number
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
			graphics.drawPoint(x-1, y-1);
			graphics.drawPoint(x-1, y+1);
			graphics.drawPoint(x+1, y-1);
			graphics.drawPoint(x+1, y+1);
		}
		else if (size<8) {
			int d = size/2;
			graphics.drawLine(x-d, y-d, x+d, y+d);
			graphics.drawLine(x-d, y+d, x+d, y-d);
		}
		else {
			int saved = graphics.getLineWidth();
			graphics.setLineWidth(size/4);
			//graphics.setLineCap(SWT.FLAT);
			int d = size/2;
			graphics.drawLine(x-d, y-d, x+d, y+d);
			graphics.drawLine(x-d, y+d, x+d, y-d);
			graphics.setLineWidth(saved);
		}
	}
}

