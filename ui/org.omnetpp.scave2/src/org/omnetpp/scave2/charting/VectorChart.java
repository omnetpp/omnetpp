package org.omnetpp.scave2.charting;

import java.math.BigDecimal;
import java.math.RoundingMode;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.Triangle;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
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

import static org.omnetpp.scave2.model.ChartProperties.*;

//XXX strange effects when resized and vertical scrollbar pulled...
public class VectorChart extends ZoomableCachingCanvas {
	private static final Color DEFAULT_TICK_LINE_COLOR = new Color(null, 160, 160, 160);
	private static final Color DEFAULT_TICK_LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color DEFAULT_INSETS_BACKGROUND_COLOR = new Color(null, 236, 233, 216);
	private static final Color DEFAULT_INSETS_LINE_COLOR = new Color(null, 0, 0, 0);
	private static final int DEFAULT_TICK_SPACING = 100; // space between ticks in pixels
	private static final IVectorPlotter DEFAULT_DEFAULT_PLOTTER = new LinesVectorPlotter();
	private static final IChartSymbol DEFAULT_DEFAULT_SYMBOL = new SquareSymbol(3);
	
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
		clearCanvasCacheAndRedraw();
	}
	
	/*==================================
	 *          Properties
	 *==================================*/
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			;
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			;
		else if (PROP_X_AXIS_TITLE.equals(name))
			;
		else if (PROP_Y_AXIS_TITLE.equals(name))
			;
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			;
		else if (PROP_LABEL_FONT.equals(name))
			;
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			;
		// Axes
		else if (PROP_X_AXIS_MIN.equals(name))
			;
		else if (PROP_X_AXIS_MAX.equals(name))
			;
		else if (PROP_Y_AXIS_MIN.equals(name))
			;
		else if (PROP_Y_AXIS_MAX.equals(name))
			;
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
			setDefaultSymbol(SymbolType.valueOf(value));
		else if (PROP_SYMBOL_SIZE.equals(name))
			setDefaultSymbolSize(Converter.stringToInteger(value));
		else if (PROP_LINE_TYPE.equals(name))
			setDefaultLineType(LineStyle.valueOf(value));
		else if (PROP_HIDE_LINE.equals(name))
			setHideLines(Converter.stringToBoolean(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			;
		else if (PROP_LEGEND_BORDER.equals(name))
			;
		else if (PROP_LEGEND_FONT.equals(name))
			;
		else if (PROP_LEGEND_POSITION.equals(name))
			;
		else if (PROP_LEGEND_ANCHORING.equals(name))
			;
	}

	public IVectorPlotter getDefaultLineType() {
		return defaultPlotter;
	}

	public void setDefaultLineType(IVectorPlotter defaultPlotter) {
		this.defaultPlotter = defaultPlotter;
		clearCanvasCacheAndRedraw();
	}
	
	public void setDefaultLineType(LineStyle type) {
		switch (type) {
		case None: setDefaultLineType(new DotsVectorPlotter()); break;
		case Linear: setDefaultLineType(new LinesVectorPlotter()); break;
		case Step: setDefaultLineType(new SampleHoldVectorPlotter()); break;
		case Spline: /*TODO*/ break; 
		}
	}

	public IChartSymbol getDefaultSymbol() {
		return defaultSymbol;
	}

	public void setDefaultSymbol(IChartSymbol defaultSymbol) {
		Assert.isLegal(defaultSymbol != null);
		this.defaultSymbol = defaultSymbol;
		clearCanvasCacheAndRedraw();
	}
	
	public void setDefaultSymbol(SymbolType symbolType) {
		int size = defaultSymbol.getSizeHint();
		switch (symbolType) {
		case Cross: setDefaultSymbol(new CrossSymbol(size)); break;
		case Diamond: setDefaultSymbol(new DiamondSymbol(size)); break;
		case Oval: setDefaultSymbol(new OvalSymbol(size)); break;
		case Plus: setDefaultSymbol(new PlusSymbol(size)); break;
		case Square: setDefaultSymbol(new SquareSymbol(size)); break;
		case Triangle: setDefaultSymbol(new TriangleSymbol(size)); break;
		}
	}
	
	public void setDisplaySymbols(Boolean value) {
		displaySymbols = value != null ? value : true;
		clearCanvasCacheAndRedraw();
	}
	
	public void setDefaultSymbolSize(int size) {
		defaultSymbol.setSizeHint(size);
		clearCanvasCacheAndRedraw();
	}
	
	public void setHideLines(Boolean value) {
		hideLines = value != null ? value : false;
		clearCanvasCacheAndRedraw();
	}
	
	public int getAntialias() {
		return antialias;
	}

	public void setAntialias(int antialias) {
		this.antialias = antialias;
		clearCanvasCacheAndRedraw();
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
	protected void beforePaint() {
		super.beforePaint();
	}

	@Override
	protected void paintCachableLayer(Graphics graphics) {
		graphics.setAntialias(antialias);
		IVectorPlotter plotter = hideLines ? NULL_PLOTTER : defaultPlotter;
		IChartSymbol symbol = displaySymbols ? defaultSymbol : NULL_SYMBOL;
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			graphics.setForegroundColor(ColorFactory.getGoodColor(series));
			graphics.setBackgroundColor(ColorFactory.getGoodColor(series));
			plotter.plot(dataset, series, graphics, this, symbol);
		}
	}

	@Override
	protected void paintNoncachableLayer(Graphics graphics) {
		Insets insets = getInsets();
		graphics.setAntialias(antialias);

		// draw insets border
		graphics.setForegroundColor(insetsBackgroundColor);
		graphics.setBackgroundColor(insetsBackgroundColor);
		graphics.fillRectangle(0, 0, getWidth(), insets.top); // top
		graphics.fillRectangle(0, getHeight()-insets.bottom, getWidth(), insets.bottom); // bottom
		graphics.fillRectangle(0, 0, insets.left, getHeight()); // left
		graphics.fillRectangle(getWidth()-insets.right, 0, insets.right, getHeight()); // right

		graphics.setForegroundColor(insetsLineColor);
		graphics.drawRectangle(insets.left-1, insets.top-1, getWidth()-insets.getWidth()+1, getHeight()-insets.getHeight()+1);

		Rectangle rect = graphics.getClip(new Rectangle());

		//draw X ticks
		double startX = fromCanvasX(rect.x);
		double endX = fromCanvasX(rect.x + rect.width);
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

		for (BigDecimal t=tickStartX; t.compareTo(tickEndX)<0; t = t.add(tickIntvlX)) {
			int x = toCanvasX(t.doubleValue());
			graphics.setLineStyle(SWT.LINE_DOT);
			graphics.setForegroundColor(tickLineColor);
			graphics.drawLine(x, 0, x, getHeight());
			graphics.setForegroundColor(tickLabelColor);
			String str = t.toPlainString() + "s";
			graphics.setBackgroundColor(insetsBackgroundColor);
			graphics.fillText(str, x + 3, 2);
			graphics.fillText(str, x + 3, getHeight() - 16);
		}

		// Y axis
		// XXX don't draw if falls into gutter area
		if (startX<=0 && endX>=0) {
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.drawLine(toCanvasX(0), 0, toCanvasX(0), getHeight());
		}
		
		//draw Y ticks
		//XXX factor out common code with X axis ticks
		double startY = fromCanvasY(rect.y);
		double endY = fromCanvasY(rect.y + rect.width);
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

		for (BigDecimal t=tickStartY; t.compareTo(tickEndY)<0; t = t.add(tickIntvlY)) {
			int y = toCanvasY(t.doubleValue());
			graphics.setLineStyle(SWT.LINE_DOT);
			graphics.setForegroundColor(tickLineColor);
			graphics.drawLine(0, y, getWidth(), y);
			graphics.setForegroundColor(tickLabelColor);
			String str = t.toPlainString() + "s";
			graphics.setBackgroundColor(insetsBackgroundColor);
			graphics.fillText(str, 2, y+3);
			graphics.fillText(str, getWidth() - 16, y+3);
		}

		// X axis
		// XXX don't draw if falls into gutter area
		//if (startY<=0 && endY>=0) { //XXX with this "if" sometimes it doesn't draw
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.drawLine(0, toCanvasY(0), getWidth(), toCanvasY(0));
		//}
	}
}
