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

import org.eclipse.draw2d.Graphics;
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

	private static final String DEFAULT_X_AXIS_TITLE = "";
	private static final String DEFAULT_Y_AXIS_TITLE = "";

	private static final double DEFAULT_BAR_BASELINE = 0.0;
	private static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
	private static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.defaultBackground;
	private static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.asColor("grey80");

	private static final boolean DEFAULT_INVERT_XY = false;
	private static final boolean DEFAULT_SHOW_GRID = false;
	private static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	private static final boolean DEFAULT_Y_LOGARITHMIC = false;
	
	private CategoryDataset dataset;

	private int widthBar = 10;
	private int hgapMinor = 5;
	private int hgapMajor = 20;
	
	
	private String xAxisTitle;
	private String yAxisTitle;
	private Font axisTitleFont;
	private RowLabels rowLabels = new RowLabels(null, DEFAULT_X_LABELS_ROTATED_BY);

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
		return xAxisTitle;
	}

	public void setXAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_X_AXIS_TITLE;

		xAxisTitle = title;
		scheduleRedraw();
	}

	public String getYAxisTitle() {
		return yAxisTitle;
	}

	public void setYAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_Y_AXIS_TITLE;

		yAxisTitle = title;
		scheduleRedraw();
	}

	public Font getAxisTitleFont() {
		return axisTitleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font == null)
			return;

		axisTitleFont = font;
		scheduleRedraw();
	}

	public void setLabelFont(Font font) {
		if (font == null)
			return;

		rowLabels.setFont(font);
		scheduleRedraw();
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;

		rowLabels.setRotation(angle);
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
		remaining = rowLabels.layout(gc, remaining);

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

	@Override
	protected void paintNoncachableLayer(GC gc) {
		// TODO: draw axes
		title.draw(gc);
		legend.draw(gc);
		rowLabels.draw(gc);
	}
	
	class RowLabels {
		
		private Font font;
		private Transform transform = new Transform(getDisplay()); // XXX
		private double rotation;
		
		private Rectangle bounds;
		
		public RowLabels(Font font, double rotation) {
			this.font = font;
			this.rotation = rotation;
		}
		
		public void setFont(Font font) {
			this.font = font;
		}
		
		public void setRotation(double angle) {
			this.rotation = angle;
		}
		
		public Rectangle layout(GC gc, Rectangle parent) {
			Font saveFont = gc.getFont();
			if (font != null)
				gc.setFont(font);
			
			gc.getTransform(transform);
			transform.rotate((float)rotation);
			gc.setTransform(transform);

			int height = 0;
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Point size = gc.textExtent(label);
				System.out.println("Height: " + size.y);
				height = Math.max(height, size.y);
			}
			
			transform.rotate((float)-rotation);
			gc.setTransform(transform);
			gc.setFont(saveFont);
			
			bounds = new Rectangle(
						parent.x, parent.y + parent.height - height,
						parent.width, height);
			
			return new Rectangle(parent.x, parent.y, parent.width, parent.height - height);
		}
		
		public void draw(GC gc) {
			int cColumns = dataset.getColumnCount();
			
			Font saveFont = gc.getFont();
			if (font != null)
				gc.setFont(font);
			
			gc.getTransform(transform);
			transform.rotate((float)rotation);
			gc.setTransform(transform);
			
			for (int row = 0; row < dataset.getRowCount(); ++row) {
				String label = dataset.getRowKey(row).toString();
				Point size = gc.textExtent(label);
				int left = toCanvasX(getLeftX(row, 0));
				int right = toCanvasX(getRightX(row, cColumns - 1));
				gc.drawText(label, left + (right - left - size.x) / 2, bounds.y);
			}
			
			transform.rotate((float)-rotation);
			gc.setTransform(transform);
			gc.setFont(saveFont);
		}
	}
}
