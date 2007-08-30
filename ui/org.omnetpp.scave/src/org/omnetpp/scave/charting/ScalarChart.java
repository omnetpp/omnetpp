package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_BASELINE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_OUTLINE_COLOR;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_COLOR;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.ShowGrid;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;

/**
 * Bar chart.
 *
 * @author tomi
 */
public class ScalarChart extends ChartCanvas {
	private ScalarDataset dataset;

	private LinearAxis valueAxis = new LinearAxis(this, true, DEFAULT_Y_AXIS_LOGARITHMIC);
	private DomainAxis domainAxis = new DomainAxis(this);
	private BarPlot plot = new BarPlot();

	private Map<String,BarProperties> barProperties = new HashMap<String,BarProperties>();
	private static final String KEY_ALL = null;
	
	static class BarProperties {
		RGB color;
	}
	
	static class BarSelection implements IChartSelection {
		// TODO
	}
	
	public ScalarChart(Composite parent, int style) {
		super(parent, style);
		new Tooltip();
		
		this.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				setSelection(new BarSelection());
			}
		});
	}
	
	@Override
	public void dispose() {
		domainAxis.dispose();
		super.dispose();
	}

	@Override
	public void doSetDataset(IDataset dataset) {
		if (dataset != null && !(dataset instanceof ScalarDataset))
			throw new IllegalArgumentException("must be an ScalarDataset");
		
		this.dataset = (ScalarDataset)dataset;
		updateLegend();
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	public ScalarDataset getDataset() {
		return dataset;
	}
	
	public BarPlot getPlot() {
		return plot;
	}

	@Override
	protected RectangularArea calculatePlotArea() {
		return plot.calculatePlotArea();
	}

	private void updateLegend() {
		legend.clearLegendItems();
		legendTooltip.clearItems();
		IChartSymbol symbol = new SquareSymbol();
		if (dataset != null) {
			for (int i = 0; i < dataset.getColumnCount(); ++i) {
				legend.addLegendItem(plot.getBarColor(i), dataset.getColumnKey(i).toString(), symbol, false);
				legendTooltip.addItem(plot.getBarColor(i), dataset.getColumnKey(i).toString(), symbol, false);
			}
		}
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
		else if (name.startsWith(PROP_BAR_COLOR))
			setBarColor(getKeyFrom(name), ColorFactory.asRGB(value));
		// Axes
		else if (PROP_XY_GRID.equals(name))
			setShowGrid(Converter.stringToEnum(value, ShowGrid.class));
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			setLogarithmicY(Converter.stringToBoolean(value));
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
		chartChanged();
	}

	public String getYAxisTitle() {
		return valueAxis.getTitle();
	}

	public void setYAxisTitle(String title) {
		valueAxis.setTitle(title==null ? DEFAULT_Y_AXIS_TITLE : title);
		chartChanged();
	}

	public Font getAxisTitleFont() {
		return domainAxis.titleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font != null) {
			domainAxis.titleFont = font;
			valueAxis.setTitleFont(font);
			chartChanged();
		}
	}

	public void setLabelFont(Font font) {
		if (font == null)
			font = DEFAULT_LABELS_FONT;
		domainAxis.labelsFont = font;
		valueAxis.setTickFont(font);
		chartChanged();
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;
		domainAxis.rotation = Math.max(0, Math.min(90, angle));
		chartChanged();
	}

	public Double getBarBaseline() {
		return plot.barBaseline;
	}

	public void setBarBaseline(Double value) {
		if (value == null)
			value = DEFAULT_BAR_BASELINE;

		plot.barBaseline = value;
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}

	public BarPlacement getBarPlacement() {
		return plot.barPlacement;
	}

	public void setBarPlacement(BarPlacement value) {
		if (value == null)
			value = DEFAULT_BAR_PLACEMENT;

		plot.barPlacement = value;
		chartChanged();
	}

	public void setLogarithmicY(Boolean value) {
		boolean logarithmic = value != null ? value : DEFAULT_Y_AXIS_LOGARITHMIC;
		transform = logarithmic ? new LogarithmicYTransform() : null;
		valueAxis.setLogarithmic(logarithmic);
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}

	public void setShowGrid(ShowGrid value) {
		if (value == null)
			value = DEFAULT_SHOW_GRID;

		valueAxis.setShowGrid(value);
		chartChanged();
	}
	
	public RGB getBarColor(String key) {
		BarProperties barProps = getBarProperties(key);
		if (barProps == null || barProps.color == null)
			barProps = getDefaultBarProperties();
		return barProps != null ? barProps.color : null;
	}
	
	public void setBarColor(String key, RGB color) {
		BarProperties barProps = getOrCreateBarProperties(key);
		barProps.color = color;
		updateLegend();
		chartChanged();
	}
	
	private String getKeyFrom(String propertyKey) {
		int index = propertyKey.indexOf('/');
		return index >= 0 ? propertyKey.substring(index + 1) : KEY_ALL;
	}
	
	public String getKeyFor(int columnIndex) {
		if (columnIndex >= 0 && columnIndex < dataset.getColumnCount())
			return dataset.getColumnKey(columnIndex);
		else
			return null;
	}
	
	public BarProperties getBarProperties(String key) {
		return (key != null ? barProperties.get(key) : null);
	}
	
	public BarProperties getDefaultBarProperties() {
		return barProperties.get(KEY_ALL);
	}
	
	private BarProperties getOrCreateBarProperties(String key) {
		BarProperties barProps = getBarProperties(key);
		if (barProps == null) {
			barProps = new BarProperties();
			barProperties.put(key, barProps);
		}
		return barProps;
	}

	/*=============================================
	 *               Drawing
	 *=============================================*/

	@Override
	protected void doLayoutChart() {
		GC gc = new GC(Display.getCurrent());

		try {
			// preserve zoomed-out state while resizing
			boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
			boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

			// Calculate space occupied by title and legend and set insets accordingly
			Rectangle area = new Rectangle(getClientArea());
			Rectangle remaining = legendTooltip.layout(gc, area);
			remaining = title.layout(gc, area);
			remaining = legend.layout(gc, remaining);

			Rectangle mainArea = remaining.getCopy();
			Insets insetsToMainArea = new Insets();
			domainAxis.layoutHint(gc, mainArea, insetsToMainArea);
			// postpone valueAxis.layoutHint() as it wants to use coordinate mapping which is not yet set up (to calculate ticks)
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
			valueAxis.layoutHint(gc, mainArea, insetsToMainArea);

			// now we have the final insets, set it everywhere again 
			domainAxis.setLayout(mainArea, insetsToMainArea);
			valueAxis.setLayout(mainArea, insetsToMainArea);
			plotArea = mainArea.getCopy().crop(insetsToMainArea);
			legend.layoutSecondPass(plotArea);
			//FIXME how to handle it when plotArea.height/width comes out negative??
			plot.layout(gc, plotArea);
			setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

			if (shouldZoomOutX)
				zoomToFitX();
			if (shouldZoomOutY)
				zoomToFitY();
			validateZoom(); //Note: scrollbar.setVisible() triggers Resize too
		} 
		catch (Exception e) {
			ScavePlugin.logError(e);
		}
		finally {
			gc.dispose();
		}
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		resetDrawingStylesAndColors(gc);
		gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
		
		valueAxis.drawGrid(gc);
		plot.draw(gc);
	}
	
	@Override
	protected void paintNoncachableLayer(GC gc) {
		resetDrawingStylesAndColors(gc);
		gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
		
		paintInsets(gc);
		title.draw(gc);
		legend.draw(gc);
		valueAxis.drawAxis(gc);
		domainAxis.draw(gc);
		drawRubberband(gc);
		legendTooltip.draw(gc);
		drawStatusText(gc);
	}
	
	

	@Override
	public void setZoomX(double zoomX) {
		super.setZoomX(zoomX);
		chartChanged();
	}

	@Override
	public void setZoomY(double zoomY) {
		super.setZoomY(zoomY);
		chartChanged();
	}



	/**
	 * Draws the bars of the bar chart. 
	 */
	class BarPlot {
		private Rectangle rect = new Rectangle(0,0,1,1);
		private int widthBar = 10;
		private int hgapMinor = 5;
		private int hgapMajor = 20;
		private double horizontalInset = 1.0;   // left/right inset relative to the bars' width 
		private double verticalInset = 0.1; // top inset relative to the height of the highest bar
		
		private double barBaseline = DEFAULT_BAR_BASELINE;
		private BarPlacement barPlacement = DEFAULT_BAR_PLACEMENT;
		private Color barOutlineColor = DEFAULT_BAR_OUTLINE_COLOR;
		
		public Rectangle getRectangle() {
			return rect;
		}
		
		public Rectangle layout(GC gc, Rectangle rect) {
			this.rect = rect.getCopy();
			return rect;
		}
		
		public void draw(GC gc) {
			if (dataset != null) {
				resetDrawingStylesAndColors(gc);
				Graphics graphics = new SWTGraphics(gc);
				graphics.pushState();

				Rectangle clip = graphics.getClip(new Rectangle());

				int cColumns = dataset.getColumnCount();
				int[] indices = getRowColumnsInRectangle(clip);
				for (int i = indices[0]; i <= indices[1]; ++i) {
					int row = i / cColumns;
					int column = i % cColumns;
					drawBar(graphics, row, column);
				}
				graphics.popState();
				graphics.dispose();
			}
		}
		
		protected void drawBar(Graphics graphics, int row, int column) {
			Rectangle rect = getBarRectangle(row, column);
			rect.width = Math.max(rect.width, 1);
			rect.height = Math.max(rect.height, 1);
			graphics.setBackgroundColor(getBarColor(column));
			graphics.fillRectangle(rect);
			if (rect.width >= 4 && rect.height >= 3) {
				graphics.setForegroundColor(barOutlineColor);
				graphics.drawRectangle(rect.getCropped(new Insets(0,0,0,0)));
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

		public int findRowColumn(double x, double y) {
			if (dataset == null)
				return -1;
			int cRows = dataset.getRowCount();
			int cColumns = dataset.getColumnCount();
			x -= horizontalInset * widthBar;
			if (x < 0)
				return -1;
			double rowWidth = cColumns * widthBar + (cColumns - 1) * hgapMinor;
			int row = (int) Math.floor(x / (rowWidth+hgapMajor));
			if (row >= cRows)
				return -1;  // x too big
			x -= row * (rowWidth+hgapMajor);
			if (x > rowWidth)
				return -1;  // x falls in a major gap
			int column = (int) Math.floor(x / (widthBar + hgapMinor));
			x -= column * (widthBar+hgapMinor);
			if (x > widthBar)
				return -1;  // x falls in a minor gap
			double value = dataset.getValue(row, column);
			if (Double.isNaN(value) || (value >= barBaseline ? (y < barBaseline || y > value) : (y > barBaseline || y < value)))
				return -1;  // above or below actual bar 
			return row * cColumns + column; 
		}
		
		protected Color getBarColor(int column) {
			RGB color = ScalarChart.this.getBarColor(getKeyFor(column));
			if (color != null)
				return new Color(null, color);
			else
				return ColorFactory.getGoodDarkColor(column);
		}
		
		protected Rectangle getBarRectangle(int row, int column) {
			int x = toCanvasX(getLeftX(row, column));
			int y = toCanvasY(getTopY(row, column));
			int width = toCanvasDistX(getRightX(row,column) - getLeftX(row, column));
			int height = toCanvasDistY(getTopY(row, column) - getBottomY(row, column));
			return new Rectangle(x, y, width, height);
		}
		
		protected RectangularArea calculatePlotArea() {
			if (dataset == null)
				return new RectangularArea(0, 0, 1, 1);
			
			int cRows = dataset.getRowCount();
			int cColumns = dataset.getColumnCount();
			double minX = getLeftX(0, 0);
			double maxX = getRightX(cRows - 1, cColumns - 1);
			double minY = transformBaseline(barBaseline);
			double maxY = transformBaseline(barBaseline);
			for (int row = 0; row < cRows; ++row)
				for (int column = 0; column < cColumns; ++column) {
					double topY = plot.getTopY(row, column);
					double bottomY = plot.getBottomY(row, column);
					if (!Double.isNaN(bottomY))
						minY = Math.min(minY, bottomY);
					if (!Double.isNaN(topY))
						maxY = Math.max(maxY, topY);
				}
			if (minY > maxY) { // no data points
				minY = 0.0;
				maxY = 1.0;
			}
			double height = maxY - minY;
			return new RectangularArea(minX - horizontalInset * widthBar, minY,
									   maxX + horizontalInset * widthBar, maxY + verticalInset * height);
		}
		
		protected double getLeftX(int row, int column) {
			int cColumns = dataset.getColumnCount();
			double rowWidth = cColumns * widthBar + (cColumns - 1) * hgapMinor;
			return horizontalInset * widthBar + row * (rowWidth + hgapMajor) + column * (widthBar + hgapMinor); 
		}
		
		protected double getRightX(int row, int column) {
			return getLeftX(row, column) + widthBar;
		}
		
		protected double getTopY(int row, int column) {
			double value = transformValue(dataset.getValue(row, column));
			double baseline = transformBaseline(barBaseline);
			return (Double.isNaN(value) || value > baseline ?
					value : baseline);
		}
		
		protected double getBottomY(int row, int column) {
			double value = transformValue(dataset.getValue(row, column));
			double baseline = transformBaseline(barBaseline);
			return (Double.isNaN(value) || value < baseline ?
					value : baseline);
		}
		
		protected double transformValue(double y) {
			if (transform != null)
				return transform.transformY(y);
			else
				return y;
		}
		
		protected double transformBaseline(double baseline) {
			baseline = transformValue(baseline);
			return Double.isNaN(baseline) ? 0.0 : baseline;
		}
	}
	
	/**
	 * Bar chart tooltip
	 * @author Andras
	 */
	class Tooltip {
		public Tooltip() {
			HoverSupport hoverSupport = new HoverSupport();
			hoverSupport.setHoverSizeConstaints(600, 400);
			hoverSupport.adapt(ScalarChart.this, new IHoverTextProvider() {
				public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
					return getHoverText(x, y, outSizeConstraint);
				}
			});
		}
		
		private String getHoverText(int x, int y, SizeConstraint outSizeConstraint) {
			int rowColumn = plot.findRowColumn(inverseTransformX(fromCanvasX(x)),
											   inverseTransformY(fromCanvasY(y)));
			if (rowColumn != -1) {
				int numColumns = dataset.getColumnCount();
				int row = rowColumn / numColumns;
				int column = rowColumn % numColumns;
				String key = (String) dataset.getColumnKey(column);

				String line1 = StringEscapeUtils.escapeHtml(key);
				String line2 = "value: " + dataset.getValue(row, column);
				int maxLength = Math.max(line1.length(), line2.length());
				outSizeConstraint.preferredWidth = 20 + maxLength * 7;
				outSizeConstraint.preferredHeight = 25 + 2 * 12;
				return HoverSupport.addHTMLStyleSheet(line1 + "<br>" + line2);
			}
			return null;
		}
	}
}
