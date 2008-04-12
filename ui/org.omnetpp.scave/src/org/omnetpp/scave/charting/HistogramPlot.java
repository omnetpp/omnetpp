package org.omnetpp.scave.charting;

import java.util.AbstractList;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartProperties.HistogramBar;

class HistogramPlot {
	
	HistogramChartCanvas canvas;
	Rectangle area = Rectangle.SINGLETON;
	
	HistogramBar barType = ChartDefaults.DEFAULT_HIST_BAR;
	
	HistogramPlot(HistogramChartCanvas canvas) {
		this.canvas = canvas;
	}
	
	Rectangle getArea() {
		return area;
	}
	
	void setBarType(HistogramBar barType) {
		this.barType = barType;
	}
	
	protected RectangularArea calculatePlotArea() {
		IHistogramDataset dataset = canvas.getDataset();
		double minX = Double.POSITIVE_INFINITY, maxX = Double.NEGATIVE_INFINITY;
		double minY = 0.0, maxY = 0.0;
		boolean underflowCell = false, overflowCell = false; 
		
		for (int series = 0; series < dataset.getSeriesCount(); ++series) {
			int count = dataset.getCellsCount(series);
			for (int index = 0; index < count; ++index) {
				double xLeft = dataset.getCellMin(series, index);
				double xRight = dataset.getCellMax(series, index);
				double value = dataset.getCellValue(series, index);
				if (!Double.isNaN(value))
					maxY = Math.max(maxY, value);
				if (index <= 1) {
					if (Double.isInfinite(xLeft))
						underflowCell = true;
					else
						minX = Math.min(minX, xLeft);
				}
				else if (index >= count - 2) {
					if (Double.isInfinite(xRight))
						overflowCell = true;
					else
						maxX = Math.max(maxX, xRight);
				}
					
			}
		}
		
		if (minX >= maxX || minY >= maxY) // empty
			return new RectangularArea(0.0, 0.0, 1.0, 1.0);
		
		double width = maxX - minX;
		double height = maxY - minY;
		if (underflowCell)
			minX -= width / 10;
		if (overflowCell)
			maxX += width / 10;
		maxY += height / 4;
		
		return new RectangularArea(minX, minY, maxX, maxY);
	}

	Rectangle layout(GC gc, Rectangle area) {
		Assert.isNotNull(area);
		this.area = area;
		return area;
	}
	
	void draw(GC gc, ICoordsMapping coordsMapping) {
		IHistogramDataset dataset = canvas.getDataset();
		switch (barType) {
		case Solid:
			gc.setForeground(ColorFactory.BLACK);
			for (int series = 0; series < dataset.getSeriesCount(); ++series) {
				gc.setBackground(getHistogramColor(series));
				for (int index = 0; index < dataset.getCellsCount(series); ++index) {
					double xl = dataset.getCellMin(series, index);
					double xr = dataset.getCellMax(series, index);
					double y = dataset.getCellValue(series, index);
					if (y > 0.0) {
						int left = Double.isInfinite(xl) ? area.x : coordsMapping.toCanvasX(xl);
						int right = Double.isInfinite(xr) ? area.right() : coordsMapping.toCanvasX(xr);
						int bottom = coordsMapping.toCanvasY(0);
						int top = coordsMapping.toCanvasY(y);
						gc.fillRectangle(left, top, right-left, bottom-top);
						gc.drawRectangle(left, top, right-left, bottom-top);
					}
				}
			}
			break;
		case Outline:
			gc.setLineWidth(4);
			gc.setLineStyle(SWT.LINE_SOLID);
			gc.setAlpha(128);
			//gc.setLineJoin(SWT.JOIN_MITER);
			for (int series = 0; series < dataset.getSeriesCount(); ++series) {
				gc.setForeground(getHistogramColor(series));
				int prevTop = area.bottom();
				int cellCount = dataset.getCellsCount(series);
				int firstIndex = (Double.isInfinite(dataset.getCellMin(series, 0)) && dataset.getCellValue(series, 0) == 0.0 ?
									1 : 0);
				int lastIndex = (Double.isInfinite(dataset.getCellMax(series, cellCount-1)) && dataset.getCellValue(series, cellCount-1) == 0.0 ?
									cellCount - 2 : cellCount - 1);
				int[] points = new int[6*(lastIndex-firstIndex+1) + 2]; // coordinates of 3*n+1 points
				int i = 0;
				for (int index = firstIndex ; index <= lastIndex; ++index) {
					double xl = dataset.getCellMin(series, index);
					double xr = dataset.getCellMax(series, index);
					double y = dataset.getCellValue(series, index);
					int left = Double.isInfinite(xl) ? area.x : coordsMapping.toCanvasX(xl);
					int right = Double.isInfinite(xr) ? area.right() : coordsMapping.toCanvasX(xr);
					int top = coordsMapping.toCanvasY(y);
					
					points[i++] = left; points[i++] = prevTop;
					points[i++] = left; points[i++] = top;
					points[i++] = right; points[i++] = top;
					
					if (index == lastIndex) {
						points[i++] = right; points[i++] = area.bottom();
					}

					prevTop = top;
				}
				gc.drawPolyline(points);
			}
			break;
		}
	}
	
	void updateLegend(ILegend legend) {
		legend.clearItems();
		IHistogramDataset dataset = canvas.getDataset();
		IChartSymbol symbol = new SquareSymbol();
		for (int series = 0; series < dataset.getSeriesCount(); ++series) {
			legend.addItem(getHistogramColor(series), dataset.getSeriesKey(series), symbol, false);
		}
	}
	
	String getTooltipText(int x, int y, SizeConstraint outSizeConstraint) {
		Integer[] seriesAndIndeces = findHistogramColumns(x,y);
		if (seriesAndIndeces.length == 0)
			return null;
		IHistogramDataset dataset = canvas.getDataset();
		StringBuilder result = new StringBuilder();
		for (int i = 0; i < seriesAndIndeces.length; i+=2) {
			int series = seriesAndIndeces[i];
			int index = seriesAndIndeces[i+1];
			String key = dataset.getSeriesKey(series);
			double min = dataset.getCellMin(series, index);
			double max = dataset.getCellMax(series, index);
			double value = dataset.getCellValue(series, index);
			if (dataset.isDiscrete(series))
				result.append(String.format("%s [%s]=%.0f", key, Math.round((max+min)/2), value));
			else
				result.append(String.format("%s [%.3g,%.3g]=%.3g", key, min, max, value));
			if (i != seriesAndIndeces.length - 1)
				result.append("<br>");
		}
		
		return result.toString();
	}
	
	private Integer[] findHistogramColumns(int x, int y) {
		double xx = canvas.fromCanvasX(x);
		double yy = canvas.fromCanvasY(y);
		IHistogramDataset dataset = canvas.getDataset();
		List<Integer> result = new ArrayList<Integer>();
		for (int series = 0; series < dataset.getSeriesCount(); ++series) {
			int index = findBin(dataset, series, xx);
			if (index >= 0) {
				double value = dataset.getCellValue(series, index);
				boolean isOver;
				switch (barType) {
				case Solid: isOver = (yy <= value);	break;
				case Outline: isOver = Math.abs(y - canvas.toCanvasY(value)) <= 2; break;
				default: isOver = false; Assert.isTrue(false, "Unknown HistogramBar type: " + barType); break;
				}
				if (isOver) {
					result.add(series);
					result.add(index);
				}
			}
		}
		return result.toArray(new Integer[result.size()]);
	}
	
	private static int findBin(final IHistogramDataset dataset, final int series, double x) {
		final int size = dataset.getCellsCount(series);
		List<Double> binsList = new AbstractList<Double>() {
			@Override public Double get(int index) { return dataset.getCellMin(series, index); }
			@Override public int size()            { return size; }
		};
		int index = Collections.binarySearch(binsList, x);
		if (index >= 0)
			return index;
		index = -index-1;
		if (0 < index && index < size && x <= dataset.getCellMax(series, index-1))
			return index-1;
		else	
			return -1;
	}
	
	protected Color getHistogramColor(int series) {
		String key = canvas.getDataset().getSeriesKey(series);
		RGB color = canvas.getHistogramColor(key);
		if (color != null)
			return new Color(null, color);
		else
			return ColorFactory.getGoodDarkColor(series);
	}
}
