package org.omnetpp.scave2.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.xy.XYDataset;

import sun.java2d.loops.DrawRect;

public class VectorChart extends ZoomableCachingCanvas {
	private static final Color INSETS_BACKGROUND_COLOR = new Color(null, 255, 255, 128);
	private static final Color INSETS_LINE_COLOR = new Color(null, 0,0,0);
	
	private XYDataset dataset;

	public VectorChart(Composite parent, int style) {
		super(parent, style);
		super.setInsets(new Insets(10,30,10,10));
	}

	public XYDataset getDataset() {
		return dataset;
	}

	public void setDataset(XYDataset dataset) {
		this.dataset = dataset;
		calculateArea();
		clearCanvasCacheAndRedraw();
	}

	private void calculateArea() {
		if (dataset==null || dataset.getSeriesCount()==0) {
			setArea(0,0,0,0);
			return;
		}
		double minX = Double.MAX_VALUE;
		double minY = Double.MAX_VALUE;
		double maxX = Double.MIN_VALUE;
		double maxY = Double.MIN_VALUE;
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			int n = dataset.getItemCount(series);
			if (n>0) {
				// X must be increasing
				minX = Math.min(minX, dataset.getXValue(series, 0));
				maxX = Math.max(maxX, dataset.getXValue(series, n-1));
				for (int i=0; i<n; i++) {
					double y = dataset.getYValue(series, i);
					minY = Math.min(minY, y);
					maxY = Math.max(maxY, y);
				}
			}
		}
		setArea(minX, minY, maxX, maxY);
	}

	@Override
	protected void beforePaint() {
	}

	@Override
	protected void paintCachableLayer(Graphics graphics) {
		graphics.setForegroundColor(new Color(null,0,0,0));
		graphics.drawRectangle(
				toCanvasX(minX),
				toCanvasY(minY),
				10,10);
		graphics.drawRectangle(
				toCanvasX(maxX),
				toCanvasY(maxY),
				10,10);
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			int n = dataset.getItemCount(series);
			for (int i=0; i<n; i++) {
				graphics.drawPoint(
						toCanvasX(dataset.getXValue(series, i)),
						toCanvasY(dataset.getYValue(series, i)));
			}
		}
	}

	@Override
	protected void paintNoncachableLayer(Graphics graphics) {
		Insets insets = getInsets();
		graphics.drawText("X range: "+minX+".."+maxX+"   Y range: "+minY+".."+maxY, insets.left, insets.top);

		// draw insets border
		graphics.setForegroundColor(INSETS_BACKGROUND_COLOR);
		graphics.setBackgroundColor(new Color(null, 255, 255, 180));
		graphics.fillRectangle(0, 0, getWidth(), insets.top); // top
		graphics.fillRectangle(0, getHeight()-insets.bottom, getWidth(), insets.bottom); // bottom
		graphics.fillRectangle(0, 0, insets.left, getHeight()); // left
		graphics.fillRectangle(getWidth()-insets.right, 0, insets.right, getHeight()); // right

		graphics.setForegroundColor(INSETS_LINE_COLOR);
		graphics.drawRectangle(insets.left-1, insets.top-1, getWidth()-insets.getWidth()+1, getHeight()-insets.getHeight()+1);
		
	}
}
