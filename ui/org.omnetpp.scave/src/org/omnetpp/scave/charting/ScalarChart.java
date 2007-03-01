package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_BASELINE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_OUTLINE_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_INVERT_XY;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_TITLE;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GeomUtils;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;

/**
 * Bar chart.
 *
 * @author tomi
 */
public class ScalarChart extends ChartCanvas {
	private CategoryDataset dataset;

	private LinearAxis valueAxis = new LinearAxis(this, true);
	private DomainAxis domainAxis = new DomainAxis();
	private BarPlot plot = new BarPlot();

	private Double yMin;
	private Double yMax;
	
	public ScalarChart(Composite parent, int style) {
		super(parent, style | SWT.DOUBLE_BUFFERED);
		setCaching(true);
		setInsets(new Insets(0,0,0,0));
	}

	public void setDataset(CategoryDataset dataset) {
		this.dataset = dataset;
		updateLegend();
		updateArea();
		scheduleRedraw();
	}
	
	private void updateLegend() {
		legend.clearLegendItems();
		for (int i = 0; i < dataset.getColumnCount(); ++i) {
			legend.addLegendItem(plot.getBarColor(i), dataset.getColumnKey(i).toString());
		}
	}
	
	private void updateArea() {
		PlotArea area = plot.getPlotArea();
		setArea(area.minX, area.minY, area.maxX, area.maxY);
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
			throw new IllegalArgumentException("Logarithmic axis not yet supported"); //TODO
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
		return valueAxis.getTitle();
	}

	public void setYAxisTitle(String title) {
		valueAxis.setTitle(title==null ? DEFAULT_Y_AXIS_TITLE : title);
		scheduleRedraw();
	}

	public Font getAxisTitleFont() {
		return domainAxis.titleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font != null) {
			domainAxis.titleFont = font;
			valueAxis.setTitleFont(font);
			scheduleRedraw();
		}
	}

	public void setLabelFont(Font font) {
		if (font == null)
			font = DEFAULT_LABELS_FONT;
		domainAxis.labelsFont = font;
		valueAxis.setTickFont(font);
		scheduleRedraw();
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;
		domainAxis.rotation = angle;
		scheduleRedraw();
	}

	public Double getBarBaseline() {
		return plot.barBaseline;
	}

	public void setBarBaseline(Double value) {
		if (value == null)
			value = DEFAULT_BAR_BASELINE;

		plot.barBaseline = value;
		scheduleRedraw();
	}

	public BarPlacement getBarPlacement() {
		return plot.barPlacement;
	}

	public void setBarPlacement(BarPlacement value) {
		if (value == null)
			value = DEFAULT_BAR_PLACEMENT;

		plot.barPlacement = value;
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
		return plot.invertXY;
	}

	public void setInvertXY(Boolean value) {
		if (value == null)
			value = DEFAULT_INVERT_XY;

		plot.invertXY = value;
		scheduleRedraw();
	}

	public Boolean getGridVisible() {
		return valueAxis.isGridVisible();
	}

	public void setGridVisible(Boolean value) {
		if (value == null)
			value = DEFAULT_SHOW_GRID;

		valueAxis.setGridVisible(value);
		scheduleRedraw();
	}

	/*=============================================
	 *               Drawing
	 *=============================================*/

	@Override
	protected void beforePaint(GC gc) {
		// ZoomableCachingCanvas's beforePaint() must be called, as it does zoom validation etc.
		super.beforePaint(gc);
		
		// Calculate space occupied by title and legend and set insets accordingly
		Rectangle area = new Rectangle(getClientArea());
		Rectangle remaining = title.layout(gc, area);
		remaining = legend.layout(gc, remaining);
		
		Rectangle areaMinusLegend = remaining.getCopy();
		Insets insets = new Insets();
		valueAxis.layoutHint(gc, areaMinusLegend, insets);
		remaining = domainAxis.layout(gc, remaining);
		remaining = plot.layout(gc, remaining);

		setInsets(insets);
		super.beforePaint(gc);
	}
	
	
	@Override
	protected void paintCachableLayer(GC gc) {
		valueAxis.drawGrid(gc);
		plot.draw(gc);
	}
	
	private Rectangle getPlotRectagle() {
		return plot.getRectangle();
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		title.draw(gc);
		legend.draw(gc);
		valueAxis.drawAxis(gc);
		domainAxis.draw(gc);
	}

	class BarPlot {
		private Rectangle rect;
		private int widthBar = 10;
		private int hgapMinor = 5;
		private int hgapMajor = 20;
		private int inset = 10;
		
		private double barBaseline = DEFAULT_BAR_BASELINE;
		private BarPlacement barPlacement = DEFAULT_BAR_PLACEMENT;
		private Color barOutlineColor = DEFAULT_BAR_OUTLINE_COLOR;
		private Color backgroundColor = DEFAULT_BACKGROUND_COLOR;
		private Boolean invertXY = DEFAULT_INVERT_XY;
		
		public Rectangle getRectangle() {
			return rect;
		}
		
		public Rectangle layout(GC gc, Rectangle rect) {
			this.rect = rect;
			return rect;
		}
		
		public void draw(GC gc) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			
			Rectangle clip = graphics.getClip(new Rectangle());
			graphics.setBackgroundColor(backgroundColor);
			graphics.fillRectangle(clip);
			
			int cColumns = dataset.getColumnCount();
			int[] indeces = getRowColumnsInRectangle(clip);
			for (int i = indeces[0]; i <= indeces[1]; ++i) {
				int row = i / cColumns;
				int column = i % cColumns;
				drawBar(graphics, row, column);
			}
			graphics.popState();
			graphics.dispose();
		}
		
		protected void drawBar(Graphics graphics, int row, int column) {
			Rectangle rect = getBarRectangle(row, column);
			graphics.setForegroundColor(barOutlineColor);
			graphics.drawRectangle(rect);
			if (rect.width > 1 && rect.height > 1) {
				graphics.setBackgroundColor(getBarColor(column));
				graphics.fillRectangle(rect.getCropped(new Insets(1,1,1,1)));
			}
		}
		
		protected int[] getRowColumnsInRectangle(org.eclipse.draw2d.geometry.Rectangle rect) {
			int[] result = new int[2];
			result[0] = getRowColumn(rect.x, true);
			result[1] = getRowColumn(rect.x + rect.width, false);
			return result;
		}
		
		private int getRowColumn(double x, boolean before) {
			int cRows = dataset.getRowCount();
			int cColumns = dataset.getColumnCount();
			return before ? 0 : (cRows*cColumns-1);
		}
		
		protected Color getBarColor(int column) {
			return ColorFactory.getGoodColor(column);
		}
		
		protected Rectangle getBarRectangle(int row, int column) {
			int x = toCanvasX(getLeftX(row, column));
			int y = toCanvasY(getTopY(row, column));
			int width = toCanvasDistX(getRightX(row,column) - getLeftX(row, column));
			int height = toCanvasDistY(getTopY(row, column) - getBottomY(row, column));
			return new Rectangle(x, y, width, height);
		}
		
		public PlotArea getPlotArea() {
			int cRows = dataset.getRowCount();
			int cColumns = dataset.getColumnCount();
			double minX = getLeftX(0, 0);
			double maxX = getRightX(cRows - 1, cColumns - 1);
			double minY = Double.MAX_VALUE;
			double maxY = Double.MIN_VALUE;
			for (int row = 0; row < cRows; ++row)
				for (int column = 0; column < cColumns; ++column) {
					minY = Math.min(minY, plot.getBottomY(row, column));
					maxY = Math.max(maxY, plot.getTopY(row, column));
				}
			return new PlotArea(minX - inset, maxX + inset, minY, maxY + inset);
		}
		
		protected double getLeftX(int row, int column) {
			int cColumns = dataset.getColumnCount();
			double rowWidth = cColumns * widthBar + (cColumns - 1) * hgapMinor;
			return inset + row * (rowWidth + hgapMajor) + column * (widthBar + hgapMinor); 
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
	}


	class DomainAxis {
		private Rectangle rect;
		private int labelsHeight;
		private String title = DEFAULT_X_AXIS_TITLE;
		private Font titleFont = DEFAULT_AXIS_TITLE_FONT;
		private Font labelsFont = DEFAULT_LABELS_FONT;
		private double rotation = DEFAULT_X_LABELS_ROTATED_BY;
		
		public Ticks getTicks() {
			return new Ticks(1.0, 0.0, 1.0); // TODO
		}

		public Rectangle layout(GC gc, Rectangle rect) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			//
			graphics.setFont(titleFont);
			int titleHeight = gc.textExtent(title).y;
			//
			gc.setFont(labelsFont);
			labelsHeight = 0;
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Dimension size = GeomUtils.rotatedSize(new Dimension(gc.textExtent(label)), rotation);
				labelsHeight = Math.max(labelsHeight, size.height);
			}
			
			int height = 10 + labelsHeight + titleHeight;
			graphics.popState();
			graphics.dispose();
			
			this.rect = new Rectangle(rect.x, rect.bottom() - height, rect.width, height);
			return new Rectangle(rect.x, rect.y, rect.width, Math.max(rect.height - height, 0));
		}
		
		public void draw(GC gc) {
			Graphics graphics = new SWTGraphics(gc);
			graphics.pushState();
			graphics.setClip(rect);
			// draw axis
			graphics.setLineStyle(SWT.LINE_SOLID);
			graphics.setLineWidth(1);
			graphics.setForegroundColor(ColorFactory.asColor("black"));
			Rectangle plotRect = getPlotRectagle();
			//graphics.drawLine(plotRect.x, rect.y + 5, plotRect.x + plotRect.width, rect.y + 5);
			// draw labels
			int cColumns = dataset.getColumnCount();
			graphics.setFont(labelsFont);
			graphics.pushState();
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Point size = gc.textExtent(label);
				int left = plot.getBarRectangle(row, 0).x;
				int right = plot.getBarRectangle(row, cColumns - 1).right();
				
				graphics.restoreState();
				graphics.drawLine(left, rect.y + 5, right, rect.y + 5);
				graphics.translate((left + right) / 2, rect.y + 10);
				graphics.rotate((float)rotation);
				graphics.drawText(label, -size.x / 2, - labelsHeight / 2);
			}
			graphics.popState();
			
			// draw axis title
			graphics.setFont(titleFont);
			Point size = gc.textExtent(title);
			graphics.drawText(title, plotRect.x + (plotRect.width - size.x) / 2, rect.y + 10);

			graphics.popState();
			graphics.dispose();
		}
	}
}
