/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;

/**
 * Interface for chart symbol drawing classes
 *
 * @author andras
 */
public interface IPlotSymbol {

//XXX implementation note: fillOval(), fillPolygon() etc are more efficient than bltting images of the same size. see test code below.
//  graphics.setBackgroundColor(new Color(null,100,0,0));
//  Image img = new Image(Display.getCurrent(), 11, 11);
//  Graphics graphics = new Graphics(img);
//  graphics.setBackground(new Color(null,100,0,0));
//  graphics.fillOval(0, 0, 4, 4);
//
//  long start = System.currentTimeMillis();
//  for (int i=0; i<100000; i++)
//      graphics.drawImage(img, 100, 100);
//  Debug.println("images: "+(System.currentTimeMillis()-start));
//
//  start = System.currentTimeMillis();
//  int[] ii = new int[] {100,105,105,100,110,105,105,110};
//  for (int i=0; i<100000; i++)
//      graphics.fillPolygon(ii);
//  Debug.println("oval: "+(System.currentTimeMillis()-start));

    public int getSizeHint();
    public void setSizeHint(int size);
    public void drawSymbol(Graphics graphics, long x, long y);
}
