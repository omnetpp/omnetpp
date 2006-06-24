package org.omnetpp.scave2.charting.plotter;

import org.eclipse.draw2d.Graphics;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.scave2.charting.VectorChart;

public interface IVectorPlotter {

	public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart, IChartSymbol symbol); 
}
