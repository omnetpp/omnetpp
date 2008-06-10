package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.BarProperties.PROP_BAR_COLOR;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_WRAP_LABELS;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_LABELS_ROTATED_BY;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ScalarChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.ScalarChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.properties.ScalarChartProperties.PROP_WRAP_LABELS;

import org.apache.commons.lang.StringEscapeUtils;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.dataset.IAveragedScalarDataset;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IScalarDataset;
import org.omnetpp.scave.charting.dataset.IStringValueScalarDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.properties.ChartProperties.ShowGrid;
import org.omnetpp.scave.charting.properties.ScalarChartProperties.BarPlacement;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.model2.StatUtils;

/**
 * Bar chart.
 *
 * @author tomi
 */
public class ScalarChart extends ChartCanvas {
	private IScalarDataset dataset;

	private LinearAxis valueAxis = new LinearAxis(true, DEFAULT_Y_AXIS_LOGARITHMIC, false);
	private DomainAxis domainAxis = new DomainAxis(this);
	private BarPlot plot;

	private PropertyMap<BarProperties> properties = new PropertyMap<BarProperties>(BarProperties.class);
	static class BarProperties {
		RGB color;
	}
	
	static class BarSelection implements IChartSelection {
		// TODO selection on ScalarCharts
	}
	
	public ScalarChart(Composite parent, int style) {
		super(parent, style);
		plot = new BarPlot(this);
		new Tooltip(this);
		
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
	protected double transformY(double y) {
		return valueAxis.transform(y);
	}

	@Override
	protected double inverseTransformY(double y) {
		return valueAxis.inverseTransform(y);
	}

	@Override
	public void doSetDataset(IDataset dataset) {
		if (dataset != null && !(dataset instanceof IScalarDataset))
			throw new IllegalArgumentException("must be an IScalarDataset");
		
		this.dataset = (IScalarDataset)dataset;
		updateLegends();
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	public IScalarDataset getDataset() {
		return dataset;
	}
	
	public BarPlot getPlot() {
		return plot;
	}

	@Override
	protected RectangularArea calculatePlotArea() {
		return plot.calculatePlotArea();
	}
	
	private void updateLegends() {
		updateLegend(legend);
		updateLegend(legendTooltip);
	}

	private void updateLegend(ILegend legend) {
		legend.clearItems();
		IChartSymbol symbol = new SquareSymbol();
		if (dataset != null) {
			for (int i = 0; i < dataset.getColumnCount(); ++i) {
				legend.addItem(plot.getBarColor(i), dataset.getColumnKey(i), symbol, false);
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
		else if (PROP_WRAP_LABELS.equals(name))
			setWrapLabels(Converter.stringToBoolean(value));
		// Bars	
		else if (PROP_BAR_BASELINE.equals(name))
			setBarBaseline(Converter.stringToDouble(value));
		else if (PROP_BAR_PLACEMENT.equals(name))
			setBarPlacement(Converter.stringToEnum(value, BarPlacement.class));
		else if (name.startsWith(PROP_BAR_COLOR))
			setBarColor(getElementId(name), ColorFactory.asRGB(value));
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
	
	public void setWrapLabels(Boolean value) {
		if (value == null)
			value = DEFAULT_WRAP_LABELS;
		domainAxis.wrapLabels = value;
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
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}

	public void setLogarithmicY(Boolean value) {
		boolean logarithmic = value != null ? value : DEFAULT_Y_AXIS_LOGARITHMIC;
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
		BarProperties barProps = properties.getProperties(key);
		if (barProps == null || barProps.color == null)
			barProps = properties.getDefaultProperties();
		return barProps != null ? barProps.color : null;
	}
	
	public void setBarColor(String key, RGB color) {
		BarProperties barProps = properties.getOrCreateProperties(key);
		barProps.color = color;
		updateLegends();
		chartChanged();
	}
	
	public String getKeyFor(int columnIndex) {
		if (columnIndex >= 0 && columnIndex < dataset.getColumnCount())
			return dataset.getColumnKey(columnIndex);
		else
			return null;
	}
	
	/*=============================================
	 *               Drawing
	 *=============================================*/
	Rectangle mainArea; // containing plots and axes
	Insets axesInsets; // space occupied by axes

	@Override
	protected Rectangle doLayoutChart(GC gc, int pass) {
		ICoordsMapping mapping = getOptimizedCoordinateMapper();

		if (pass == 1) {
			// Calculate space occupied by title and legend and set insets accordingly
			Rectangle area = new Rectangle(getClientArea());
			Rectangle remaining = legendTooltip.layout(gc, area);
			remaining = title.layout(gc, area);
			remaining = legend.layout(gc, remaining, 1);
	
			mainArea = remaining.getCopy();
			axesInsets = domainAxis.layout(gc, mainArea, new Insets(), mapping, pass);
			axesInsets = valueAxis.layout(gc, mainArea, axesInsets, mapping, pass);

			// tentative plotArea calculation (y axis ticks width missing from the picture yet)
			Rectangle plotArea = mainArea.getCopy().crop(axesInsets);
			return plotArea;
		}
		else if (pass == 2) {
			// now the coordinate mapping is set up, so the y axis knows what tick labels
			// will appear, and can calculate the occupied space from the longest tick label.
			valueAxis.layout(gc, mainArea, axesInsets, mapping, pass);
			domainAxis.layout(gc, mainArea, axesInsets, mapping, pass);
			Rectangle remaining = mainArea.getCopy().crop(axesInsets);
			remaining = legend.layout(gc, remaining, pass);
			//FIXME how to handle it when plotArea.height/width comes out negative??
			Rectangle plotArea = plot.layout(gc, remaining);
			return plotArea;
		}
		else
			return null;
	}
	
	@Override
	protected void doPaintCachableLayer(GC gc, ICoordsMapping coordsMapping) {
		gc.fillRectangle(gc.getClipping());
		valueAxis.drawGrid(gc, coordsMapping);
		plot.draw(gc, coordsMapping);
	}
	
	@Override
	protected void doPaintNoncachableLayer(GC gc, ICoordsMapping coordsMapping) {
		paintInsets(gc);
		plot.drawBaseline(gc, coordsMapping);
		title.draw(gc);
		legend.draw(gc);
		valueAxis.drawAxis(gc, coordsMapping);
		domainAxis.draw(gc, coordsMapping);
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

	@Override
	String getHoverHtmlText(int x, int y, SizeConstraint outSizeConstraint) {
		int rowColumn = plot.findRowColumn(fromCanvasX(x), fromCanvasY(y));
		if (rowColumn != -1) {
			int numColumns = dataset.getColumnCount();
			int row = rowColumn / numColumns;
			int column = rowColumn % numColumns;
			String key = (String) dataset.getColumnKey(column);
			String valueStr = null;
			if (dataset instanceof IStringValueScalarDataset) {
				valueStr = ((IStringValueScalarDataset)dataset).getValueAsString(row, column);
			}
			if (valueStr == null) {
				double value = dataset.getValue(row, column);
				double halfInterval = Double.NaN;
				if (dataset instanceof IAveragedScalarDataset) {
					Statistics stat = ((IAveragedScalarDataset)dataset).getStatistics(row, column);
					halfInterval = StatUtils.confidenceInterval(stat, ScalarChart.CONFIDENCE_LEVEL);
				}
				valueStr = formatValue(value, halfInterval);
			}
			String line1 = StringEscapeUtils.escapeHtml(key);
			String line2 = "value: " + valueStr;
			//int maxLength = Math.max(line1.length(), line2.length());
			TextLayout textLayout = new TextLayout(getDisplay());
			textLayout.setText(line1 + "\n" + line2);
			textLayout.setWidth(320);
			org.eclipse.swt.graphics.Rectangle bounds= textLayout.getBounds();
			outSizeConstraint.preferredWidth = 20 + bounds.width;
			outSizeConstraint.preferredHeight = 20 + bounds.height;

//			outSizeConstraint.preferredWidth = 20 + maxLength * 7;
//			outSizeConstraint.preferredHeight = 25 + 2 * 12;
			return line1 + "<br>" + line2;
		}
		return null;
	}
}
