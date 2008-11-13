package org.omnetpp.scave.charting;

import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.scave.charting.VectorChart.LineProperties;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;

class LinePlot implements ILinePlot {
	
	private static final boolean debug = false;
	
	private VectorChart chart;
	private Rectangle rect = new Rectangle(0,0,1,1);
	private boolean smartMode = true; // whether smartModeLimit is enabled
	private int smartModeLimit = 10000; // turn off symbols if there're more than this amount of points on the plot
	
	public LinePlot(VectorChart chart) {
		this.chart = chart;
	}
	
	public IXYDataset getDataset() {
		return chart.getDataset();
	}
	
	public double transformX(double x) {
		return chart.transformX(x);
	}
	
	public double transformY(double y) {
		return chart.transformY(y);
	}
	
	public double inverseTransformX(double x) {
		return chart.inverseTransformX(x);
	}
	
	public double inverseTransformY(double y) {
		return chart.inverseTransformY(y);
	}
	
	
	public Rectangle getPlotRectangle() {
		return rect;
	}
	
	protected RectangularArea calculatePlotArea() {

		RectangularArea area = new RectangularArea(Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY,
													Double.NEGATIVE_INFINITY, Double.NEGATIVE_INFINITY);
		
		IXYDataset dataset = getDataset();

		if (dataset!=null && dataset.getSeriesCount() > 0) {
			
			area.minX = dataset.getMinX();
			area.minY = dataset.getMinY();
			area.maxX = dataset.getMaxX();
			area.maxY = dataset.getMaxY();
			
			// try to find the area by transforming the dataset range
			area = chart.transformArea(area);
			
			if (!area.isFinite()) {
				// some bounds are outside of the transformation domain,
				// so calculate the bounding box by transforming the points and
				// omitting infinite values
				long startTime = System.currentTimeMillis();
				long numOfPoints = 0;

				area.minX = Double.POSITIVE_INFINITY;
				area.minY = Double.POSITIVE_INFINITY;
				area.maxX = Double.NEGATIVE_INFINITY;
				area.maxY = Double.NEGATIVE_INFINITY;

				for (int series = 0; series < dataset.getSeriesCount(); series++) {
					int n = dataset.getItemCount(series);
					if (n > 0) {
						// X must be increasing
						area.minX = Math.min(area.minX, chart.transformX(dataset.getX(series, 0)));
						area.maxX = Math.max(area.maxX, chart.transformX(dataset.getX(series, n-1)));
						for (int i = 0; i < n; i++) {
							double y = chart.transformY(dataset.getY(series, i));
							if (!Double.isNaN(y) && !Double.isInfinite(y)) {
								area.minY = Math.min(area.minY, y);
								area.maxY = Math.max(area.maxY, y);
							}
						}
	
						numOfPoints += n;
					}
				}
				if (debug) {
					long duration = System.currentTimeMillis() - startTime;
					Debug.format("calculatePlotArea(): %d ms (%d points)%n", duration, numOfPoints);
				}
			}
		}
		
		if (area.minX > area.maxX) {
			area.minX = 0.0;
			area.maxX = 1.0;
		}
		if (area.minY > area.maxY) {
			area.minY = 0.0;
			area.maxY = 1.0;
		}
		
        area.minX = (area.minX>=0 ? 0 : area.minX-area.width()/80);
        area.maxX = (area.maxX<=0 ? 0 : area.maxX+area.width()/80);
        area.minY = (area.minY>=0 ? 0 : area.minY-area.height()/3);
        area.maxY = (area.maxY<=0 ? 0 : area.maxY+area.height()/3);
		
		return area;
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

					if (smartMode && plotter.getNumPointsInXRange(this, series, gc, coordsMapping) >= smartModeLimit) {
						//XXX this may have unwanted effects when caching is on,
						// i.e. parts of a line w/ symbols, other parts the SAME line w/o symbols....
						if (debug) Debug.println("\"smart mode\": turning off symbols");
						symbol = null;
					}

					plotter.plot(this, series, gc, coordsMapping, symbol);

					// if drawing is taking too long, display busy cursor
					if (System.currentTimeMillis() - startTime > 1000) {
						Cursor cursor = Display.getCurrent().getSystemCursor(SWT.CURSOR_WAIT);
						chart.getShell().setCursor(cursor);
						chart.setCursor(null); // crosshair cursor would override shell's busy cursor 
					}
				}
			}
			chart.getShell().setCursor(null);
			if (debug) Debug.println("plotting: "+(System.currentTimeMillis()-startTime)+" ms");
		}
	}
}
