package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_TITLE;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GeomUtils;
import org.omnetpp.scave2.Activator;
import org.omnetpp.scave2.charting.plotter.ChartSymbol;
import org.omnetpp.scave2.charting.plotter.CrossSymbol;
import org.omnetpp.scave2.charting.plotter.DiamondSymbol;
import org.omnetpp.scave2.charting.plotter.DotsVectorPlotter;
import org.omnetpp.scave2.charting.plotter.IChartSymbol;
import org.omnetpp.scave2.charting.plotter.IVectorPlotter;
import org.omnetpp.scave2.charting.plotter.LinesVectorPlotter;
import org.omnetpp.scave2.charting.plotter.OvalSymbol;
import org.omnetpp.scave2.charting.plotter.PinsVectorPlotter;
import org.omnetpp.scave2.charting.plotter.PlusSymbol;
import org.omnetpp.scave2.charting.plotter.SampleHoldVectorPlotter;
import org.omnetpp.scave2.charting.plotter.SquareSymbol;
import org.omnetpp.scave2.charting.plotter.TriangleSymbol;
import org.omnetpp.scave2.charting.plotter.VectorPlotter;
import org.omnetpp.scave2.model.ChartProperties.LineStyle;
import org.omnetpp.scave2.model.ChartProperties.SymbolType;

//XXX strange effects when resized and vertical scrollbar pulled...
public class VectorChart extends ChartCanvas {

	private static final Color DEFAULT_TICK_LINE_COLOR = new Color(null, 160, 160, 160);
	private static final Color DEFAULT_TICK_LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color DEFAULT_INSETS_BACKGROUND_COLOR = new Color(null, 236, 233, 216);
	private static final Color DEFAULT_INSETS_LINE_COLOR = new Color(null, 0, 0, 0);
	private static final int DEFAULT_TICK_SPACING = 100; // space between ticks in pixels
	
	private static final String DEFAULT_X_TITLE = "";
	private static final String DEFAULT_Y_TITLE = "";
	
	private OutputVectorDataset dataset;
	private int antialias = SWT.ON; // SWT.ON, SWT.OFF, SWT.DEFAULT
	private Color insetsBackgroundColor = DEFAULT_INSETS_BACKGROUND_COLOR;
	private Color insetsLineColor = DEFAULT_INSETS_LINE_COLOR;
	
	static class LineProperties {
		
		static final SymbolType DEFAULT_SYMBOL_TYPE = SymbolType.Square;
		static final Integer DEFAULT_SYMBOL_SIZE = Integer.valueOf(3);
		static final LineStyle DEFAULT_LINE_STYLE = LineStyle.Linear;
		
		private static final IChartSymbol NULL_SYMBOL = new ChartSymbol() {
			public void drawSymbol(Graphics graphics, int x, int y) {}
		};
		private static final IVectorPlotter NULL_PLOTTER = new VectorPlotter() {
			public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart, IChartSymbol symbol) {}
		};
		
		SymbolType symbolType;
		Integer symbolSize;
		LineStyle lineStyle;
	}
	
	private static final String KEY_ALL = null;
	
	private Map<String, LineProperties> lineProperties = new HashMap<String,LineProperties>();
	private Axes axes = new Axes(DEFAULT_X_TITLE, DEFAULT_Y_TITLE, null);
	private TickLabels tickLabels = new TickLabels(DEFAULT_TICK_LINE_COLOR, DEFAULT_TICK_LABEL_COLOR, null, DEFAULT_TICK_SPACING);
	private CrossHair crosshair = new CrossHair();
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		super.setInsets(new Insets(18,40,18,40));
		lineProperties.put(KEY_ALL, new LineProperties());
	}
	
	public OutputVectorDataset getDataset() {
		return dataset;
	}

	public void setDataset(OutputVectorDataset dataset) {
		this.dataset = dataset;
		updateLegend();
		calculateArea();
		scheduleRedraw();
	}
	
	private void updateLegend() {
		legend.clearLegendItems();
		for (int i = 0; i < dataset.getSeriesCount(); ++i) {
			legend.addLegendItem(getLineColor(i), dataset.getSeriesKey(i).toString());
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
			;
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
			;
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			;
		else if (PROP_XY_INVERT.equals(name))
			;
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
		axes.setXTitle(value != null ? value : DEFAULT_X_TITLE);
		scheduleRedraw();
	}
	
	public void setYAxisTitle(String value) {
		axes.setYTitle(value != null ? value : DEFAULT_Y_TITLE);
		scheduleRedraw();
	}
	
	public void setAxisTitleFont(Font value) {
		if (value != null) {
			axes.setFont(value);
			scheduleRedraw();
		}
	}

	public LineStyle getLineStyle (String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.lineStyle == null)
			props = getDefaultLineProperties();
		return props.lineStyle != null ? props.lineStyle : LineProperties.DEFAULT_LINE_STYLE;
	}

	public void setLineStyle(String key, LineStyle type) {
		LineProperties props = getOrCreateLineProperties(key);
		props.lineStyle = type;
		scheduleRedraw();
	}

	public SymbolType getSymbolType(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolType == null)
			props = getDefaultLineProperties();
		return props.symbolType != null ? props.symbolType : LineProperties.DEFAULT_SYMBOL_TYPE;
	}
	
	public void setSymbolType(String key, SymbolType symbolType) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolType = symbolType;
		scheduleRedraw();
	}
	
	public int getSymbolSize(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolSize == null)
			props = getDefaultLineProperties();
		return props.symbolSize != null ? props.symbolSize : LineProperties.DEFAULT_SYMBOL_SIZE;
	}
	
	public void setSymbolSize(String key, Integer size) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolSize = size;
		scheduleRedraw();
	}
	
	public void setXMin(Double value) {
		if (value != null)
			setArea(value, minY, maxX, maxY);
		scheduleRedraw();
	}
	
	public void setXMax(Double value) {
		if (value != null)
			setArea(minX, minY, value, maxY);
		scheduleRedraw();
	}
	
	public void setYMin(Double value) {
		if (value != null)
			setArea(minX, value, maxX, maxY);
		scheduleRedraw();
	}
	
	public void setYMax(Double value) {
		if (value != null)
			setArea(minX, minY, maxX, value);
		scheduleRedraw();
	}
	
	public void setGridVisibility(Boolean value) {
		tickLabels.setGridVisible(value != null ? value : false);
		scheduleRedraw();
	}
	
	public void setTickLabelFont(Font font) {
		if (font != null)
			tickLabels.setFont(font);
		scheduleRedraw();
	}
	
	public int getAntialias() {
		return antialias;
	}

	public void setAntialias(int antialias) {
		this.antialias = antialias;
		scheduleRedraw();
	}

	private void calculateArea() {
		if (dataset==null || dataset.getSeriesCount()==0) {
			setArea(0,0,0,0);
			return;
		}
		
		// calculate bounding box
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
        double width = maxX - minX;
        double height = maxY - minY;

        setArea(minX-width/80, minY-height/5, maxX+width/80, maxY+height/3); // leave some extra space
        zoomToFit(); // zoom out completely
	}

	@Override
	protected void beforePaint(GC gc) {
		// Calculate space occupied by title and legend and set insets accordingly
		Rectangle area = new Rectangle(getClientArea());
		Rectangle remaining = title.layout(gc, area);
		remaining = legend.layout(gc, remaining);
		remaining = tickLabels.layout(gc, remaining);
		remaining = axes.layout(gc, remaining);
		remaining = crosshair.layout(gc, remaining);

		Insets insets = GeomUtils.subtract(area, remaining);
		setInsets(insets);
		super.beforePaint(gc);
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		Graphics graphics = new SWTGraphics(gc);
		graphics.setAntialias(antialias);
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			String key = dataset.getSeriesKey(series).toString();
			IVectorPlotter plotter = getPlotter(key);
			IChartSymbol symbol = getSymbol(key);
			Color color = getLineColor(series);
			graphics.setForegroundColor(color);
			graphics.setBackgroundColor(color);
			plotter.plot(dataset, series, graphics, this, symbol);
		}
		graphics.dispose();
	}
	
	private IVectorPlotter getPlotter(String key) {
		LineStyle style = getLineStyle(key);
		switch (style) {
		case None: return new DotsVectorPlotter();
		case Linear: return new LinesVectorPlotter();
		case Step: return new SampleHoldVectorPlotter();
		case Pins: return new PinsVectorPlotter();
		default:
			Assert.isTrue(false, "Unknown LineStyle: " + style);
			return LineProperties.NULL_PLOTTER;
		}
	}
	
	private IChartSymbol getSymbol(String key) {
		SymbolType type = getSymbolType(key);
		int size = getSymbolSize(key);
		switch (type) {
		case None: return LineProperties.NULL_SYMBOL;
		case Cross: return new CrossSymbol(size);
		case Diamond: return new DiamondSymbol(size);
		case Oval: return new OvalSymbol(size);
		case Plus: return new PlusSymbol(size);
		case Square: return new SquareSymbol(size);
		case Triangle: return new TriangleSymbol(size);
		default:
			Assert.isTrue(false, "Unknown SymbolType: " + type);
			return LineProperties.NULL_SYMBOL;
		}
	}
	
	private Color getLineColor(int index) {
		return ColorFactory.getGoodColor(index);
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		Insets insets = getInsets();
		gc.setAntialias(antialias);

		// draw insets border
		Rectangle canvasRect = new Rectangle(getClientArea());
		gc.setForeground(insetsBackgroundColor);
		gc.setBackground(insetsBackgroundColor);
		gc.fillRectangle(0, 0, canvasRect.width, insets.top); // top
		gc.fillRectangle(0, canvasRect.bottom()-insets.bottom, canvasRect.width, insets.bottom); // bottom
		gc.fillRectangle(0, 0, insets.left, canvasRect.height); // left
		gc.fillRectangle(canvasRect.right()-insets.right, 0, insets.right, canvasRect.height); // right
		gc.setForeground(insetsLineColor);
		gc.drawRectangle(insets.left-1, insets.top-1, getViewportWidth()+1, getViewportHeight()+1);

		title.draw(gc);
		legend.draw(gc);
		tickLabels.draw(gc);
		axes.draw(gc);
		crosshair.draw(gc);
	}

	class TickLabels {
		private Color lineColor;
		private Color labelColor;
		private Font font;
		private int spacing;
		private boolean showGrid;
		
		private Rectangle bounds; // bounds of the plot (including ticks)
		private Rectangle plotBounds; // bounds of the plot (without tick area)

		public TickLabels(Color lineColor, Color labelColor, Font font, int spacing) {
			this.lineColor = lineColor;
			this.labelColor = labelColor;
			this.font = font;
			this.spacing = spacing;
		}
		
		public Font getFont() {
			return font;
		}
		
		public void setFont(Font font) {
			this.font = font;
		}
		
		public boolean isGridVisible() {
			return showGrid;
		}
		
		public void setGridVisible(boolean visible) {
			showGrid = visible;
		}
		
		public Rectangle layout(GC gc, Rectangle constraint) {
			Font saveFont = gc.getFont();
			if (font != null)
				gc.setFont(font);
			
			bounds = constraint;
			int height = gc.getFontMetrics().getHeight();
			Insets insets = new Insets(height + 4, 40, height + 4, 40);
			plotBounds = constraint.getCropped(insets);

			gc.setFont(saveFont);
			
			return plotBounds;
		}
		
		public void draw(GC gc) {
			Font saveFont = gc.getFont();
			if (font != null)
				gc.setFont(font);
			
			//draw X ticks
			Ticks ticks = new Ticks(fromCanvasX(plotBounds.x),
									fromCanvasX(plotBounds.x + plotBounds.width),
									spacing / getZoomX());
			for (BigDecimal t : ticks) {
				int x = toCanvasX(t.doubleValue());
				if (x < plotBounds.x || x > plotBounds.x + plotBounds.width)
					continue;
				if (showGrid) {
					gc.setLineStyle(SWT.LINE_DOT);
					gc.setForeground(lineColor);
					gc.drawLine(x, plotBounds.y, x, plotBounds.y + plotBounds.height);
				}
				gc.setForeground(labelColor);
				gc.setBackground(insetsBackgroundColor);
				String str = t.toPlainString() + "s";
				gc.drawText(str, x + 3, bounds.y + 2);
				gc.drawText(str, x + 3, bounds.y + bounds.height - 16);
			}

			//draw Y ticks
			//XXX factor out common code with X axis ticks
			ticks = new Ticks(fromCanvasY(plotBounds.y + plotBounds.height),
							  fromCanvasY(plotBounds.y),
							  spacing / getZoomY());
			for (BigDecimal t : ticks) {
				int y = toCanvasY(t.doubleValue());
				if (y < plotBounds.y || y > plotBounds.y + plotBounds.height)
					continue;
				if (showGrid) {
					gc.setLineStyle(SWT.LINE_DOT);
					gc.setForeground(lineColor);
					gc.drawLine(plotBounds.x, y, plotBounds.x + plotBounds.width, y);
				}

				gc.setForeground(labelColor);
				gc.setBackground(insetsBackgroundColor);
				String str = t.toPlainString();
				gc.drawText(str, bounds.x + 2, y+3);
				gc.drawText(str, bounds.x + bounds.width - 16, y+3);
			}
			
			gc.setFont(saveFont);
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
	
	class CrossHair {
		
		Rectangle rect;
		private int x = Integer.MAX_VALUE;
		private int y = Integer.MAX_VALUE;
		
		public CrossHair() {
			addMouseMoveListener(new MouseMoveListener() {
				public void mouseMove(MouseEvent e) {
					x = e.x;
					y = e.y;
					redraw();
					
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
				
				DPoint dataPoint = dataPointNearTo(x, y, 5);
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
				gc.drawText(coordinates, left + 2, top + 1);
				
				gc.setLineDash(saveLineDash);
				gc.setLineWidth(saveLineWidth);
				gc.setForeground(saveForeground);
			}
		}
		
		private DPoint dataPointNearTo(int x, int y, int d) {
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
	
	class Axes {
		
		private String xAxisTitle;
		private String yAxisTitle;
		private Font axisTitleFont;
		
		private Rectangle bounds;
		
		public Axes(String xTitle, String yTitle, Font font) {
			this.xAxisTitle = xTitle;
			this.yAxisTitle = yTitle;
			this.axisTitleFont = font;
		}
		
		public String getXTitle() {
			return xAxisTitle;
		}
		
		public void setXTitle(String title) {
			xAxisTitle = title;
		}
		
		public String getYTitle() {
			return yAxisTitle;
		}
		
		public void setYTitle(String title) {
			yAxisTitle = title;
		}
		
		public Font getFont() {
			return axisTitleFont;
		}
		
		public void setFont(Font font) {
			this.axisTitleFont = font;
		}
		
		public Rectangle layout(GC gc, Rectangle area) {
			bounds = area;
			return area;
		}
		
		public void draw(GC gc) {
			int left = bounds.x, top = bounds.y, right = bounds.x + bounds.width, bottom = bounds.y + bounds.height;

			// X axis
			double startY = fromCanvasY(bottom);
			double endY = fromCanvasY(top);
			if (startY<=0 && endY>=0) {
				gc.setLineStyle(SWT.LINE_SOLID);
				gc.drawLine(left, toCanvasY(0), right, toCanvasY(0));
			}

			// Y axis
			double startX = fromCanvasX(left);
			double endX = fromCanvasX(right);
			if (startX<=0 && endX>=0) {
				gc.setLineStyle(SWT.LINE_SOLID);
				gc.drawLine(toCanvasX(0), top, toCanvasX(0), bottom);
			}
		}
	}
}
