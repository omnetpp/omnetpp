package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;

/**
 * Vector plotter drawing symbols only (and no lines).
 * 
 * @author Andras
 */
public class DotsVectorPlotter extends VectorPlotter {

	public void plot(ILinePlot plot, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		plotSymbols(plot, series, gc, mapping, symbol);
	}
}
