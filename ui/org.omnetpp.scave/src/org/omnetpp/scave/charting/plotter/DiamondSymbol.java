/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;

/**
 * Draws a diamond symbol.
 * 
 * @author andras
 */
public class DiamondSymbol extends ChartSymbol {
	private int size;
	private int[] poly;
	private int[] work = new int[8];

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
		int d = (sizeHint*71+50)/100;
		poly = new int[] {-d+1,0,0,-d,d,0,0,d}; // with anti-alias OFF, +1 magic is needed to make it look symmetric
		 										//FIXME with ON, it looks asymmetric... HELP!!!
		//poly = new int[] {-d,0,0,-d,d,0,0,d}; XXX this will be asymmetric too, but WHY? 
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
		else {
			// manual translation; XXX try gc.setTransform(), maybe it's faster?
			work[0] = x + poly[0];
			work[1] = y + poly[1];
			work[2] = x + poly[2];
			work[3] = y + poly[3];
			work[4] = x + poly[4];
			work[5] = y + poly[5];
			work[6] = x + poly[6];
			work[7] = y + poly[7];
			gc.setBackground(gc.getForeground());
			gc.fillPolygon(work); //XXX make filled/unfilled version
		}
	}
}
