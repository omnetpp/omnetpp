/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;

/**
 * Draws a "plus" symbol.
 *
 * @author andras
 */
public class PlusSymbol extends ChartSymbol {
	private int size;

	public PlusSymbol() {
	}

	public PlusSymbol(int size) {
		super(size);
	}

	@Override
	public void setSizeHint(int sizeHint) {
		super.setSizeHint(sizeHint);
		size = (134*sizeHint+50)/100; // 3/sqrt(5) ~ 1.34
		size |= 1; // make it an odd number
	}

	public void drawSymbol(GC gc, int x, int y) {
		if (size<=0) {
			// nothing
		}
		else if (size==1) {
			gc.drawPoint(x, y);
		}
		else if (size==2 || size==3) {
			gc.drawPoint(x, y);
			gc.drawPoint(x-1, y);
			gc.drawPoint(x+1, y);
			gc.drawPoint(x, y-1);
			gc.drawPoint(x, y+1);
		}
		else if (size<8) {
			int d = size/2;
			gc.drawLine(x-d, y, x+d, y);
			gc.drawLine(x, y-d, x, y+d);
		}
		else {
			int saved = gc.getLineWidth();
			gc.setLineWidth(size/4);
			//graphics.setLineCap(SWT.FLAT);
			int d = size/2;
			gc.drawLine(x-d, y, x+d, y);
			gc.drawLine(x, y-d, x, y+d);
			gc.setLineWidth(saved);
		}
	}
}
