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
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
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
	
	private XYDataset dataset;
	private IVectorPlotter defaultPlotter = DEFAULT_DEFAULT_PLOTTER;
	private IChartSymbol defaultSymbol = DEFAULT_DEFAULT_SYMBOL;
	private int antialias = SWT.ON; // SWT.ON, SWT.OFF, SWT.DEFAULT
	private Color tickLineColor = DEFAULT_TICK_LINE_COLOR;
	private Color tickLabelColor = DEFAULT_TICK_LABEL_COLOR;
	private Color insetsBackgroundColor = DEFAULT_INSETS_BACKGROUND_COLOR;
	private Color insetsLineColor = DEFAULT_INSETS_LINE_COLOR;
	private int tickSpacing = DEFAULT_TICK_SPACING;
	private boolean displaySymbols = true;
	private boolean hideLines = false;
	private Font labelFont;
	
	private String title = DEFAULT_TITLE;
	private Font titleFont;
	
	private boolean displayLegend = DEFAULT_DISPLAY_LEGEND;
	private boolean legendBorder = DEFAULT_LEGEND_BORDER;
	private Font legendFont = null;
	private LegendPosition legendPosition = DEFAULT_LEGEND_POSITION;
	private LegendAnchor legendAnchor = DEFAULT_LEGEND_ANCHOR;
	
	private Runnable scheduledRedraw;
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		super.setInsets(new Insets(18,40,18,40));
	}
	
	public XYDataset getDataset() {
		return dataset;
	}

	public void setDataset(XYDataset dataset) {
		this.dataset = dataset;
		calculateArea();
		scheduleRedraw();
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
			;
		else if (PROP_Y_AXIS_TITLE.equals(name))
			;
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			;
		else if (PROP_LABEL_FONT.equals(name))
			setLabelFont(Converter.stringToSwtfont(value));
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
			;
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
		this.title = value;
		scheduleRedraw();
	}
	
	public void setTitleFont(Font value) {
		if (value == null)
			return;
		this.titleFont = value;
		scheduleRedraw();
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
	
	public void setLabelFont(Font font) {
		if (font != null)
			labelFont = font;
		scheduleRedraw();
	}
	
	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;
		displayLegend = value;
		scheduleRedraw();
	}
	
	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;
		legendBorder = value;
		scheduleRedraw();
	}
	
	public void setLegendFont(Font value) {
		if (value == null)
			return;
		legendFont = value;
		scheduleRedraw();
	}
	
	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;
		legendPosition = value;
		scheduleRedraw();
	}
	
	public void setLegendAnchor(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;
		legendAnchor = value;
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
		/* in progress ...
		
		// Calculate space occupied by title and legend and set insets accordingly
		Rectangle rect = gc.getClipping();
		calculateLegendSize(gc, rect);
		
		Rectangle clip = gc.getClipping();
		Insets insets = new Insets();
		insets.top = Math.max(rect.y - clip.y, 0) + 18;
		insets.left = Math.max(rect.x - clip.x, 0) + 40;
		insets.bottom = Math.max((clip.y + clip.height) - (rect.y + rect.height), 0) + 18;
		insets.right = Math.max((clip.x + clip.width) - (rect.x + rect.width), 0) + 40;
		setInsets(insets);
		*/
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

		Rectangle rect = gc.getClipping();
		
		/* work in progress...
			drawTitle(gc, rect);
			if (displayLegend) drawLegend(gc, rect);
		*/
		drawAxesAndTicks(gc, rect);
	}

	/**
	 * @param graphics
	 * @param area
	 */
	protected void drawAxesAndTicks(GC gc, Rectangle area) {
		//draw X ticks
		double startX = fromCanvasX(area.x);
		double endX = fromCanvasX(area.x + area.width);
		int tickScale = (int)Math.ceil(Math.log10(tickSpacing / getZoomX()));
		BigDecimal tickSpacingX = BigDecimal.valueOf(tickSpacing / getZoomX());
		BigDecimal tickStartX = new BigDecimal(startX).setScale(-tickScale, RoundingMode.FLOOR);
		BigDecimal tickEndX = new BigDecimal(endX).setScale(-tickScale, RoundingMode.CEILING);
		BigDecimal tickIntvlX = new BigDecimal(1).scaleByPowerOfTen(tickScale);

		// use 2, 4, 6, 8, etc. if possible
		if (tickIntvlX.divide(BigDecimal.valueOf(5)).compareTo(tickSpacingX) > 0)
			tickIntvlX = tickIntvlX.divide(BigDecimal.valueOf(5));
		// use 5, 10, 15, 20, etc. if possible
		else if (tickIntvlX.divide(BigDecimal.valueOf(2)).compareTo(tickSpacingX) > 0)
			tickIntvlX = tickIntvlX.divide(BigDecimal.valueOf(2));

		if (labelFont != null) gc.setFont(labelFont);
		for (BigDecimal t=tickStartX; t.compareTo(tickEndX)<0; t = t.add(tickIntvlX)) {
			int x = toCanvasX(t.doubleValue());
			gc.setLineStyle(SWT.LINE_DOT);
			gc.setForeground(tickLineColor);
			gc.drawLine(x, area.y, x, area.y + area.height);
			gc.setForeground(tickLabelColor);
			String str = t.toPlainString() + "s";
			gc.setBackground(insetsBackgroundColor);
			gc.drawText(str, x + 3, area.y + 2);
			gc.drawText(str, x + 3, area.y + area.height - 16);
		}

		// Y axis
		// XXX don't draw if falls into gutter area
		if (startX<=0 && endX>=0) {
			gc.setLineStyle(SWT.LINE_SOLID);
			gc.drawLine(toCanvasX(0), area.y, toCanvasX(0), area.y + area.height);
		}
		
		//draw Y ticks
		//XXX factor out common code with X axis ticks
		double startY = fromCanvasY(area.y);
		double endY = fromCanvasY(area.y + area.height);
		int tickScaleY = (int)Math.ceil(Math.log10(tickSpacing / getZoomY()));
		BigDecimal tickSpacingY = BigDecimal.valueOf(tickSpacing / getZoomY());
		BigDecimal tickStartY = new BigDecimal(startY).setScale(-tickScaleY, RoundingMode.FLOOR);
		BigDecimal tickEndY = new BigDecimal(endY).setScale(-tickScaleY, RoundingMode.CEILING);
		BigDecimal tickIntvlY = new BigDecimal(1).scaleByPowerOfTen(tickScaleY);

		// use 2, 4, 6, 8, etc. if possible
		if (tickIntvlY.divide(BigDecimal.valueOf(5)).compareTo(tickSpacingY) > 0)
			tickIntvlY = tickIntvlY.divide(BigDecimal.valueOf(5));
		// use 5, 10, 15, 20, etc. if possible
		else if (tickIntvlY.divide(BigDecimal.valueOf(2)).compareTo(tickSpacingY) > 0)
			tickIntvlY = tickIntvlY.divide(BigDecimal.valueOf(2));

		if(labelFont != null) gc.setFont(labelFont);
		for (BigDecimal t=tickStartY; t.compareTo(tickEndY)<0; t = t.add(tickIntvlY)) {
			int y = toCanvasY(t.doubleValue());
			gc.setLineStyle(SWT.LINE_DOT);
			gc.setForeground(tickLineColor);
			gc.drawLine(area.x, y, area.x + area.width, y);
			gc.setForeground(tickLabelColor);
			String str = t.toPlainString() + "s";
			gc.setBackground(insetsBackgroundColor);
			gc.drawText(str, area.x + 2, y+3);
			gc.drawText(str, area.x + area.width - 16, y+3);
		}

		// X axis
		// XXX don't draw if falls into gutter area
		//if (startY<=0 && endY>=0) { //XXX with this "if" sometimes it doesn't draw
			gc.setLineStyle(SWT.LINE_SOLID);
			gc.drawLine(area.x, toCanvasY(0), area.x + area.width, toCanvasY(0));
		//}
	}
	
	/**
	 * Draws the title into the specified area.
	 * @param graphics TODO
	 * @param area
	 */
	protected void drawTitle(GC gc, Rectangle area) {
		if (titleFont != null)
			gc.setFont(titleFont);
		Point size = gc.stringExtent(title);
		Point location = new Point(area.x + (area.width - size.x) / 2, 0);
		gc.drawString(title, location.x, location.y, true);
		area.y = Math.min(area.y + size.y, area.y + area.height);
		area.height = Math.max(area.height - size.y, 0);
	}
	
	/**
	 * Draws a legend into the specified area.
	 * @param gc
	 * @param area
	 */
	protected void drawLegend(GC gc, Rectangle area) {
		int top = area.y, left = area.x, bottom = area.y + area.height, right = area.x + area.width;

		if (legendFont != null)
			gc.setFont(legendFont);
		
		// calculate legend size
		Point size = calculateLegendSize(gc, area);
		
		// calculate legend position
		int dx, dy;
		switch (legendAnchor) {
		case North:		dx = 0; dy = -1; break;
		case NorthEast:	dx = 1; dy = -1; break; 
		case East:		dx = 1; dy = 0; break;
		case SouthEast:	dx = 1; dy = 1; break;
		case South:		dx = 0; dy = 1; break;
		case SouthWest:	dx = -1; dy = 1; break;
		case West:		dx = -1; dy = 0; break;
		case NorthWest:	dx = -1; dy = -1; break;
		default: throw new IllegalStateException();
		}
		
		int x,y;
		switch (legendPosition) {
		case Above:
			x = left + (area.width - size.x) * (dx + 1) / 2;
			y = top;
			top = Math.min(top + size.y, bottom);
			break;
		case Below:
			x = left + (area.width - size.x) * (dx + 1) / 2;
			y = bottom - size.y;
			bottom = Math.max(bottom - size.y, top);
			break;
		case Left:
			x = left;
			y = top + (area.height - size.y) * (dy + 1) / 2;
			left = Math.min(left + size.x, right);
			break;
		case Right:
			x = right - size.x;
			y = top + (area.height - size.y) * (dy + 1) / 2;
			right = Math.max(right - size.x, left);
			break;
		case Inside:
			x = left + (area.width - size.x) * (dx + 1) / 2;
			y = top + (area.height - size.y) * (dy + 1) / 2;
			break;
		default:
			throw new IllegalStateException();
		}
		
		// update remaining space
		area.x = left;
		area.y = top;
		area.width = right - left;
		area.height = bottom - top;
		
		// draw background and border
		Rectangle borderRect = new Rectangle(x, y, size.x, size.y);
		gc.setBackground(ColorFactory.asColor("white"));
		gc.fillRectangle(new Rectangle(x, y, size.x, size.y));
		if (legendBorder) {
			gc.setForeground(ColorFactory.asColor("black"));
			gc.drawRectangle(borderRect);
		}
		
		// draw legend items
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			Color color = getLineColor(series);
			gc.setForeground(color);
			gc.setBackground(color);
			gc.fillOval(x+2 , y + (size.y - 5) / 2, 5, 5);
			x += 9;
			
			String label = dataset.getSeriesKey(series).toString();
			Point labelSize = gc.stringExtent(label);
			gc.setForeground(ColorFactory.asColor("black"));
			gc.drawString(label, x, y + (size.y - labelSize.y) / 2, true);
			x += labelSize.x + 5;
		}
	}
	
	private Point calculateLegendSize(GC gc, Rectangle area) {
		Point size = new Point(0,5);
		for (int i = 0; i < dataset.getSeriesCount(); ++i) {
			String label = dataset.getSeriesKey(i).toString();
			Point labelSize = gc.stringExtent(label);

			if (i > 0) size.x += 5;
			size.x += 9 + labelSize.x;
			size.y = Math.max(size.y, labelSize.y);
		}
		return size;
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
}
