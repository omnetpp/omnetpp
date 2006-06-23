package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.jfree.data.xy.XYDataset;

public interface IVectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart); 
}
