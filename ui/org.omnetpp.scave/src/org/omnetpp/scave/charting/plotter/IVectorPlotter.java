package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.dataset.IXYDataset;

public interface IVectorPlotter {

	public int getNumPointsInXRange(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping);

	public void plot(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol); 
}
