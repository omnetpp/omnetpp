package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;

/**
 * Declares utility functions for subclasses
 * 
 * @author andras
 */
public abstract class VectorPlotter implements IVectorPlotter {

//XXX note: fillOval(), fillPolygon() etc are more efficient than bltting images of the same size. see test code below. 
//	graphics.setBackgroundColor(new Color(null,100,0,0));
//	Image img = new Image(Display.getCurrent(), 11, 11);
//	GC gc = new GC(img);
//	gc.setBackground(new Color(null,100,0,0));
//	gc.fillOval(0, 0, 4, 4);
//
//	long start = System.currentTimeMillis();
//	for (int i=0; i<100000; i++)
//		graphics.drawImage(img, 100, 100);
//	System.out.println("images: "+(System.currentTimeMillis()-start));
//
//	start = System.currentTimeMillis();
//	int[] ii = new int[] {100,105,105,100,110,105,105,110};
//	for (int i=0; i<100000; i++)
//		graphics.fillPolygon(ii);
//	System.out.println("oval: "+(System.currentTimeMillis()-start));
	
	public static void drawSquare(Graphics graphics, int x, int y, int size) {
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
			graphics.fillRectangle(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
		}
	}

	public static void drawDiamond(Graphics graphics, int x, int y, int size) {
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
			graphics.fillPolygon(poly); //XXX make filled/unfilled version
			graphics.translate(-x, -y);
		}
	}
	
	public static void drawTriangle(Graphics graphics, int x, int y, int size) {
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
		}
		else {
			int d = size/2;
			int dd = (size*433+500)/1000;  // 0.433 = cos(30)/2
			int[] poly = new int[] {0,-d, -dd, d/2, dd, d/2};
			graphics.translate(x, y);
			graphics.fillPolygon(poly); //XXX make filled/unfilled version
			graphics.translate(-x, -y);
		}
	}

	public static void drawCircle(Graphics graphics, int x, int y, int size) {
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
		else if (size<8) {
			graphics.drawOval(x-size/2, y-size/2, size, size);
		}
		else {
			int saved = graphics.getLineWidth();
			graphics.setLineWidth(size/4);
			graphics.drawOval(x-size/2, y-size/2, size, size); //XXX make filled/unfilled version
			graphics.setLineWidth(saved);
		}
	}

	public static void drawPlus(Graphics graphics, int x, int y, int size) {
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
			graphics.setLineCap(SWT.FLAT);
			int d = size/2;
			graphics.drawLine(x-d, y, x+d, y);
			graphics.drawLine(x, y-d, x, y+d);
			graphics.setLineWidth(saved);
		}
	}

	public static void drawCross(Graphics graphics, int x, int y, int size) {
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
			graphics.setLineCap(SWT.FLAT);
			int d = size/2;
			graphics.drawLine(x-d, y-d, x+d, y+d);
			graphics.drawLine(x-d, y+d, x+d, y-d);
			graphics.setLineWidth(saved);
		}
	}
	
}
