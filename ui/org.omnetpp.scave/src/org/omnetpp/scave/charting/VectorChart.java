package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_INSETS_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_INSETS_LINE_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LINE_STYLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.jfree.data.general.Dataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties.LineStyle;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.plotter.CrossSymbol;
import org.omnetpp.scave.charting.plotter.DiamondSymbol;
import org.omnetpp.scave.charting.plotter.DotsVectorPlotter;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.LinesVectorPlotter;
import org.omnetpp.scave.charting.plotter.OvalSymbol;
import org.omnetpp.scave.charting.plotter.PinsVectorPlotter;
import org.omnetpp.scave.charting.plotter.PlusSymbol;
import org.omnetpp.scave.charting.plotter.PointsVectorPlotter;
import org.omnetpp.scave.charting.plotter.SampleHoldVectorPlotter;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.plotter.TriangleSymbol;


/**
 * Line chart.
 */
public class VectorChart extends ChartCanvas {
	
	private OutputVectorDataset dataset = null;

	private LinearAxis xAxis = new LinearAxis(this, false);
	private LinearAxis yAxis = new LinearAxis(this, true);
	private Map<String, LineProperties> lineProperties = new HashMap<String,LineProperties>();
	private CrossHair crosshair = new CrossHair();

	private Color insetsBackgroundColor = DEFAULT_INSETS_BACKGROUND_COLOR;
	private Color insetsLineColor = DEFAULT_INSETS_LINE_COLOR;
	
	private int layoutDepth = 0; // how many layoutChart() calls are on the stack
	private int repaintCounter;
	private boolean debug = false;

	private static final String KEY_ALL = null;
	
	static class LineProperties {
		SymbolType symbolType;
		Integer symbolSize;
		LineStyle lineStyle;
	}
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		lineProperties.put(KEY_ALL, new LineProperties());
	}
	
	public OutputVectorDataset getDataset() {
		return dataset;
	}

	@Override
	public void setDataset(Dataset dataset) {
		if (dataset != null && !(dataset instanceof OutputVectorDataset))
			throw new IllegalArgumentException("must be an OutputVectorDataset");

		this.dataset = (OutputVectorDataset)dataset;
		updateLegend();
		calculateArea();
		chartChanged();
	}
	
	private void updateLegend() {
		legend.clearLegendItems();
		if (dataset != null) {
			for (int i = 0; i < dataset.getSeriesCount(); ++i) {
				legend.addLegendItem(getLineColor(i), dataset.getSeriesKey(i).toString());
			}
		}
	}
	
	/*==================================
	 *          Properties
	 *==================================*/
	public void setProperty(String name, String value) {
		Assert.isLegal(name != null);
		// Titles
		if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToSwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setTickLabelFont(Converter.stringToSwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			; //TODO
		// Axes
		else if (PROP_X_AXIS_MIN.equals(name))
			setXMin(Converter.stringToDouble(value));
		else if (PROP_X_AXIS_MAX.equals(name))
			setXMax(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MIN.equals(name))
			setYMin(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MAX.equals(name))
			setYMax(Converter.stringToDouble(value));
		else if (PROP_X_AXIS_LOGARITHMIC.equals(name))
			; //TODO
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			; //TODO
		else if (PROP_XY_INVERT.equals(name))
			; //TODO
		else if (PROP_XY_GRID.equals(name))
			setGridVisibility(Converter.stringToBoolean(value));
		// Lines
		else if (name.startsWith(PROP_SYMBOL_TYPE))
			setSymbolType(getKey(name), Converter.stringToEnum(value, SymbolType.class));
		else if (name.startsWith(PROP_SYMBOL_SIZE))
			setSymbolSize(getKey(name), Converter.stringToInteger(value));
		else if (name.startsWith(PROP_LINE_TYPE))
			setLineStyle(getKey(name), Converter.stringToEnum(value, LineStyle.class));
		else
			super.setProperty(name, value);
	}
	
	private String getKey(String name) {
		int index = name.indexOf('/');
		return index >= 0 ? name.substring(index + 1) : KEY_ALL;
	}

	protected LineProperties getLineProperties(String key) {
		return lineProperties.get(key);
	}
	
	protected LineProperties getDefaultLineProperties() {
		return lineProperties.get(KEY_ALL);
	}
	
	protected LineProperties getOrCreateLineProperties(String key) {
		LineProperties properties = lineProperties.get(key);
		if (properties == null)
			lineProperties.put(key, properties = new LineProperties());
		return properties;
	}

	public void setXAxisTitle(String value) {
		xAxis.setTitle(value != null ? value : DEFAULT_X_AXIS_TITLE);
		chartChanged();
	}
	
	public void setYAxisTitle(String value) {
		yAxis.setTitle(value != null ? value : DEFAULT_Y_AXIS_TITLE);
		chartChanged();
	}
	
	public void setAxisTitleFont(Font value) {
		if (value != null) {
			xAxis.setTitleFont(value);
			yAxis.setTitleFont(value);
			chartChanged();
		}
	}

	public LineStyle getLineStyle (String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.lineStyle == null)
			props = getDefaultLineProperties();
		return props.lineStyle != null ? props.lineStyle : DEFAULT_LINE_STYLE;
	}

	public void setLineStyle(String key, LineStyle type) {
		LineProperties props = getOrCreateLineProperties(key);
		props.lineStyle = type;
		chartChanged();
	}

	public SymbolType getSymbolType(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolType == null)
			props = getDefaultLineProperties();
		return props.symbolType != null ? props.symbolType : DEFAULT_SYMBOL_TYPE;
	}
	
	public void setSymbolType(String key, SymbolType symbolType) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolType = symbolType;
		chartChanged();
	}
	
	public int getSymbolSize(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolSize == null)
			props = getDefaultLineProperties();
		return props.symbolSize != null ? props.symbolSize : DEFAULT_SYMBOL_SIZE;
	}
	
	public void setSymbolSize(String key, Integer size) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolSize = size;
		chartChanged();
	}
	
	public void setXMin(Double value) {
		if (value != null)
			setArea(value, getMinY(), getMaxX(), getMaxY());
		chartChanged();
	}
	
	public void setXMax(Double value) {
		if (value != null)
			setArea(getMinX(), getMinY(), value, getMaxY());
		chartChanged();
	}
	
	public void setYMin(Double value) {
		if (value != null)
			setArea(getMinX(), value, getMaxX(), getMaxY());
		chartChanged();
	}
	
	public void setYMax(Double value) {
		if (value != null)
			setArea(getMinX(), getMinY(), getMaxX(), value);
		chartChanged();
	}
	
	public void setGridVisibility(Boolean value) {
		boolean b = value != null ? value : false;
		xAxis.setGridVisible(b);
		yAxis.setGridVisible(b);
		chartChanged();
	}
	
	public void setTickLabelFont(Font font) {
		if (font != null) {
			xAxis.setTickFont(font);
			yAxis.setTickFont(font);
		}
		chartChanged();
	}
	
	private void calculateArea() {
		if (dataset==null || dataset.getSeriesCount()==0)
			return;
	
		// calculate bounding box
		long startTime = System.currentTimeMillis();
		double minX = Double.MAX_VALUE;
		double minY = Double.MAX_VALUE;
		double maxX = Double.MIN_VALUE;
		double maxY = Double.MIN_VALUE;
		for (int series = 0; series < dataset.getSeriesCount(); series++) {
			int n = dataset.getItemCount(series);
			if (n > 0) {
				// X must be increasing
				minX = Math.min(minX, dataset.getXValue(series, 0));
				maxX = Math.max(maxX, dataset.getXValue(series, n-1));
				for (int i = 0; i < n; i++) {
					double y = dataset.getYValue(series, i);
					minY = Math.min(minY, y);
					maxY = Math.max(maxY, y);
				}
			}
		}
        double width = maxX - minX;
        double height = maxY - minY;
		long duration = System.currentTimeMillis() - startTime;
		System.out.println("calculateArea(): "+duration+" ms");
        
        // set the chart area, leaving some room around the data lines
        setArea((minX>=0 ? 0 : minX-width/80), (minY>=0 ? 0 : minY-height/3), 
        		(maxX<=0 ? 0 : maxX+width/80), (maxY<=0 ? 0 : maxY+height/3));
	}

	@Override
	protected void layoutChart() {
		// prevent nasty infinite layout recursions
		if (layoutDepth>0)
			return; 
		
		layoutDepth++;
		GC gc = new GC(Display.getCurrent());
		System.out.println("layoutChart(), level "+layoutDepth);

		try {
			// preserve zoomed-out state while resizing
			boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
			boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

			// Calculate space occupied by title and legend and set insets accordingly
			Rectangle area = new Rectangle(getClientArea());
			Rectangle remaining = title.layout(gc, area);
			remaining = legend.layout(gc, remaining);

			Rectangle mainArea = remaining.getCopy();
			Insets insetsToMainArea = new Insets();
			xAxis.layoutHint(gc, mainArea, insetsToMainArea);
			// postpone yAxis.layoutHint() as it wants to use coordinate mapping which is not yet set up (to calculate ticks)
			insetsToMainArea.left = 50; insetsToMainArea.right = 30; // initial estimate for y axis

			// tentative plotArea calculation (y axis ticks width missing from the picture yet)
			Rectangle plotArea = mainArea.getCopy().crop(insetsToMainArea);
			setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

			if (shouldZoomOutX)
				zoomToFitX();
			if (shouldZoomOutY)
				zoomToFitY();
			validateZoom(); //Note: scrollbar.setVisible() triggers Resize too

			// now the coordinate mapping is set up, so the y axis knows what tick labels
			// will appear, and can calculate the occupied space from the longest tick label.
			yAxis.layoutHint(gc, mainArea, insetsToMainArea);

			// now we have the final insets, set it everywhere again 
			xAxis.setLayout(mainArea, insetsToMainArea);
			yAxis.setLayout(mainArea, insetsToMainArea);
			crosshair.layout(gc, plotArea);

			plotArea = mainArea.getCopy().crop(insetsToMainArea);
			//FIXME how to handle it when plotArea.height/width comes out negative??
			setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

			if (shouldZoomOutX)
				zoomToFitX();
			if (shouldZoomOutY)
				zoomToFitY();
			validateZoom(); //Note: scrollbar.setVisible() triggers Resize too
		} 
		catch (Throwable e) {
			ScavePlugin.logError(e);
		}
		finally {
			gc.dispose();
			layoutDepth--;
		}
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		resetDrawingStylesAndColors(gc);
		xAxis.drawGrid(gc);
		yAxis.drawGrid(gc);

		if (dataset != null) {
			resetDrawingStylesAndColors(gc);
			gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
			gc.setLineStyle(SWT.LINE_SOLID);
			long startTime = System.currentTimeMillis();
			for (int series=0; series<dataset.getSeriesCount(); series++) {
				String key = dataset.getSeriesKey(series).toString();
				IVectorPlotter plotter = getPlotter(key);
				IChartSymbol symbol = getSymbol(key);
				Color color = getLineColor(series);
				gc.setForeground(color);
				gc.setBackground(color);

				plotter.plot(dataset, series, gc, getOptimizedCoordinateMapper(), symbol);
			}
			System.out.println("plotting: "+(System.currentTimeMillis()-startTime)+" ms");

			if (debug) {
				repaintCounter++;
				gc.setForeground(ColorFactory.getGoodColor(repaintCounter));
				gc.setBackground(ChartDefaults.DEFAULT_BACKGROUND_COLOR);
				Rectangle clip = new Rectangle(gc.getClipping());
				for (int x = clip.x - clip.x%100; x<clip.right(); x+=100) {
					for (int y = clip.y - clip.y%100; y<clip.bottom(); y+=100) {
						gc.drawLine(x, y, x+10, y+20);
						gc.drawText("paint#"+repaintCounter+" ("+canvasToVirtualX(x)+","+canvasToVirtualY(y)+")", x+10, y+15);
					}
				}
			}
		}
	}
	
	private IVectorPlotter getPlotter(String key) {
		LineStyle style = getLineStyle(key);
		switch (style) {
		case None: return new DotsVectorPlotter();
		case Linear: return new LinesVectorPlotter();
		case Step: return new SampleHoldVectorPlotter();
		case Pins: return new PinsVectorPlotter();
		case Points: return new PointsVectorPlotter();
		case Dots: return new DotsVectorPlotter();
		default: throw new IllegalArgumentException("unknown line style: " + style);
		}
	}
	
	private IChartSymbol getSymbol(String key) {
		SymbolType type = getSymbolType(key);
		int size = getSymbolSize(key);
		switch (type) {
		case None: return null;
		case Cross: return new CrossSymbol(size);
		case Diamond: return new DiamondSymbol(size);
		case Oval: return new OvalSymbol(size);
		case Plus: return new PlusSymbol(size);
		case Square: return new SquareSymbol(size);
		case Triangle: return new TriangleSymbol(size);
		default: throw new IllegalArgumentException("unknown symbol type: " + type);
		}
	}
	
	private Color getLineColor(int index) {
		return ColorFactory.getGoodColor(index);
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		Insets insets = getInsets();
		resetDrawingStylesAndColors(gc);
		gc.setAntialias(antialias ? SWT.ON : SWT.OFF);

		// draw insets border
		Rectangle canvasRect = new Rectangle(getClientArea());
		gc.setForeground(insetsBackgroundColor);
		gc.setBackground(insetsBackgroundColor);
		gc.fillRectangle(0, 0, canvasRect.width, insets.top); // top
		gc.fillRectangle(0, canvasRect.bottom()-insets.bottom, canvasRect.width, insets.bottom); // bottom
		gc.fillRectangle(0, 0, insets.left, canvasRect.height); // left
		gc.fillRectangle(canvasRect.right()-insets.right, 0, insets.right, canvasRect.height); // right
		gc.setForeground(insetsLineColor);
		gc.drawRectangle(insets.left, insets.top, getViewportWidth(), getViewportHeight());

		title.draw(gc);
		legend.draw(gc);
		xAxis.drawAxis(gc);
		yAxis.drawAxis(gc);
		crosshair.draw(gc);

		// draw status text
		if (getStatusText() != null) {
			resetDrawingStylesAndColors(gc);
			org.eclipse.swt.graphics.Rectangle rect = getViewportRectangle();
			gc.drawText(getStatusText(), rect.x+10, rect.y+10);
		}
	}

	private static final Cursor CROSS_CURSOR = new Cursor(null, SWT.CURSOR_CROSS);
	private static final Font CROSS_HAIR_NORMAL_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	private static final Font CROSS_HAIR_BOLD_FONT = new Font(null, "Arial", 8, SWT.BOLD);

	static class DPoint {
		double x;
		double y;
		
		public DPoint(double x, double y) {
			this.x = x;
			this.y = y;
		}
	}
	
	/**
	 * Displays crosshair mouse cursor.
	 */
	class CrossHair {
		
		Rectangle rect;
		private int x = Integer.MAX_VALUE;
		private int y = Integer.MAX_VALUE;
		DPoint dataPoint;
		
		public CrossHair() {
			addMouseMoveListener(new MouseMoveListener() {
				public void mouseMove(MouseEvent e) {
					x = e.x;
					y = e.y;
					
					// snap to data point
					long startTime = System.currentTimeMillis();
					dataPoint = dataPointNear(x, y, 2);
					System.out.println("crosshair: "+(System.currentTimeMillis()-startTime)+" ms");
					
					if (dataPoint != null) {
						x = toCanvasX(dataPoint.x);
						y = toCanvasY(dataPoint.y);
					}
						
					redraw();  //XXX this is killer if canvas is not cached. unfortunately, gc.setXORMode() cannot be used
					
					if (rect != null && rect.contains(x,y))
						setCursor(CROSS_CURSOR);
					else
						setCursor(null);
				}
			});
		}
		
		public Rectangle layout(GC gc, Rectangle rect) {
			this.rect = rect;
			return rect;
		}
		
		public void draw(GC gc) {
			if (rect != null && rect.contains(x, y)) {
				int[] saveLineDash = gc.getLineDash();
				int saveLineWidth = gc.getLineWidth();
				Color saveForeground = gc.getForeground();
				
				gc.setLineDash(new int[] {3, 3});
				gc.setLineWidth(1);
				gc.setForeground(ColorFactory.asColor("red"));
				gc.drawLine(rect.x, y, rect.x + rect.width, y);
				gc.drawLine(x, rect.y, x, rect.y + rect.height);
				
				Font font = dataPoint != null ? CROSS_HAIR_BOLD_FONT : CROSS_HAIR_NORMAL_FONT; 
				String coordinates =
					String.format("%.3g,%.3g",
						dataPoint != null ? dataPoint.x : fromCanvasX(x),
						dataPoint != null ? dataPoint.y : fromCanvasY(y));
				gc.setFont(font);
				Point size = gc.textExtent(coordinates);
				int left = x + 3;
				int top = y - size.y - 4;
				if (left + size.x + 3 > rect.x + rect.width)
					left = x - size.x - 6;
				if (top < rect.y)
					top = y + 3;
				gc.setForeground(ColorFactory.asColor("black"));
				gc.setBackground(getBackground());
				gc.setLineStyle(SWT.LINE_SOLID);
				gc.drawRectangle(left, top, size.x + 3, size.y + 1);
				gc.drawText(coordinates, left + 2, top + 1, false); // XXX set as tooltip, rather than draw it on the canvas!
				
				gc.setLineDash(saveLineDash);
				gc.setLineWidth(saveLineWidth);
				gc.setForeground(saveForeground);
			}
		}
		
		private DPoint dataPointNear(int x, int y, int d) {
			if (dataset==null)
				return null;
			
			// for each series, perform binary search on the x axis
			for (int series = 0; series < dataset.getSeriesCount(); ++series) {
				int start = 0;
				int end = dataset.getItemCount(series) - 1;
				
				while (start <= end) {
					int mid = (end + start) / 2;
					int midX = toCanvasX(dataset.getXValue(series, mid));
					
					if (Math.abs(midX - x) <= d) {
						for (int i = mid; i >= start; --i) {
							double xx = dataset.getXValue(series, i);
							double yy = dataset.getYValue(series, i);
							int dx = toCanvasX(xx) - x;
							int dy = toCanvasY(yy) - y;
							if (dx * dx + dy * dy <= d * d)
								return new DPoint(xx, yy);
							if (Math.abs(dx) > d)
								break;
						}
						for (int i = mid + 1; i <= end; ++i) {
							double xx = dataset.getXValue(series, i);
							double yy = dataset.getYValue(series, i);
							int dx = toCanvasX(xx) - x;
							int dy = toCanvasY(yy) - y;
							if (dx * dx + dy * dy <= d * d)
								return new DPoint(xx, yy);
							if (Math.abs(dx) > d)
								break;
						}
						break;
					}
					else if (midX - x < 0) {
						start = mid + 1;
					}
					else if (midX - x > 0) {
						end = mid - 1;
					}
				}
			}
			return null;
		}
	}
}
