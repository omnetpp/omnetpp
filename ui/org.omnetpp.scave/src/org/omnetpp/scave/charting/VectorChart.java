package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LINE_STYLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartProperties.PROP_Y_AXIS_TITLE;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset.InterpolationMode;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.VectorPlotterFactory;
import org.omnetpp.scave.charting.properties.ChartProperties.LineType;
import org.omnetpp.scave.charting.properties.ChartProperties.ShowGrid;
import org.omnetpp.scave.charting.properties.ChartProperties.SymbolType;


/**
 * Line chart.
 */
public class VectorChart extends ChartCanvas {
	
	private static final boolean debug = false;
	
	private IXYDataset dataset = null;
	private List<LineProperties> lineProperties;
	private LineProperties defaultProperties;

	private LinearAxis xAxis = new LinearAxis(false, false, true);
	private LinearAxis yAxis = new LinearAxis(true, DEFAULT_Y_AXIS_LOGARITHMIC, true);
	private CrossHair crosshair = new CrossHair(this);
	private LinePlot plot;
	
	/**
	 * Class representing the properties of one line of the chart.
	 * There is one instance for each series of the dataset + one extra for the default properties.
	 * Property getters fallback to the default if the property is not set for the line.
	 */
	class LineProperties {
		LineProperties fallback;
		int series;
		String lineId;
		Boolean displayLine;
		SymbolType symbolType;
		Integer symbolSize;
		LineType lineType;
		RGB lineColor;
		
		/**
		 * Constructor for the defaults.
		 */
		private LineProperties() {
			this.displayLine = DEFAULT_DISPLAY_LINE;
			this.symbolSize = DEFAULT_SYMBOL_SIZE;
			this.lineType = DEFAULT_LINE_STYLE;
			this.series = -1;
			this.lineId = null;
		}
		
		public LineProperties(String lineId, int series) {
			Assert.isLegal(lineId != null);
			this.lineId = lineId;
			this.series = series;
			fallback = defaultProperties;
		}
		
		public String getLineId() {
			return lineId;
		}
		
		public boolean getDisplayLine() {
			if (displayLine == null && fallback != null)
				return fallback.getDisplayLine();
			Assert.isTrue(displayLine != null);
			return displayLine;
		}
		
		public void setDisplayLine(Boolean displayLine) {
			if (displayLine == null && this == defaultProperties)
				displayLine = DEFAULT_DISPLAY_LINE;
			this.displayLine = displayLine;
		}
		
		public SymbolType getSymbolType() {
			SymbolType symbolType = this.symbolType;
			
			if (symbolType == null && fallback != null)
				symbolType = fallback.getSymbolType();
			
			if (symbolType == null) {
				switch (series % 6) {
				case 0: symbolType = SymbolType.Square; break;
				case 1: symbolType = SymbolType.Dot; break;
				case 2: symbolType = SymbolType.Triangle; break;
				case 3: symbolType = SymbolType.Diamond; break;
				case 4: symbolType = SymbolType.Cross; break;
				case 5: symbolType = SymbolType.Plus; break;
				default: symbolType = null; break;
				}
			}
			//Assert.isTrue(symbolType != null);
			return symbolType;
		}
		
		public void setSymbolType(SymbolType symbolType) {
			this.symbolType = symbolType;
		}
		
		public int getSymbolSize() {
			if (symbolSize == null && fallback != null)
				return fallback.getSymbolSize();
			Assert.isTrue(symbolSize != null);
			return symbolSize;
		}
		
		public void setSymbolSize(Integer symbolSize) {
			if (symbolSize == null && this == defaultProperties)
				symbolSize = DEFAULT_SYMBOL_SIZE;
			this.symbolSize = symbolSize;
		}
		
		public LineType getLineType() {
			if (lineType == null && fallback != null && fallback.lineType != null)
				return fallback.lineType;
			//Assert.isTrue(lineType != null);
			return lineType == null ? lineTypeFromInterpolationMode() : lineType;
		}
		
		public void setLineType(LineType lineType) {
			if (lineType == null && this == defaultProperties)
				lineType = DEFAULT_LINE_STYLE;
			this.lineType = lineType;
		}
		
		public RGB getLineColor() {
			if (lineColor == null && fallback != null)
				return fallback.getLineColor();
			return lineColor;
		}
		
		public void setLineColor(RGB lineColor) {
			this.lineColor = lineColor;
		}
		
		public IVectorPlotter getPlotter() {
			Assert.isTrue(this != defaultProperties);
			LineType type = getLineType();
			IVectorPlotter plotter = VectorPlotterFactory.createVectorPlotter(type);
			plotter.setDatasetTransformation(transform);
			return plotter; 
		}
		
		public IChartSymbol getSymbol() {
			Assert.isTrue(this != defaultProperties);
			SymbolType type = getSymbolType();
			int size = getSymbolSize();
			return ChartSymbolFactory.createChartSymbol(type, size);
		}
		
		public Color getColor() {
			Assert.isTrue(this != defaultProperties);
			RGB color = getLineColor();
			if (color != null)
				return ColorFactory.asColor(ColorFactory.asString(color));
			else
				return ColorFactory.getGoodDarkColor(series);
		}
		
		private LineType lineTypeFromInterpolationMode() {
			if (series < 0)
				return LineType.Linear;
			InterpolationMode mode = dataset.getSeriesInterpolationMode(series);
			return getLineTypeForInterpolationMode(mode);
		}
	}
	
	public static LineType getLineTypeForInterpolationMode(InterpolationMode mode) {
		switch (mode) {
		case None: return LineType.Points;
		case Linear: return LineType.Linear;
		case SampleHold: return LineType.SampleHold;
		case BackwardSampleHold: return LineType.BackwardSampleHold;
		}
		return LineType.Linear;
	}
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		lineProperties = new ArrayList<LineProperties>();
		defaultProperties = new LineProperties();
		plot = new LinePlot(this);
		this.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				List<CrossHair.DataPoint> points = new ArrayList<CrossHair.DataPoint>();
				int count = crosshair.dataPointsNear(e.x, e.y, 3, points, 1, getOptimizedCoordinateMapper());
				if (count > 0) {
					CrossHair.DataPoint point = points.get(0);
					setSelection(new VectorChartSelection(VectorChart.this, point));
				}
				else
					setSelection(null);
			}
		});
	}
	
	public VectorChartSelection getSelection() {
		return (VectorChartSelection)selection;
	}
	
	public LineProperties getLineProperties(int series) {
		Assert.isTrue(series >= 0 && dataset != null && series < dataset.getSeriesCount(),
				String.format("Received series=%d, series count=%d", series, dataset!=null?dataset.getSeriesCount():0));
		return lineProperties.get(series); // index 0 is the default
	}
	
	public LineProperties getLineProperties(String lineId) {
		if (lineId == null)
			return defaultProperties;
		for (LineProperties props : lineProperties)
			if (lineId.equals(props.lineId))
				return props;
		return null;
	}
	
	public LineProperties getOrCreateLineProperties(String lineId) {
		LineProperties props = getLineProperties(lineId);
		if (props == null) {
			props = new LineProperties(lineId, -1);
			lineProperties.add(props);
		}
		return props;
	}
	
	public void updateLineProperties() {
		if (debug) System.out.println("updateLineProperties()");
		
		int seriesCount = dataset != null ? dataset.getSeriesCount() : 0;
		List<LineProperties> newProps = new ArrayList<LineProperties>(seriesCount);
		
		// create an index for the current line properties
		Map<String,LineProperties> map = new HashMap<String,LineProperties>();
		for (LineProperties props : lineProperties) {
			if (props.getLineId() != null)
				map.put(props.getLineId(), props);
		}
		// add properties for series in the current dataset
		for (int series = 0; series < seriesCount; ++series) {
			String lineId = dataset.getSeriesKey(series);
			LineProperties oldProps =  map.get(lineId);
			if (oldProps != null) {
				oldProps.series = series;
				newProps.add(oldProps);
				map.remove(lineId);
			}
			else {
				newProps.add(new LineProperties(lineId, series));
			}
		}
		// add properties that was set but not present in the current dataset
		// we need this, because the properties can be set earlier than the dataset
		for (LineProperties props : map.values()) {
			props.series = -1;
			newProps.add(props);
		}
		
		lineProperties = newProps;
	}
	
	public IXYDataset getDataset() {
		return dataset;
	}

	@Override
	public void doSetDataset(IDataset dataset) {
		if (dataset != null && !(dataset instanceof IXYDataset))
			throw new IllegalArgumentException("must be an IXYDataset");
		this.dataset = (IXYDataset)dataset;
		this.selection = null;
		updateLineProperties();
		updateLegends();
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	private void updateLegends() {
		updateLegend(legend);
		updateLegend(legendTooltip);
	}
	
	private void updateLegend(ILegend legend) {
		legend.clearItems();
		if (dataset != null) {
			for (int i = 0; i < dataset.getSeriesCount(); ++i) {
				LineProperties props = getLineProperties(i);
				if (props.getDisplayLine()) {
					Color color = props.getColor();
					String key = props.getLineId();
					IChartSymbol symbol = props.getSymbol();
					legend.addItem(color, key, symbol, true);
				}
			}
		}
	}
	
	/*==================================
	 *          Properties
	 *==================================*/
	public void setProperty(String name, String value) {
		Assert.isLegal(name != null);
		if (debug) System.out.println("VectorChart.setProperty: "+name+"='"+value+"'");
		// Titles
		if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToSwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setTickLabelFont(Converter.stringToSwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			; //TODO PROP_X_LABELS_ROTATE_BY
		// Axes
		else if (PROP_X_AXIS_MIN.equals(name))
			setXMin(Converter.stringToDouble(value));
		else if (PROP_X_AXIS_MAX.equals(name))
			setXMax(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			setLogarithmicY(Converter.stringToBoolean(value));
		else if (PROP_XY_GRID.equals(name))
			setShowGrid(Converter.stringToEnum(value, ShowGrid.class));
		// Lines
		else if (name.startsWith(PROP_DISPLAY_LINE))
			setDisplayLine(getLineId(name), Converter.stringToBoolean(value));
		else if (name.startsWith(PROP_SYMBOL_TYPE))
			setSymbolType(getLineId(name), Converter.stringToEnum(value, SymbolType.class));
		else if (name.startsWith(PROP_SYMBOL_SIZE))
			setSymbolSize(getLineId(name), Converter.stringToInteger(value));
		else if (name.startsWith(PROP_LINE_TYPE))
			setLineStyle(getLineId(name), Converter.stringToEnum(value, LineType.class));
		else if (name.startsWith(PROP_LINE_COLOR))
			setLineColor(getLineId(name), ColorFactory.asRGB(value));
		else
			super.setProperty(name, value);
	}
	
	private String getLineId(String propertyName) {
		int index = propertyName.indexOf('/');
		return index >= 0 ? propertyName.substring(index + 1) : null;
	}

	public void setXAxisTitle(String value) {
		xAxis.setTitle(value != null ? value : DEFAULT_X_AXIS_TITLE);
		chartChanged();
	}
	
	public void setYAxisTitle(String value) {
		yAxis.setTitle(value != null ? value : DEFAULT_Y_AXIS_TITLE);
		chartChanged();
	}
	
	public void setAxisTitleFont(Font value) {
		if (value != null) {
			xAxis.setTitleFont(value);
			yAxis.setTitleFont(value);
			chartChanged();
		}
	}

	public void setDisplayLine(String key, Boolean value) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setDisplayLine(value);
		updateLegends();
		chartChanged();
	}
	
	public void setLineStyle(String key, LineType type) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setLineType(type);
		chartChanged();
	}
	
	public void setLineColor(String key, RGB color) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setLineColor(color);
		updateLegends();
		chartChanged();
	}

	public void setSymbolType(String key, SymbolType symbolType) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setSymbolType(symbolType);
		updateLegends();
		chartChanged();
	}
	
	public void setSymbolSize(String key, Integer size) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setSymbolSize(size);
		chartChanged();
	}
	
	public void setLogarithmicY(Boolean value) {
		boolean logarithmic = value != null ? value : DEFAULT_Y_AXIS_LOGARITHMIC;
		yAxis.setLogarithmic(logarithmic);
		transform = logarithmic ? new LogarithmicYTransform() : null;
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	public void setShowGrid(ShowGrid value) {
		ShowGrid showGrid = value != null ? value : DEFAULT_SHOW_GRID;
		xAxis.setShowGrid(showGrid);
		yAxis.setShowGrid(showGrid);
		chartChanged();
	}
	
	public void setTickLabelFont(Font font) {
		if (font == null)
			font = DEFAULT_LABELS_FONT;
		if (font != null) {
			xAxis.setTickFont(font);
			yAxis.setTickFont(font);
			chartChanged();
		}
	}
	
	protected RectangularArea calculatePlotArea() {
		return plot.calculatePlotArea();
	}
	
	@Override
	protected void doLayoutChart(GC gc) {
		Rectangle area = new Rectangle(getClientArea());

		// preserve zoomed-out state while resizing
		boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
		boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

		// Calculate space occupied by title and legend and set insets accordingly
		ICoordsMapping coordsMapping = getOptimizedCoordinateMapper();
		Rectangle remaining = legendTooltip.layout(gc, area);
		remaining = title.layout(gc, area);
		remaining = legend.layout(gc, remaining);

		Rectangle mainArea = remaining.getCopy();
		Insets insetsToMainArea = new Insets();
		xAxis.layoutHint(gc, mainArea, insetsToMainArea, coordsMapping);
		// postpone yAxis.layoutHint() as it wants to use coordinate mapping which is not yet set up (to calculate ticks)
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
		coordsMapping = getOptimizedCoordinateMapper();
		yAxis.layoutHint(gc, mainArea, insetsToMainArea, coordsMapping);

		// now we have the final insets, set it everywhere again 
		xAxis.setLayout(mainArea, insetsToMainArea);
		yAxis.setLayout(mainArea, insetsToMainArea);
		plotArea = plot.layout(gc, mainArea.getCopy().crop(insetsToMainArea));
		crosshair.layout(gc, plotArea);
		legend.layoutSecondPass(plotArea);
		//FIXME how to handle it when plotArea.height/width comes out negative??
		setViewportRectangle(new org.eclipse.swt.graphics.Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height));

		if (shouldZoomOutX)
			zoomToFitX();
		if (shouldZoomOutY)
			zoomToFitY();
		validateZoom(); //Note: scrollbar.setVisible() triggers Resize too
	}
	
	@Override
	protected void doPaintCachableLayer(GC gc, ICoordsMapping coordsMapping) {
		gc.fillRectangle(gc.getClipping());
		xAxis.drawGrid(gc, coordsMapping);
		yAxis.drawGrid(gc, coordsMapping);
		plot.draw(gc, coordsMapping);
	}
	
	@Override
	protected void doPaintNoncachableLayer(GC gc, ICoordsMapping coordsMapping) {
		paintInsets(gc);
		title.draw(gc);
		legend.draw(gc);
		xAxis.drawAxis(gc, coordsMapping);
		yAxis.drawAxis(gc, coordsMapping);
		legendTooltip.draw(gc);
		drawStatusText(gc);
		if (getSelection() != null)
			getSelection().draw(gc, coordsMapping);
		drawRubberband(gc);
		crosshair.draw(gc, coordsMapping);
	}

	@Override
	String getHoverHtmlText(int x, int y, SizeConstraint outSizeConstraint) {
		return null;
	}
}
