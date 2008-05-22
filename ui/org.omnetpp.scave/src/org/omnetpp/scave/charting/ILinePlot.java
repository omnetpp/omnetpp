package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.scave.charting.dataset.IXYDataset;

public interface ILinePlot {
	public IXYDataset getDataset();
	public Rectangle getPlotRectangle();
	public double transformX(double x);
	public double transformY(double y);
	public double inverseTransformX(double x);
	public double inverseTransformY(double y);
}
