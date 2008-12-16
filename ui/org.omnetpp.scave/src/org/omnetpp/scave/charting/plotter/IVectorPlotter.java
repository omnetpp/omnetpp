/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.plotter;

import org.eclipse.swt.graphics.GC;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.scave.charting.ILinePlot;

public interface IVectorPlotter {
	
	public int getNumPointsInXRange(ILinePlot plot, int series, GC gc, ICoordsMapping mapping);

	public void plot(ILinePlot plot, int series, GC gc, ICoordsMapping mapping, IChartSymbol symbol);
}
