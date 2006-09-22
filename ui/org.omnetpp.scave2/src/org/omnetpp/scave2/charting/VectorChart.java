package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave2.model.ChartProperties.PROP_DISPLAY_SYMBOLS;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_HIDE_LINE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_POSITION;
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

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave2.charting.plotter.ChartSymbol;
import org.omnetpp.scave2.charting.plotter.CrossSymbol;
import org.omnetpp.scave2.charting.plotter.DiamondSymbol;
import org.omnetpp.scave2.charting.plotter.DotsVectorPlotter;
import org.omnetpp.scave2.charting.plotter.IChartSymbol;
import org.omnetpp.scave2.charting.plotter.IVectorPlotter;
import org.omnetpp.scave2.charting.plotter.LinesVectorPlotter;
import org.omnetpp.scave2.charting.plotter.OvalSymbol;
import org.omnetpp.scave2.charting.plotter.PlusSymbol;
import org.omnetpp.scave2.charting.plotter.SampleHoldVectorPlotter;
import org.omnetpp.scave2.charting.plotter.SquareSymbol;
import org.omnetpp.scave2.charting.plotter.TriangleSymbol;
import org.omnetpp.scave2.charting.plotter.VectorPlotter;
import org.omnetpp.scave2.model.ChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.ChartProperties.LegendPosition;
import org.omnetpp.scave2.model.ChartProperties.LineStyle;
import org.omnetpp.scave2.model.ChartProperties.SymbolType;

//XXX strange effects when resized and vertical scrollbar pulled...
public class VectorChart extends ZoomableCachingCanvas {
	
	private static final Color DEFAULT_TICK_LINE_COLOR = new Color(null, 160, 160, 160);
	private static final Color DEFAULT_TICK_LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color DEFAULT_INSETS_BACKGROUND_COLOR = new Color(null, 236, 233, 216);
	private static final Color DEFAULT_INSETS_LINE_COLOR = new Color(null, 0, 0, 0);
	private static final int DEFAULT_TICK_SPACING = 100; // space between ticks in pixels
	private static final IVectorPlotter DEFAULT_DEFAULT_PLOTTER = new LinesVectorPlotter();
	private static final IChartSymbol DEFAULT_DEFAULT_SYMBOL = new SquareSymbol(3);
	
	private static final String DEFAULT_TITLE = "";
	private static final String DEFAULT_X_TITLE = "";
	private static final String DEFAULT_Y_TITLE = "";
	private static final boolean DEFAULT_DISPLAY_LEGEND = false;
	private static final boolean DEFAULT_LEGEND_BORDER = false;
	private static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Above;
	private static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;
	
	private static final IChartSymbol NULL_SYMBOL = new ChartSymbol() {
		public void drawSymbol(Graphics graphics, int x, int y) {}
	};
	
	private static final IVectorPlotter NULL_PLOTTER = new VectorPlotter() {
		public void plot(XYDataset dataset, int series, Graphics graphics, VectorChart chart, IChartSymbol symbol) {}
	};
	
	private OutputVectorDataset dataset;
	private IVectorPlotter defaultPlotter = DEFAULT_DEFAULT_PLOTTER;
	private IChartSymbol defaultSymbol = DEFAULT_DEFAULT_SYMBOL;
	private int antialias = SWT.ON; // SWT.ON, SWT.OFF, SWT.DEFAULT
	private Color insetsBackgroundColor = DEFAULT_INSETS_BACKGROUND_COLOR;
	private Color insetsLineColor = DEFAULT_INSETS_LINE_COLOR;
	private boolean displaySymbols = true;
	private boolean hideLines = false;
	
	private Title title = new Title(DEFAULT_TITLE, null);
	private Legend legend = new Legend(DEFAULT_DISPLAY_LEGEND, DEFAULT_LEGEND_BORDER, null, DEFAULT_LEGEND_POSITION, DEFAULT_LEGEND_ANCHOR);
	private Axes axes = new Axes(DEFAULT_X_TITLE, DEFAULT_Y_TITLE, null);
	private TickLabels tickLabels = new TickLabels(DEFAULT_TICK_LINE_COLOR, DEFAULT_TICK_LABEL_COLOR, null, DEFAULT_TICK_SPACING);
	
	private Runnable scheduledRedraw;

	public VectorChart(Composite parent, int style) {
		super(parent, style);
		super.setInsets(new Insets(18,40,18,40));
	}
	
	public OutputVectorDataset getDataset() {
		return dataset;
	}

	public void setDataset(OutputVectorDataset dataset) {
		this.dataset = dataset;
		setLegend();
		calculateArea();
		scheduleRedraw();
	}
	
	private void setLegend() {
		legend.clearLegendItems();
		for (int i = 0; i < dataset.getSeriesCount(); ++i) {
			legend.addLegendItem(getLineColor(i), dataset.getSeriesKey(i).toString());
		}
	}
	
	/*==================================
	 *          Properties
	 *==================================*/
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToSwtfont(value));
		else if (PROP_X_AXIS_TITLE.equals(name))
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
		else if (PROP_DISPLAY_SYMBOLS.equals(name))
			setDisplaySymbols(Converter.stringToBoolean(value));
		else if (PROP_SYMBOL_TYPE.equals(name))
			setDefaultSymbol(Converter.stringToEnum(value, SymbolType.class));
		else if (PROP_SYMBOL_SIZE.equals(name))
			setDefaultSymbolSize(Converter.stringToInteger(value));
		else if (PROP_LINE_TYPE.equals(name))
			setDefaultLineType(Converter.stringToEnum(value, LineStyle.class));
		else if (PROP_HIDE_LINE.equals(name))
			setHideLines(Converter.stringToBoolean(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			setDisplayLegend(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_BORDER.equals(name))
			setLegendBorder(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_FONT.equals(name))
			setLegendFont(Converter.stringToSwtfont(value));
		else if (PROP_LEGEND_POSITION.equals(name))
			setLegendPosition(Converter.stringToEnum(value, LegendPosition.class));
		else if (PROP_LEGEND_ANCHORING.equals(name))
			setLegendAnchor(Converter.stringToEnum(value, LegendAnchor.class));
	}
	
	public void setTitle(String value) {
		if (value == null)
			value = DEFAULT_TITLE;
		title.setText(value);
		scheduleRedraw();
	}
	
	public void setTitleFont(Font value) {
		if (value == null)
			return;
		title.setFont(value);
		scheduleRedraw();
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

	public IVectorPlotter getDefaultLineType() {
		return defaultPlotter;
	}

	public void setDefaultLineType(IVectorPlotter defaultPlotter) {
		this.defaultPlotter = defaultPlotter;
		scheduleRedraw();
	}
	
	public void setDefaultLineType(LineStyle type) {
		if (type == null)
			return;
		
		switch (type) {
		case None: setDefaultLineType(new DotsVectorPlotter()); break;
		case Linear: setDefaultLineType(new LinesVectorPlotter()); break;
		case Step: setDefaultLineType(new SampleHoldVectorPlotter()); break;
		case Spline: /*TODO*/ break; 
		}
		scheduleRedraw();
	}

	public IChartSymbol getDefaultSymbol() {
		return defaultSymbol;
	}

	public void setDefaultSymbol(IChartSymbol defaultSymbol) {
		Assert.isLegal(defaultSymbol != null);
		this.defaultSymbol = defaultSymbol;
		scheduleRedraw();
	}
	
	public void setDefaultSymbol(SymbolType symbolType) {
		if (symbolType == null)
			return;
		
		int size = defaultSymbol.getSizeHint();
		switch (symbolType) {
		case Cross: setDefaultSymbol(new CrossSymbol(size)); break;
		case Diamond: setDefaultSymbol(new DiamondSymbol(size)); break;
		case Oval: setDefaultSymbol(new OvalSymbol(size)); break;
		case Plus: setDefaultSymbol(new PlusSymbol(size)); break;
		case Square: setDefaultSymbol(new SquareSymbol(size)); break;
		case Triangle: setDefaultSymbol(new TriangleSymbol(size)); break;
		}
		scheduleRedraw();
	}
	
	public void setDisplaySymbols(Boolean value) {
		displaySymbols = value != null ? value : true;
		scheduleRedraw();
	}
	
	public void setDefaultSymbolSize(Integer size) {
		if (size == null)
			return;
		defaultSymbol.setSizeHint(size);
		scheduleRedraw();
	}
	
	public void setHideLines(Boolean value) {
		hideLines = value != null ? value : false;
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
	
	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;
		legend.setVisible(value);
		scheduleRedraw();
	}
	
	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;
		legend.setDrawBorder(value);
		scheduleRedraw();
	}
	
	public void setLegendFont(Font value) {
		if (value == null)
			return;
		legend.setFont(value);
		scheduleRedraw();
	}
	
	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;
		legend.setPosition(value);
		scheduleRedraw();
	}
	
	public void setLegendAnchor(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;
		legend.setAnchor(value);
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
		Rectangle remaining = title.layout(gc, getClientArea());
		remaining = legend.layout(gc, remaining);
		remaining = tickLabels.layout(gc, remaining);
		remaining = axes.layout(gc, remaining);

		Insets insets = calculateInsets(getClientArea(), remaining);
		setInsets(insets);
		super.beforePaint(gc);
	}
	
	@Override
	protected void paintCachableLayer(Graphics graphics) {
		graphics.setAntialias(antialias);
		IVectorPlotter plotter = hideLines ? NULL_PLOTTER : defaultPlotter;
		IChartSymbol symbol = displaySymbols ? defaultSymbol : NULL_SYMBOL;
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			Color color = getLineColor(series);
			graphics.setForegroundColor(color);
			graphics.setBackgroundColor(color);
			plotter.plot(dataset, series, graphics, this, symbol);
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
		gc.setForeground(insetsBackgroundColor);
		gc.setBackground(insetsBackgroundColor);
		gc.fillRectangle(0, 0, getWidth(), insets.top); // top
		gc.fillRectangle(0, getHeight()-insets.bottom, getWidth(), insets.bottom); // bottom
		gc.fillRectangle(0, 0, insets.left, getHeight()); // left
		gc.fillRectangle(getWidth()-insets.right, 0, insets.right, getHeight()); // right
		gc.setForeground(insetsLineColor);
		gc.drawRectangle(insets.left-1, insets.top-1, getWidth()-insets.getWidth()+1, getHeight()-insets.getHeight()+1);

		title.draw(gc);
		legend.draw(gc);
		tickLabels.draw(gc);
		axes.draw(gc);
	}

	private void scheduleRedraw() {
		if (scheduledRedraw == null) {
			scheduledRedraw = new Runnable() {
				public void run() {
					scheduledRedraw = null;
					clearCanvasCacheAndRedraw();
				}
			};
			getDisplay().asyncExec(scheduledRedraw);
		}
	}
	
	private static Insets calculateInsets(Rectangle outer, Rectangle inner) {
		return new Insets(Math.max(inner.y - outer.y, 0),
						  Math.max(inner.x - outer.x, 0),
						  Math.max(outer.y + outer.height - inner.y - inner.height, 0),
						  Math.max(outer.x + outer.width - inner.x - inner.width, 0));
	}
	
	private static Rectangle extractInsets(Rectangle rect, Insets insets) {
		return new Rectangle(
				rect.x + insets.left,
				rect.y + insets.top,
				rect.width - insets.getWidth(),
				rect.height - insets.getHeight());
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
			plotBounds = extractInsets(constraint, insets);

			gc.setFont(saveFont);
			
			return plotBounds;
		}
		
		public void draw(GC gc) {
			Font saveFont = gc.getFont();
			if (font != null)
				gc.setFont(font);
			
			//draw X ticks
			BigDecimal tickStartX = getTickStart(fromCanvasX(plotBounds.x), getZoomX());
			BigDecimal tickEndX = getTickEnd(fromCanvasX(plotBounds.x + plotBounds.width), getZoomX());
			BigDecimal tickDeltaX = getTickDelta(getZoomX());

			for (BigDecimal t=tickStartX; t.compareTo(tickEndX)<0; t = t.add(tickDeltaX)) {
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
			BigDecimal tickStartY = getTickStart(fromCanvasY(plotBounds.y + plotBounds.height), getZoomY());
			BigDecimal tickEndY = getTickEnd(fromCanvasY(plotBounds.y), getZoomY());
			BigDecimal tickDeltaY = getTickDelta(getZoomY());

			for (BigDecimal t=tickStartY; t.compareTo(tickEndY)<0; t = t.add(tickDeltaY)) {
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
		
		private BigDecimal getTickStart(double start, double zoom) {
			int tickScale = (int)Math.ceil(Math.log10(spacing / zoom));
			BigDecimal tickStart = new BigDecimal(start).setScale(-tickScale, RoundingMode.FLOOR);
			return tickStart;
		}
		
		private BigDecimal getTickEnd(double end, double zoom) {
			int tickScale = (int)Math.ceil(Math.log10(spacing / zoom));
			BigDecimal tickEnd = new BigDecimal(end).setScale(-tickScale, RoundingMode.CEILING);
			return tickEnd;
		}
		
		private BigDecimal getTickDelta(double zoom) {
			int tickScale = (int)Math.ceil(Math.log10(spacing / zoom));
			BigDecimal tickSpacing = BigDecimal.valueOf(spacing / zoom);
			BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);

			// use 2, 4, 6, 8, etc. if possible
			if (tickIntvl.divide(BigDecimal.valueOf(5)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(5));
			// use 5, 10, 15, 20, etc. if possible
			else if (tickIntvl.divide(BigDecimal.valueOf(2)).compareTo(tickSpacing) > 0)
				tickIntvl = tickIntvl.divide(BigDecimal.valueOf(2));
			return tickIntvl;
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
