package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;

/**
 * Interface for chart symbol drawing classes
 * 
 * @author andras
 */
public interface IChartSymbol {

//XXX implementation note: fillOval(), fillPolygon() etc are more efficient than bltting images of the same size. see test code below. 
//	graphics.setBackgroundColor(new Color(null,100,0,0));
//	Image img = new Image(Display.getCurrent(), 11, 11);
//	GC gc = new GC(img);
//	gc.setBackground(new Color(null,100,0,0));
//	gc.fillOval(0, 0, 4, 4);
//
//	long start = System.currentTimeMillis();
//	for (int i=0; i<100000; i++)
//		graphics.drawImage(img, 100, 100);
//	Debug.println("images: "+(System.currentTimeMillis()-start));
//
//	start = System.currentTimeMillis();
//	int[] ii = new int[] {100,105,105,100,110,105,105,110};
//	for (int i=0; i<100000; i++)
//		graphics.fillPolygon(ii);
//	Debug.println("oval: "+(System.currentTimeMillis()-start));

	public int getSizeHint();
	public void setSizeHint(int size);
	public void drawSymbol(GC gc, int x, int y);
}
