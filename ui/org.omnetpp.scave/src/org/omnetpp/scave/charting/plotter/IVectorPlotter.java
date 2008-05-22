package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;

public interface IVectorPlotter {
	
	public int getNumPointsInXRange(ILinePlot plot, int series, GC gc, ICoordsMapping mapping);

	public void plot(ILinePlot plot, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol);
}
