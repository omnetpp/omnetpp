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
	double baseline = 0.0;
	
	RectangularArea bars[][] = new RectangularArea[0][];
	double transformedBaseline;
	
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
		
		int histCount = dataset.getSeriesCount();
		bars = new RectangularArea[histCount][];
		
		transformedBaseline = calculateBaseline();
		
		for (int series = 0; series < histCount; ++series) {
			int count = dataset.getCellsCount(series);
			bars[series] = new RectangularArea[count];
			for (int index = 0; index < count; ++index) {
				double xLeft = dataset.getCellMin(series, index);
				double xRight = dataset.getCellMax(series, index);
				double value = transformValue(dataset.getCellValue(series, index));
				double yTop = Math.max(value, transformedBaseline);
				double yBottom = Math.min(value, transformedBaseline);
				bars[series][index] = new RectangularArea(xLeft,yBottom,xRight,yTop);
				
				if (!Double.isNaN(yTop)) {
					maxY = Math.max(maxY, yTop);
				}
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
	
	private double calculateBaseline() {
		double baseline = getTransformedBaseline();
		
		if (Double.isInfinite(baseline)) {
			IHistogramDataset dataset = canvas.getDataset();
			int histCount = dataset.getSeriesCount();
			
			double newBaseline = baseline < 0.0 ? Double.POSITIVE_INFINITY : Double.NEGATIVE_INFINITY;
			for (int series = 0; series < histCount; ++series) {
				int cellCount = dataset.getCellsCount(series);
				for (int column = 0; column < cellCount; ++column) {
					double value = transformValue(dataset.getCellValue(series, column));
					if (!Double.isNaN(value) && !Double.isInfinite(value)) {
						if (baseline < 0.0)
							newBaseline = Math.min(newBaseline, value);
						else
							newBaseline = Math.max(newBaseline, value);
					}
				}
			}
			baseline = newBaseline;
		}
		
		return baseline;
	}

	Rectangle layout(GC gc, Rectangle area) {
		Assert.isNotNull(area);
		this.area = area;
		return area;
	}
	
	void draw(GC gc, ICoordsMapping coordsMapping) {
		switch (barType) {
		case Solid:
			gc.setForeground(ColorFactory.BLACK);
			for (int series = 0; series < bars.length; ++series) {
				gc.setBackground(getHistogramColor(series));
				for (int index = 0; index < bars[series].length; ++index) {
					double xl = bars[series][index].minX;
					double xr = bars[series][index].maxX;
					double yt = bars[series][index].maxY;
					double yb = bars[series][index].minY;
					
					if (yt != yb) {
						int left = Double.isInfinite(xl) ? area.x : coordsMapping.toCanvasX(xl);
						int right = Double.isInfinite(xr) ? area.right() : coordsMapping.toCanvasX(xr);
						int bottom = Double.isInfinite(yb) ? (yb<0?area.bottom():area.y) : coordsMapping.toCanvasY(yb);
						int top = Double.isInfinite(yt) ? (yt<0?area.bottom():area.y) : coordsMapping.toCanvasY(yt);
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
			for (int series = 0; series < bars.length; ++series) {
				gc.setForeground(getHistogramColor(series));
				int prevY = area.bottom();
				int cellCount = bars[series].length;
				int firstIndex = (Double.isInfinite(bars[series][0].minX) && bars[series][0].minY == bars[series][0].maxY ?
									1 : 0);
				int lastIndex = (Double.isInfinite(bars[series][cellCount-1].maxX) &&
									bars[series][cellCount-1].minY == bars[series][cellCount-1].maxY ?
									cellCount - 2 : cellCount - 1);
				int[] points = new int[6*(lastIndex-firstIndex+1) + 2]; // coordinates of 3*n+1 points
				int i = 0;
				for (int index = firstIndex ; index <= lastIndex; ++index) {
					double xl = bars[series][index].minX;
					double xr = bars[series][index].maxX;
					double yt = bars[series][index].maxY;
					double yb = bars[series][index].minY;
					double y = yt > transformedBaseline ? yt : yb;

					int left = Double.isInfinite(xl) ? area.x : coordsMapping.toCanvasX(xl);
					int right = Double.isInfinite(xr) ? area.right() : coordsMapping.toCanvasX(xr);
					int yy = Double.isInfinite(y) ? (y<0?area.bottom():area.y): coordsMapping.toCanvasY(y);
					
					points[i++] = left; points[i++] = prevY;
					points[i++] = left; points[i++] = yy;
					points[i++] = right; points[i++] = yy;
					
					if (index == lastIndex) {
						points[i++] = right; points[i++] = area.bottom();
					}

					prevY = yy;
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
			int index = findBin(bars[series], series, xx);
			if (index >= 0) {
				RectangularArea bar = bars[series][index];
				boolean isOver;
				switch (barType) {
				case Solid: isOver = bar.contains(xx, yy);	break;
				case Outline:
					double dy = yy > transformedBaseline ? yy - bar.maxY : yy - bar.minY;  
					isOver = Math.abs(canvas.toCanvasDistY(dy)) <= 2;
					break;
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
	
	private static int findBin(final RectangularArea[] bars, final int series, double x) {
		List<Double> binsList = new AbstractList<Double>() {
			@Override public Double get(int index) { return bars[index].minX; }
			@Override public int size()            { return bars.length; }
		};
		int index = Collections.binarySearch(binsList, x);
		if (index >= 0)
			return index;
		index = -index-1;
		if (0 < index && index < bars.length && x <= bars[index-1].maxX)
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
	
	protected double transformValue(double y) {
		if (canvas.transform != null)
			return canvas.transform.transformY(y);
		else
			return y;
	}
	
	protected double getTransformedBaseline() {
		double baseline = this.baseline;
		baseline = transformValue(baseline);
		return Double.isNaN(baseline) ? Double.NEGATIVE_INFINITY : baseline;
	}
}
