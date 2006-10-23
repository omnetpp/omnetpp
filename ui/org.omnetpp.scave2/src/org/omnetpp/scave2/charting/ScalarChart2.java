package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_POSITION;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_TITLE;

import java.awt.Font;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave2.model.ChartProperties.BarPlacement;
import org.omnetpp.scave2.model.ChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.ChartProperties.LegendPosition;

public class ScalarChart2 extends ZoomableCachingCanvas {

	private static final String DEFAULT_TITLE = "";
	private static final String DEFAULT_X_AXIS_TITLE = "";
	private static final String DEFAULT_Y_AXIS_TITLE = "";

	private static final double DEFAULT_BAR_BASELINE = 0.0;
	private static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
	private static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.defaultBackground;
	private static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.asColor("grey80");
	
	private static final boolean DEFAULT_DISPLAY_LEGEND = false;
	private static final boolean DEFAULT_LEGEND_BORDER = false;
	private static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Below;
	private static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;

	private static final boolean DEFAULT_INVERT_XY = false;
	private static final boolean DEFAULT_SHOW_GRID = false;
	private static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	private static final boolean DEFAULT_Y_LOGARITHMIC = false;
	
	private CategoryDataset dataset;

	private int widthBar = 10;
	private int hgapMinor = 5;
	private int hgapMajor = 20;
	
	
	private String title;
	private Font titleFont;
	private String xAxisTitle;
	private String yAxisTitle;
	private Font axisTitleFont;
	private Font tickLabelFont;
	private double xAxisLabelsRotatedBy;

	private boolean displayLegend;
	private Font legendFont;
	private boolean legendBorder;
	private LegendPosition legendPosition;
	private LegendAnchor legendAnchor;

	private Double barBaseline;
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
		calculateArea();
		scheduleRefresh();
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
				double value = dataset.getValue(row, column).doubleValue();
				minY = Math.min(minY, value);
				maxY = Math.max(maxY, value);
			}
		setArea(minX, minY, maxX, maxY);
	}

	/*=============================================
	 *               Properties
	 *=============================================*/
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setLabelFont(Converter.stringToAwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			setXAxisLabelsRotatedBy(Converter.stringToDouble(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			setDisplayLegend(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_BORDER.equals(name))
			setLegendBorder(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_FONT.equals(name))
			setLegendFont(Converter.stringToAwtfont(value));
		else if (PROP_LEGEND_POSITION.equals(name))
			setLegendPosition(Converter.stringToEnum(value, LegendPosition.class));
		else if (PROP_LEGEND_ANCHORING.equals(name))
			setLegendAnchoring(Converter.stringToEnum(value, LegendAnchor.class));
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
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String value) {
		if (value == null)
			value = DEFAULT_TITLE;

		title = value;
		scheduleRefresh();
	}

	public Font getTitleFont() {
		return titleFont;
	}

	public void setTitleFont(Font font) {
		if (font == null)
			return;

		titleFont = font;
		scheduleRefresh();
	}

	public String getXAxisTitle() {
		return xAxisTitle;
	}

	public void setXAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_X_AXIS_TITLE;

		xAxisTitle = title;
		scheduleRefresh();
	}

	public String getYAxisTitle() {
		return yAxisTitle;
	}

	public void setYAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_Y_AXIS_TITLE;

		yAxisTitle = title;
		scheduleRefresh();
	}

	public Font getAxisTitleFont() {
		return axisTitleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font == null)
			return;

		axisTitleFont = font;
		scheduleRefresh();
	}

	public Font getLabelFont() {
		return tickLabelFont;
	}

	public void setLabelFont(Font font) {
		if (font == null)
			return;

		tickLabelFont = font;
		scheduleRefresh();
	}

	public Double getXAxisLabelsRotatedBy() {
		return xAxisLabelsRotatedBy;
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;

		xAxisLabelsRotatedBy = angle;
		scheduleRefresh();
	}

	public Boolean getDisplayLegend() {
		return displayLegend;
	}

	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;

		displayLegend = value;
		scheduleRefresh();
	}

	public Boolean getLegendBorder() {
		return legendBorder;
	}

	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;

		legendBorder = value;
		scheduleRefresh();
	}

	public Font getLegendFont() {
		return legendFont;
	}

	public void setLegendFont(Font font) {
		if (font == null)
			return;

		legendFont = font;
		scheduleRefresh();
	}

	public LegendPosition getLegendPosition() {
		return legendPosition;
	}

	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;

		legendPosition = value;
		scheduleRefresh();
	}

	public LegendAnchor getLegendAnchoring() {
		return legendAnchor;
	}

	public void setLegendAnchoring(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;

		legendAnchor = value;
		scheduleRefresh();
	}

	public Double getBarBaseline() {
		return barBaseline;
	}

	public void setBarBaseline(Double value) {
		if (value == null)
			value = DEFAULT_BAR_BASELINE;

		barBaseline = value;
		scheduleRefresh();
	}

	public BarPlacement getBarPlacement() {
		return barPlacement;
	}

	public void setBarPlacement(BarPlacement value) {
		if (value == null)
			value = DEFAULT_BAR_PLACEMENT;

		barPlacement = value;
		scheduleRefresh();
	}

	public Double getYMin() {
		return yMin;
	}

	public void setYMin(Double value) {
		yMin = value;
		scheduleRefresh();
	}

	public Double getYMax() {
		return yMax;
	}

	public void setYMax(Double value) {
		yMax = value;
		scheduleRefresh();
	}

	public Boolean getInvertXY() {
		return invertXY;
	}

	public void setInvertXY(Boolean value) {
		if (value == null)
			value = DEFAULT_INVERT_XY;

		invertXY = value;
		scheduleRefresh();
	}

	public Boolean getGridVisible() {
		return gridVisible;
	}

	public void setGridVisible(Boolean value) {
		if (value == null)
			value = DEFAULT_SHOW_GRID;

		gridVisible = value;
		scheduleRefresh();
	}

	private void setYLogarithmic(Boolean value) {
		if (value == null)
			value = DEFAULT_Y_LOGARITHMIC;
		scheduleRefresh();
	}
	
	@Override
	protected void paintCachableLayer(Graphics graphics) {
		Rectangle clip = graphics.getClip(new Rectangle());
		
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
			graphics.setBackgroundColor(getBarColor(row, column));
			graphics.fillRectangle(x+1, y+1, width-2, height-2);
		}
	}
	
	protected Color getBarColor(int row, int column) {
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
		// TODO: draw title, legend, axes
	}
	
	private void scheduleRefresh() {
		
	}
}
