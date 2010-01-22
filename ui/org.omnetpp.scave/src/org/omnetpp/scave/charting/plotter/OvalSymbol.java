/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;

/**
 * Draws a "oval" symbol.
 *
 * @author andras
 */
public class OvalSymbol extends ChartSymbol {
	private int size;

	public OvalSymbol() {
	}

	public OvalSymbol(int size) {
		super(size);
	}

	@Override
	public void setSizeHint(int sizeHint) {
		super.setSizeHint(sizeHint);
		size = (sizeHint*113+50)/100;  // make same area as square; 1.13=2/sqrt(pi)
		size |= 1;  // make an odd number
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
		else {
			graphics.setBackgroundColor(graphics.getForegroundColor());
			graphics.fillOval(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
		}
	}
}
