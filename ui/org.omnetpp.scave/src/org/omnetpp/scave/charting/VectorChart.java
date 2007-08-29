package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_DISPLAY_LINE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LINE_STYLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_TITLE;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties.LineType;
import org.omnetpp.scave.charting.ChartProperties.ShowGrid;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.VectorPlotterFactory;


/**
 * Line chart.
 */
public class VectorChart extends ChartCanvas {
	
	private IXYDataset dataset = null;

	private LinearAxis xAxis = new LinearAxis(this, false, false);
	private LinearAxis yAxis = new LinearAxis(this, true, DEFAULT_Y_AXIS_LOGARITHMIC);
	private List<LineProperties> lineProperties;
	private LineProperties defaultProperties;
	private CrossHair crosshair = new CrossHair(this);
	
	private boolean smartMode = true; // whether smartModeLimit is enabled
	private int smartModeLimit = 10000; // turn off symbols if there're more than this amount of points on the plot
	
	public class LineSelection implements IChartSelection {
		private long vectorID;
		private int series;
		private String seriesKey;
		private int index;
		
		public LineSelection(int series, int index) {
			this.series = series;
			this.index=  index;
			this.vectorID = -1;
			this.seriesKey = null;
			if (series >= 0) {
				if (dataset instanceof VectorDataset)
					vectorID = ((VectorDataset)dataset).getID(series);
				seriesKey = dataset.getSeriesKey(series);
			}
		}
		
		public long getSelectedID() {
			return vectorID;
		}
		
		public String getSelectedKey() {
			return seriesKey;
		}
	}
	
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
			if (lineType == null && fallback != null)
				return fallback.getLineType();
			Assert.isTrue(lineType != null);
			return lineType;
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
			IVectorPlotter plotter = VectorPlotterFactory.createVectorPlotter(type);  // XXX cache
			plotter.setDatasetTransformation(transform);
			return plotter; 
		}
		
		public IChartSymbol getSymbol() {
			Assert.isTrue(this != defaultProperties);
			SymbolType type = getSymbolType();
			int size = getSymbolSize();
			return ChartSymbolFactory.createChartSymbol(type, size);  // XXX cache
		}
		
		public Color getColor() { // XXX cache
			Assert.isTrue(this != defaultProperties);
			RGB color = getLineColor();
			if (color != null)
				return new Color(null, color);
			else
				return ColorFactory.getGoodDarkColor(series);
		}
	}
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		lineProperties = new ArrayList<LineProperties>();
		defaultProperties = new LineProperties();
		this.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				List<CrossHair.DataPoint> points = new ArrayList<CrossHair.DataPoint>();
				int count = crosshair.dataPointsNear(e.x, e.y, 3, points, 1);
				if (count > 0) {
					CrossHair.DataPoint point = points.get(0);
					setSelection(new LineSelection(point.series, point.index));
				}
				else
					setSelection(null);
			}
		});
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
		System.out.println("updateLineProperties()");
		
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
		updateLegend();
		chartArea = calculatePlotArea();
		updateArea();
		chartChanged();
	}
	
	private void updateLegend() {
		legend.clearLegendItems();
		legendTooltip.clearItems();
		if (dataset != null) {
			for (int i = 0; i < dataset.getSeriesCount(); ++i) {
				LineProperties props = getLineProperties(i);
				if (props.getDisplayLine()) {
					Color color = props.getColor();
					String key = props.getLineId();
					IChartSymbol symbol = props.getSymbol();
					legend.addLegendItem(color, key, symbol, true);
					legendTooltip.addItem(color, key, symbol, true);
				}
			}
		}
	}
	
	/*==================================
	 *          Properties
	 *==================================*/
	public void setProperty(String name, String value) {
		Assert.isLegal(name != null);
		System.out.println("VectorChart.setProperty: "+name+"='"+value+"'");
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
			; //TODO
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
		updateLegend();
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
		updateLegend();
		chartChanged();
	}

	public void setSymbolType(String key, SymbolType symbolType) {
		LineProperties props = getOrCreateLineProperties(key);
		props.setSymbolType(symbolType);
		updateLegend();
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
		if (font != null) {
			xAxis.setTickFont(font);
			yAxis.setTickFont(font);
		}
		chartChanged();
	}
	
	protected RectangularArea calculatePlotArea() {
		double minX = Double.POSITIVE_INFINITY;
		double minY = Double.POSITIVE_INFINITY;
		double maxX = Double.NEGATIVE_INFINITY;
		double maxY = Double.NEGATIVE_INFINITY;

		if (dataset!=null && dataset.getSeriesCount() > 0) {
			// calculate bounding box
			long startTime = System.currentTimeMillis();
			for (int series = 0; series < dataset.getSeriesCount(); series++) {
				int n = dataset.getItemCount(series);
				if (n > 0) {
					// X must be increasing
					minX = Math.min(minX, transformX(dataset.getX(series, 0)));
					maxX = Math.max(maxX, transformX(dataset.getX(series, n-1)));
					for (int i = 0; i < n; i++) {
						double y = transformY(dataset.getY(series, i));
						if (!Double.isNaN(y)) {
							minY = Math.min(minY, y);
							maxY = Math.max(maxY, y);
						}
					}
				}
			}
			long duration = System.currentTimeMillis() - startTime;
			System.out.println("calculateArea(): "+duration+" ms");
		}
		
		if (minX > maxX) {
			minX = 0.0;
			maxX = 1.0;
		}
		if (minY > maxY) {
			minY = 0.0;
			maxY = 1.0;
		}
		
        double width = maxX - minX;
        double height = maxY - minY;
        
        minX = (minX>=0 ? 0 : minX-width/80);
		maxX = (maxX<=0 ? 0 : maxX+width/80);
		minY = (minY>=0 ? 0 : minY-height/3);
		maxY = (maxY<=0 ? 0 : maxY+height/3);
		
		return new RectangularArea(minX, minY, maxX, maxY);
	}
	
	@Override
	protected void doLayoutChart() {
		GC gc = new GC(Display.getCurrent());
		try {
			// preserve zoomed-out state while resizing
			boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
			boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

			// Calculate space occupied by title and legend and set insets accordingly
			Rectangle area = new Rectangle(getClientArea());
			if (area.isEmpty())
				return;
			
			Rectangle remaining = legendTooltip.layout(gc, area);
			remaining = title.layout(gc, area);
			remaining = legend.layout(gc, remaining);

			Rectangle mainArea = remaining.getCopy();
			Insets insetsToMainArea = new Insets();
			xAxis.layoutHint(gc, mainArea, insetsToMainArea);
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
			yAxis.layoutHint(gc, mainArea, insetsToMainArea);

			// now we have the final insets, set it everywhere again 
			xAxis.setLayout(mainArea, insetsToMainArea);
			yAxis.setLayout(mainArea, insetsToMainArea);
			plotArea = mainArea.getCopy().crop(insetsToMainArea);
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
		catch (Throwable e) {
			ScavePlugin.logError(e);
		}
		finally {
			gc.dispose();
		}
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		System.out.println("paintCachableLayer()");
//		System.out.println(String.format("area=%f, %f, %f, %f, zoom: %f, %f",
//				getMinX(), getMaxX(), getMinY(), getMaxY(), getZoomX(), getZoomY()));
//		System.out.println(String.format("view port=%s, vxy=%d, %d",
//				getViewportRectangle(), getViewportLeft(), getViewportTop()));
		
		if (getClientArea().isEmpty())
			return;
		
		
		resetDrawingStylesAndColors(gc);
		xAxis.drawGrid(gc);
		yAxis.drawGrid(gc);

		if (dataset != null) {
			ICoordsMapping mapper = getOptimizedCoordinateMapper();
			long startTime = System.currentTimeMillis();
			for (int series=0; series<dataset.getSeriesCount(); series++) {
				LineProperties props = getLineProperties(series);
				if (props.getDisplayLine()) {

					IVectorPlotter plotter = props.getPlotter();
					IChartSymbol symbol = props.getSymbol();
					Color color = props.getColor();
					resetDrawingStylesAndColors(gc);
					gc.setAntialias(antialias ? SWT.ON : SWT.OFF);
					gc.setForeground(color);
					gc.setBackground(color);

					if (smartMode && plotter.getNumPointsInXRange(dataset, series, gc, mapper) >= smartModeLimit) {
						//XXX this may have unwanted effects when caching is on,
						// i.e. parts of a line w/ symbols, other parts the SAME line w/o symbols....
						System.out.println("\"smart mode\": turning off symbols");
						symbol = null;
					}

					plotter.plot(dataset, series, gc, mapper, symbol);

					// if drawing is taking too long, display busy cursor
					if (System.currentTimeMillis() - startTime > 1000) {
						Cursor cursor = Display.getCurrent().getSystemCursor(SWT.CURSOR_WAIT);
						getShell().setCursor(cursor);
						setCursor(null); // crosshair cursor would override shell's busy cursor 
					}
				}
			}
			getShell().setCursor(null);
			System.out.println("plotting: "+(System.currentTimeMillis()-startTime)+" ms");

			if (mapper.getNumCoordinateOverflows()>0) {
				resetDrawingStylesAndColors(gc);
				gc.drawText("There were coordinate overflows during plotting, and the resulting chart\n"+
						    "may not be accurate. Please decrease zoom level.", 
						    getViewportRectangle().x+10, getViewportRectangle().y+10, true);
			}
		}
	}
	
	@Override
	protected void paintNoncachableLayer(GC gc) {
		System.out.println("paintNoncachableLayer()");
		if (getClientArea().isEmpty())
			return;
		
		resetDrawingStylesAndColors(gc);
		gc.setAntialias(antialias ? SWT.ON : SWT.OFF);

		paintInsets(gc);
		title.draw(gc);
		legend.draw(gc);
		xAxis.drawAxis(gc);
		yAxis.drawAxis(gc);
		legendTooltip.draw(gc);
		drawStatusText(gc);
		drawSelection(gc);
		drawRubberband(gc);
		crosshair.draw(gc);
	}
	
	protected void drawSelection(GC gc) {
		if (selection instanceof LineSelection) {
			LineSelection selection = (LineSelection)this.selection;
			if (selection.series >= 0 && selection.index >= 0 && dataset != null && selection.series < dataset.getSeriesCount()) {
				LineProperties props = getLineProperties(selection.series);
				if (props != null && props.getDisplayLine()) {
					ICoordsMapping mapper = getOptimizedCoordinateMapper();
					int x = mapper.toCanvasX(transformX(dataset.getX(selection.series, selection.index)));
					int y = mapper.toCanvasY(transformY(dataset.getY(selection.series, selection.index)));
					gc.setForeground(ColorFactory.RED);
					gc.setLineWidth(1);
					gc.drawOval(x-5, y-5, 10, 10);
				}
			}
		}
	}
}
