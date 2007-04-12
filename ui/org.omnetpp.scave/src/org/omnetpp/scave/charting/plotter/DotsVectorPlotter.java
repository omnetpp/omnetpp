package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.dataset.IXYDataset;

/**
 * Vector plotter drawing symbols only (and no lines).
 * 
 * @author Andras
 */
public class DotsVectorPlotter extends VectorPlotter {

	public void plot(IXYDataset dataset, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol) {
		plotSymbols(dataset, series, gc, mapping, symbol);
	}
}
