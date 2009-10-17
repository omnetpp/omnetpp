/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;

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


	public void drawSymbol(GC gc, int x, int y) {
		if (size<=0) {
			// nothing
		}
		else if (size==1) {
			gc.drawPoint(x, y);
		}
		else if (size==2 || size==3) {
			gc.drawPoint(x-1, y);
			gc.drawPoint(x+1, y);
			gc.drawPoint(x, y-1);
			gc.drawPoint(x, y+1);
		}
		else {
			gc.setBackground(gc.getForeground());
			gc.fillOval(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
		}
	}
}
