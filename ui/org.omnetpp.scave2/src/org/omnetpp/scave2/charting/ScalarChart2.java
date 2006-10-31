package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_TITLE;

import java.math.BigDecimal;
import java.math.RoundingMode;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.Transform;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GeomUtils;
import org.omnetpp.scave2.model.ChartProperties.BarPlacement;

public class ScalarChart2 extends ChartCanvas {

	private static final double DEFAULT_BAR_BASELINE = 0.0;
	private static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
	private static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.defaultBackground;
	private static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.asColor("grey80");

	private static final boolean DEFAULT_INVERT_XY = false;
	private static final boolean DEFAULT_SHOW_GRID = false;
	private static final boolean DEFAULT_Y_LOGARITHMIC = false;
	
	private CategoryDataset dataset;

	private int widthBar = 10;
	private int hgapMinor = 5;
	private int hgapMajor = 20;

	private ValueAxis valueAxis = new ValueAxis();
	private DomainAxis domainAxis = new DomainAxis();
	private Plot plot = new Plot();

	private double barBaseline;
	private BarPlacement barPlacement;
	private Color backgroundColor = DEFAULT_BACKGROUND_COLOR;
	private Color barOutlineColor = DEFAULT_BAR_OUTLINE_COLOR;

	private Double yMin;
	private Double yMax;
	private Boolean invertXY;
	private Boolean gridVisible;
	
	public ScalarChart2(Composite parent, int style) {
		super(parent, style | SWT.DOUBLE_BUFFERED);
		setCaching(true);
		setInsets(new Insets(0,0,0,0));
	}

	public void setDataset(CategoryDataset dataset) {
		this.dataset = dataset;
		setLegend();
		calculateArea();
		scheduleRedraw();
	}
	
	private void setLegend() {
		legend.clearLegendItems();
		for (int i = 0; i < dataset.getColumnCount(); ++i) {
			legend.addLegendItem(getBarColor(i), dataset.getColumnKey(i).toString());
		}
	}
	
	private void calculateArea() {
		int cRows = dataset.getRowCount();
		int cColumns = dataset.getColumnCount();
		double minX = getLeftX(0, 0);
		double maxX = getRightX(cRows - 1, cColumns - 1);
		double minY = Double.MAX_VALUE;
		double maxY = Double.MIN_VALUE;
		for (int row = 0; row < cRows; ++row)
			for (int column = 0; column < cColumns; ++column) {
				minY = Math.min(minY, getBottomY(row, column));
				maxY = Math.max(maxY, getTopY(row, column));
			}
		setArea(minX, minY, maxX, maxY);
	}

	/*=============================================
	 *               Properties
	 *=============================================*/
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToSwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setLabelFont(Converter.stringToSwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			setXAxisLabelsRotatedBy(Converter.stringToDouble(value));
		// Bars
		else if (PROP_BAR_BASELINE.equals(name))
			setBarBaseline(Converter.stringToDouble(value));
		else if (PROP_BAR_PLACEMENT.equals(name))
			setBarPlacement(Converter.stringToEnum(value, BarPlacement.class));
		// Axes
		else if (PROP_XY_INVERT.equals(name))
			setInvertXY(Converter.stringToBoolean(value));
		else if (PROP_Y_AXIS_MIN.equals(name))
			setYMin(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MAX.equals(name))
			setYMax(Converter.stringToDouble(value));
		else if (PROP_XY_GRID.equals(name))
			setGridVisible(Converter.stringToBoolean(value));
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			setYLogarithmic(Converter.stringToBoolean(value));
		else
			super.setProperty(name, value);
	}

	public String getTitle() {
		return title.getText();
	}

	public Font getTitleFont() {
		return title.getFont();
	}

	public String getXAxisTitle() {
		return domainAxis.title;
	}

	public void setXAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_X_AXIS_TITLE;

		domainAxis.title = title;
		scheduleRedraw();
	}

	public String getYAxisTitle() {
		return valueAxis.title;
	}

	public void setYAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_Y_AXIS_TITLE;

		valueAxis.title = title;
		scheduleRedraw();
	}

	public Font getAxisTitleFont() {
		return domainAxis.titleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font == null)
			return;

		domainAxis.titleFont = font;
		valueAxis.font = font;
		scheduleRedraw();
	}

	public void setLabelFont(Font font) {
		if (font == null)
			font = DEFAULT_DOMAIN_LABELS_FONT;
		domainAxis.font = font;
		scheduleRedraw();
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;
		domainAxis.rotation = angle;
		scheduleRedraw();
	}

	public Double getBarBaseline() {
		return barBaseline;
	}

	public void setBarBaseline(Double value) {
		if (value == null)
			value = DEFAULT_BAR_BASELINE;

		barBaseline = value;
		scheduleRedraw();
	}

	public BarPlacement getBarPlacement() {
		return barPlacement;
	}

	public void setBarPlacement(BarPlacement value) {
		if (value == null)
			value = DEFAULT_BAR_PLACEMENT;

		barPlacement = value;
		scheduleRedraw();
	}

	public Double getYMin() {
		return yMin;
	}

	public void setYMin(Double value) {
		yMin = value;
		scheduleRedraw();
	}

	public Double getYMax() {
		return yMax;
	}

	public void setYMax(Double value) {
		yMax = value;
		scheduleRedraw();
	}

	public Boolean getInvertXY() {
		return invertXY;
	}

	public void setInvertXY(Boolean value) {
		if (value == null)
			value = DEFAULT_INVERT_XY;

		invertXY = value;
		scheduleRedraw();
	}

	public Boolean getGridVisible() {
		return gridVisible;
	}

	public void setGridVisible(Boolean value) {
		if (value == null)
			value = DEFAULT_SHOW_GRID;

		gridVisible = value;
		scheduleRedraw();
	}

	private void setYLogarithmic(Boolean value) {
		if (value == null)
			value = DEFAULT_Y_LOGARITHMIC;
		scheduleRedraw();
	}
	
	/*=============================================
	 *               Drawing
	 *=============================================*/

	@Override
	protected void beforePaint(GC gc) {
		// Calculate space occupied by title and legend and set insets accordingly
		Rectangle remaining = title.layout(gc, getClientArea());
		remaining = legend.layout(gc, remaining);
		remaining = valueAxis.layout(gc, remaining);
		remaining = domainAxis.layout(gc, remaining);
		remaining = plot.layout(gc, remaining);

		Insets insets = GeomUtils.subtract(getClientArea(), remaining);
		setInsets(insets);
		super.beforePaint(gc);
	}
	
	
	@Override
	protected void paintCachableLayer(Graphics graphics) {
		org.eclipse.draw2d.geometry.Rectangle clip = graphics.getClip(new org.eclipse.draw2d.geometry.Rectangle());
		
		graphics.setBackgroundColor(backgroundColor);
		graphics.fillRectangle(clip);
		
		RowColumn rcStart = getRowColumn(clip.x, true);
		RowColumn rcEnd = getRowColumn(clip.x + clip.width, false);
		for (int row = rcStart.row; row <= rcEnd.row; ++row) {
			int columnStart = row == rcStart.row ? rcStart.column : 0;
			int columnEnd = row == rcEnd.row ? rcEnd.column : dataset.getColumnCount() - 1; 
			for (int column = columnStart; column <= columnEnd; ++column)
				drawBar(graphics, row, column);
		}
	}
	
	protected void drawBar(Graphics graphics, int row, int column) {
		int x = toCanvasX(getLeftX(row, column));
		int y = toCanvasY(getTopY(row, column));
		int width = toCanvasDistX(widthBar);
		int height = toCanvasDistY(getTopY(row, column) - getBottomY(row, column));
		graphics.setForegroundColor(barOutlineColor);
		graphics.drawRectangle(x, y, width, height);
		if (width > 1 && height > 1) {
			graphics.setBackgroundColor(getBarColor(column));
			graphics.fillRectangle(x+1, y+1, width-2, height-2);
		}
	}
	
	protected Color getBarColor(int column) {
		return ColorFactory.getGoodColor(column);
	}
	
	protected double getLeftX(int row, int column) {
		int cColumns = dataset.getColumnCount();
		double rowWidth = cColumns * widthBar + (cColumns - 1) * hgapMinor;
		return row * (rowWidth + hgapMajor) + column * (widthBar + hgapMinor); 
	}
	
	protected double getRightX(int row, int column) {
		return getLeftX(row, column) + widthBar;
	}
	
	protected double getTopY(int row, int column) {
		double value = dataset.getValue(row, column).doubleValue();
		return (value > barBaseline ? value : barBaseline);
	}
	
	protected double getBottomY(int row, int column) {
		double value = dataset.getValue(row, column).doubleValue();
		return (value < barBaseline ? value : barBaseline);
	}
	
	private RowColumn getRowColumn(double x, boolean before) {
		int cRows = dataset.getRowCount();
		int cColumns = dataset.getColumnCount();
		return before ? new RowColumn(0, 0) : new RowColumn(cRows-1, cColumns - 1);
	}
	
	static class RowColumn {
		public int row;
		public int column;
		
		public RowColumn(int row, int column) {
			this.row = row;
			this.column = column;
		}
	}
	
	private Rectangle getPlotRectagle() {
		return plot.getRectangle();
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		// TODO: draw axes
		title.draw(gc);
		legend.draw(gc);
		valueAxis.draw(gc);
		domainAxis.draw(gc);
	}
	
	class Plot {
		private Rectangle rect;
		
		public Rectangle getRectangle() {
			return rect;
		}
		
		public Rectangle layout(GC gc, Rectangle rect) {
			this.rect = rect;
			return rect;
		}
		
		public void draw(GC gc) {
			
		}
	}
	
	private static final String DEFAULT_X_AXIS_TITLE = "";
	private static final String DEFAULT_Y_AXIS_TITLE = "";
	private static final Font DEFAULT_AXIS_TITLE_FONT = new Font(null, "Arial", 10, SWT.NORMAL);
	private static final Font DEFAULT_DOMAIN_LABELS_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	private static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	
	class DomainAxis {
		private Rectangle rect;
		private int labelsHeight;
		private String title = DEFAULT_X_AXIS_TITLE;
		private Font titleFont = DEFAULT_AXIS_TITLE_FONT;
		private Font font = DEFAULT_DOMAIN_LABELS_FONT;
		private double rotation = DEFAULT_X_LABELS_ROTATED_BY;
		
		public Rectangle layout(GC gc, Rectangle rect) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			//
			graphics.setFont(titleFont);
			int titleHeight = gc.textExtent(title).y;
			//
			gc.setFont(font);
			labelsHeight = 0;
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Point size = gc.textExtent(label);
				System.out.println("Height: " + size.y);
				labelsHeight = Math.max(labelsHeight, size.y);
			}
			
			int height = 10 + labelsHeight + titleHeight;
			graphics.popState();
			graphics.dispose();
			
			this.rect = new Rectangle(rect.x, rect.y + rect.height - height, rect.width, height);
			return new Rectangle(rect.x, rect.y, rect.width, Math.max(rect.height - height, 0));
		}
		
		public void draw(GC gc) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			// draw axis
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.setLineWidth(1);
			graphics.setForegroundColor(ColorFactory.asColor("black"));
			Rectangle plotRect = getPlotRectagle();
			graphics.drawLine(plotRect.x, rect.y + 5, plotRect.x + plotRect.width, rect.y + 5);
			// draw labels
			int cColumns = dataset.getColumnCount();
			graphics.setFont(font);
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Point size = gc.textExtent(label);
				int left = toCanvasX(getLeftX(row, 0));
				int right = toCanvasX(getRightX(row, cColumns - 1));
				
				graphics.restoreState();
				graphics.translate(left + (right - left - size.x) / 2, rect.y + 10 + labelsHeight / 2);
				graphics.rotate((float)rotation);
				graphics.drawText(label, - (right - left - size.x) / 2, - labelsHeight / 2);
			}
			
			// draw axis title
			graphics.setFont(titleFont);
			Point size = gc.textExtent(title);
			graphics.drawText(title, plotRect.x + (plotRect.width - size.x) / 2, rect.y + 10);

			graphics.popState();
			graphics.dispose();
		}
	}
	
	class ValueAxis {
		private Rectangle rect;
		Point axisStart, axisEnd;
		Rectangle titleRect;
		
		private String title = DEFAULT_Y_AXIS_TITLE;
		private Font font = DEFAULT_AXIS_TITLE_FONT;
		
		public Rectangle layout(GC gc, Rectangle rect) {
			gc.setFont(font);
			Point titleSize = gc.textExtent(title);
			int width = titleSize.y + 30 + 10; 
			axisStart = new Point(rect.x + width - 5, rect.y);
			axisEnd = new Point(rect.x + width - 5, rect.y + rect.height);
			titleRect = new Rectangle(rect.x, rect.y + (rect.height - titleSize.x) / 2, titleSize.y, titleSize.x);
			this.rect = new Rectangle(rect.x, rect.y, width, rect.height);
			return new Rectangle(rect.x + width, rect.y, Math.max(rect.width - width, 0), rect.height);
		}
		
		public void draw(GC gc) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();

			// synchronize layout rectangle with plot's rectangle
			Rectangle plotRect = getPlotRectagle();
			layout(gc, new Rectangle(rect.x, plotRect.y, rect.width, plotRect.height));
			// draw line
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.setLineWidth(1);
			graphics.setForegroundColor(ColorFactory.asColor("black"));
			graphics.drawLine(axisStart.x, axisStart.y, axisEnd.x, axisEnd.y);
			// draw ticks
			Ticks ticks = new Ticks(fromCanvasY(plotRect.y + plotRect.height),
									fromCanvasY(plotRect.y),
									100 / getZoomY());
			for (BigDecimal tick : ticks) {
				int y = toCanvasY(tick.doubleValue());
				if (y >= plotRect.y && y <= plotRect.y + plotRect.height) {
					graphics.drawLine(axisStart.x - 3, y, axisStart.x, y);
					String label = tick.toPlainString();
					Point size = gc.textExtent(label);
					graphics.drawText(label, axisStart.x - 2 - size.x, y - size.y / 2);
				}
			}
			// draw title
			graphics.setFont(font);
			graphics.translate(titleRect.x + titleRect.width / 2, titleRect.y + titleRect.y / 2);
			graphics.rotate(-90);
			graphics.drawText(title, - titleRect.height / 2, - titleRect.width / 2);

			graphics.popState();
			graphics.dispose();
		}
	}
	
}
