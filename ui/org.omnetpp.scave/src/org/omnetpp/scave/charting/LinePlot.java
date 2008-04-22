package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.scave.charting.VectorChart.LineProperties;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;

class LinePlot {
	
	private static final boolean debug = false;
	
	private VectorChart chart;
	private Rectangle rect = new Rectangle(0,0,1,1);
	private boolean smartMode = true; // whether smartModeLimit is enabled
	private int smartModeLimit = 10000; // turn off symbols if there're more than this amount of points on the plot
	
	public LinePlot(VectorChart chart) {
		this.chart = chart;
	}
	
	protected IXYDataset getDataset() {
		return chart.getDataset();
	}
	
	protected Rectangle getPlotRectangle() {
		return rect;
	}
	
	protected RectangularArea calculatePlotArea() {
		double minX = Double.POSITIVE_INFINITY;
		double minY = Double.POSITIVE_INFINITY;
		double maxX = Double.NEGATIVE_INFINITY;
		double maxY = Double.NEGATIVE_INFINITY;
		
		IXYDataset dataset = getDataset();

		if (dataset!=null && dataset.getSeriesCount() > 0) {
			// calculate bounding box
			if (chart.transform != null) {
				long startTime = System.currentTimeMillis();
				long numOfPoints = 0;
				for (int series = 0; series < dataset.getSeriesCount(); series++) {
					int n = dataset.getItemCount(series);
					if (n > 0) {
						// X must be increasing
						minX = Math.min(minX, chart.transformX(dataset.getX(series, 0)));
						maxX = Math.max(maxX, chart.transformX(dataset.getX(series, n-1)));
						for (int i = 0; i < n; i++) {
							double y = chart.transformY(dataset.getY(series, i));
							if (!Double.isNaN(y) && !Double.isInfinite(y)) {
								minY = Math.min(minY, y);
								maxY = Math.max(maxY, y);
							}
						}
	
						numOfPoints += n;
					}
				}
				if (debug) {
					long duration = System.currentTimeMillis() - startTime;
					System.out.format("calculatePlotArea(): %d ms (%d points)%n", duration, numOfPoints);
				}
			}
			else {
				minX = dataset.getMinX();
				maxX = dataset.getMaxX();
				minY = dataset.getMinY();
				maxY = dataset.getMaxY();
			}
		}
		
		if (minX > maxX) {
			minX = 0.0;
			maxX = 1.0;
		}
		if (minY > maxY) {
			minY = 0.0;
			maxY = 1.0;
		}
		
        double width = maxX - minX;
        double height = maxY - minY;
        
        minX = (minX>=0 ? 0 : minX-width/80);
		maxX = (maxX<=0 ? 0 : maxX+width/80);
		minY = (minY>=0 ? 0 : minY-height/3);
		maxY = (maxY<=0 ? 0 : maxY+height/3);
		
		return new RectangularArea(minX, minY, maxX, maxY);
	}
	
	protected Rectangle layout(GC gc, Rectangle area) {
		this.rect = area.getCopy();
		return area;
	}
	
	
	protected void draw(GC gc, ICoordsMapping coordsMapping) {
		if (getDataset() != null) {
			IXYDataset dataset = getDataset();
			long startTime = System.currentTimeMillis();
			for (int series=0; series<dataset.getSeriesCount(); series++) {
				LineProperties props = chart.getLineProperties(series);
				if (props.getDisplayLine()) {

					IVectorPlotter plotter = props.getPlotter();
					IChartSymbol symbol = props.getSymbol();
					Color color = props.getColor();
					chart.resetDrawingStylesAndColors(gc);
					gc.setAntialias(chart.antialias ? SWT.ON : SWT.OFF);
					gc.setForeground(color);
					gc.setBackground(color);

					if (smartMode && plotter.getNumPointsInXRange(dataset, series, gc, coordsMapping) >= smartModeLimit) {
						//XXX this may have unwanted effects when caching is on,
						// i.e. parts of a line w/ symbols, other parts the SAME line w/o symbols....
						if (debug) System.out.println("\"smart mode\": turning off symbols");
						symbol = null;
					}

					plotter.plot(dataset, series, gc, coordsMapping, symbol);

					// if drawing is taking too long, display busy cursor
					if (System.currentTimeMillis() - startTime > 1000) {
						Cursor cursor = Display.getCurrent().getSystemCursor(SWT.CURSOR_WAIT);
						chart.getShell().setCursor(cursor);
						chart.setCursor(null); // crosshair cursor would override shell's busy cursor 
					}
				}
			}
			chart.getShell().setCursor(null);
			if (debug) System.out.println("plotting: "+(System.currentTimeMillis()-startTime)+" ms");
		}
	}
}
