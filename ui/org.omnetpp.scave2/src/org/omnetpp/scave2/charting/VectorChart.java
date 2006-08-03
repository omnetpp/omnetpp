package org.omnetpp.scave2.charting;

import java.math.BigDecimal;
import java.math.RoundingMode;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave2.charting.plotter.IChartSymbol;
import org.omnetpp.scave2.charting.plotter.IVectorPlotter;
import org.omnetpp.scave2.charting.plotter.LinesVectorPlotter;
import org.omnetpp.scave2.charting.plotter.SquareSymbol;

//XXX strange effects when resized and vertical scrollbar pulled...
public class VectorChart extends ZoomableCachingCanvas {
	private static final Color TICK_LINE_COLOR = new Color(null, 160, 160, 160);
	private static final Color TICK_LABEL_COLOR = new Color(null, 0, 0, 0);
	private static final Color INSETS_BACKGROUND_COLOR = new Color(null, 255, 255, 160);
	private static final Color INSETS_LINE_COLOR = new Color(null, 0, 0, 0);
	
	private static final int TICK_SPACING = 100; // space between ticks in pixels

	private XYDataset dataset;
	private IVectorPlotter defaultPlotter = new LinesVectorPlotter();
	private IChartSymbol defaultSymbol = new SquareSymbol(3);
	private int antialias = SWT.ON; // SWT.ON, SWT.OFF, SWT.DEFAULT
	
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

	public IVectorPlotter getDefaultLineType() {
		return defaultPlotter;
	}

	public void setDefaultLineType(IVectorPlotter defaultPlotter) {
		this.defaultPlotter = defaultPlotter;
		clearCanvasCacheAndRedraw();
	}

	public IChartSymbol getDefaultSymbol() {
		return defaultSymbol;
	}

	public void setDefaultSymbol(IChartSymbol defaultSymbol) {
		this.defaultSymbol = defaultSymbol;
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
		for (int series=0; series<dataset.getSeriesCount(); series++) {
			graphics.setForegroundColor(ColorFactory.getGoodColor(series));
			graphics.setBackgroundColor(ColorFactory.getGoodColor(series));
			defaultPlotter.plot(dataset, series, graphics, this, defaultSymbol);
		}
	}

	@Override
	protected void paintNoncachableLayer(Graphics graphics) {
		Insets insets = getInsets();
		graphics.setAntialias(antialias);

		// draw insets border
		graphics.setForegroundColor(INSETS_BACKGROUND_COLOR);
		graphics.setBackgroundColor(new Color(null, 255, 255, 180));
		graphics.fillRectangle(0, 0, getWidth(), insets.top); // top
		graphics.fillRectangle(0, getHeight()-insets.bottom, getWidth(), insets.bottom); // bottom
		graphics.fillRectangle(0, 0, insets.left, getHeight()); // left
		graphics.fillRectangle(getWidth()-insets.right, 0, insets.right, getHeight()); // right

		graphics.setForegroundColor(INSETS_LINE_COLOR);
		graphics.drawRectangle(insets.left-1, insets.top-1, getWidth()-insets.getWidth()+1, getHeight()-insets.getHeight()+1);

		Rectangle rect = graphics.getClip(new Rectangle());

		//draw X ticks
		double startX = fromCanvasX(rect.x);
		double endX = fromCanvasX(rect.x + rect.width);
		int tickScale = (int)Math.ceil(Math.log10(TICK_SPACING / getZoomX()));
		BigDecimal tickSpacing = BigDecimal.valueOf(TICK_SPACING / getZoomX());
		BigDecimal tickStart = new BigDecimal(startX).setScale(-tickScale, RoundingMode.FLOOR);
		BigDecimal tickEnd = new BigDecimal(endX).setScale(-tickScale, RoundingMode.CEILING);
		BigDecimal tickIntvl = new BigDecimal(1).scaleByPowerOfTen(tickScale);

		// use 2, 4, 6, 8, etc. if possible
		if (tickIntvl.divide(BigDecimal.valueOf(5)).compareTo(tickSpacing) > 0)
			tickIntvl = tickIntvl.divide(BigDecimal.valueOf(5));
		// use 5, 10, 15, 20, etc. if possible
		else if (tickIntvl.divide(BigDecimal.valueOf(2)).compareTo(tickSpacing) > 0)
			tickIntvl = tickIntvl.divide(BigDecimal.valueOf(2));

		for (BigDecimal t=tickStart; t.compareTo(tickEnd)<0; t = t.add(tickIntvl)) {
			int x = toCanvasX(t.doubleValue());
			graphics.setLineStyle(SWT.LINE_DOT);
			graphics.setForegroundColor(TICK_LINE_COLOR);
			graphics.drawLine(x, 0, x, getHeight());
			graphics.setForegroundColor(TICK_LABEL_COLOR);
			String str = t.toPlainString() + "s";
			graphics.setBackgroundColor(INSETS_BACKGROUND_COLOR);
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
		int tickScaleY = (int)Math.ceil(Math.log10(TICK_SPACING / getZoomY()));
		BigDecimal tickSpacingY = BigDecimal.valueOf(TICK_SPACING / getZoomY());
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
			graphics.setForegroundColor(TICK_LINE_COLOR);
			graphics.drawLine(0, y, getWidth(), y);
			graphics.setForegroundColor(TICK_LABEL_COLOR);
			String str = t.toPlainString() + "s";
			graphics.setBackgroundColor(INSETS_BACKGROUND_COLOR);
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
