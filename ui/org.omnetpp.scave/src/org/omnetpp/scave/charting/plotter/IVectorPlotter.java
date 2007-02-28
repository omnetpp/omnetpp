package org.omnetpp.scave.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave.charting.ICoordsMapping;

public interface IVectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, ICoordsMapping mapping, IChartSymbol symbol); 
}
