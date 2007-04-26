package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_LINE_STYLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave.charting.ChartProperties.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties.LineStyle;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.CrossSymbol;
import org.omnetpp.scave.charting.plotter.DiamondSymbol;
import org.omnetpp.scave.charting.plotter.DotsVectorPlotter;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.LinesVectorPlotter;
import org.omnetpp.scave.charting.plotter.OvalSymbol;
import org.omnetpp.scave.charting.plotter.PinsVectorPlotter;
import org.omnetpp.scave.charting.plotter.PlusSymbol;
import org.omnetpp.scave.charting.plotter.PointsVectorPlotter;
import org.omnetpp.scave.charting.plotter.SampleHoldVectorPlotter;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.plotter.TriangleSymbol;


/**
 * Line chart.
 */
public class VectorChart extends ChartCanvas {
	
	private IXYDataset dataset = null;

	private LinearAxis xAxis = new LinearAxis(this, false);
	private LinearAxis yAxis = new LinearAxis(this, true);
	private Map<String, LineProperties> lineProperties = new HashMap<String,LineProperties>();
	private CrossHair crosshair = new CrossHair(this);

	private boolean smartMode = true; // whether smartModeLimit is enabled
	private int smartModeLimit = 10000; // turn off symbols if there're more than this amount of points on the plot
	
	private int layoutDepth = 0; // how many layoutChart() calls are on the stack

	private static final String KEY_ALL = null;
	
	static class LineProperties {
		SymbolType symbolType;
		Integer symbolSize;
		LineStyle lineStyle;
		Color lineColor;
	}
	
	public VectorChart(Composite parent, int style) {
		super(parent, style);
		lineProperties.put(KEY_ALL, new LineProperties());
	}
	
	public IXYDataset getDataset() {
		return dataset;
	}

	@Override
	public void setDataset(IDataset dataset) {
		if (dataset != null && !(dataset instanceof IXYDataset))
			throw new IllegalArgumentException("must be an IXYDataset");

		this.dataset = (IXYDataset)dataset;
		updateLegend();
		calculateArea();
		chartChanged();
	}
	
	private void updateLegend() {
		legend.clearLegendItems();
		if (dataset != null) {
			for (int i = 0; i < dataset.getSeriesCount(); ++i) {
				legend.addLegendItem(getLineColor(i), dataset.getSeriesKey(i).toString());
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
		else if (PROP_Y_AXIS_MIN.equals(name))
			setYMin(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MAX.equals(name))
			setYMax(Converter.stringToDouble(value));
		else if (PROP_X_AXIS_LOGARITHMIC.equals(name))
			; //TODO
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			; //TODO
		else if (PROP_XY_INVERT.equals(name))
			; //TODO
		else if (PROP_XY_GRID.equals(name))
			setGridVisibility(Converter.stringToBoolean(value));
		// Lines
		else if (name.startsWith(PROP_SYMBOL_TYPE))
			setSymbolType(getKey(name), Converter.stringToEnum(value, SymbolType.class));
		else if (name.startsWith(PROP_SYMBOL_SIZE))
			setSymbolSize(getKey(name), Converter.stringToInteger(value));
		else if (name.startsWith(PROP_LINE_TYPE))
			setLineStyle(getKey(name), Converter.stringToEnum(value, LineStyle.class));
		else
			super.setProperty(name, value);
	}
	
	private String getKey(String name) {
		int index = name.indexOf('/');
		return index >= 0 ? name.substring(index + 1) : KEY_ALL;
	}

	protected LineProperties getLineProperties(String key) {
		return lineProperties.get(key);
	}
	
	protected LineProperties getDefaultLineProperties() {
		return lineProperties.get(KEY_ALL);
	}
	
	protected LineProperties getOrCreateLineProperties(String key) {
		LineProperties properties = lineProperties.get(key);
		if (properties == null)
			lineProperties.put(key, properties = new LineProperties());
		return properties;
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

	public LineStyle getLineStyle (String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.lineStyle == null)
			props = getDefaultLineProperties();
		return props.lineStyle != null ? props.lineStyle : DEFAULT_LINE_STYLE;
	}

	public void setLineStyle(String key, LineStyle type) {
		LineProperties props = getOrCreateLineProperties(key);
		props.lineStyle = type;
		chartChanged();
	}

	public SymbolType getSymbolType(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolType == null)
			props = getDefaultLineProperties();
		if (props.symbolType != null) {
			return props.symbolType;
		}
		else {
			// generate one
			int series = getSeriesIndex(key);
			Assert.isTrue(series >= 0); // key must be valid
			switch (series % 6) {
			case 0: return SymbolType.Square;
			case 1: return SymbolType.Oval;
			case 2: return SymbolType.Triangle;
			case 3: return SymbolType.Diamond;
			case 4: return SymbolType.Cross;
			case 5: return SymbolType.Plus;
			default: return null; // never reached  
			}
		}
	}
	
	private int getSeriesIndex(String key) {
		int c = dataset.getSeriesCount();
		for (int i = 0; i < c; ++i)
			if (dataset.getSeriesKey(i).equals(key))
				return i;
		return -1;
	}
	
	public void setSymbolType(String key, SymbolType symbolType) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolType = symbolType;
		chartChanged();
	}
	
	public int getSymbolSize(String key) {
		LineProperties props = getLineProperties(key);
		if (props == null || props.symbolSize == null)
			props = getDefaultLineProperties();
		return props.symbolSize != null ? props.symbolSize : DEFAULT_SYMBOL_SIZE;
	}
	
	public void setSymbolSize(String key, Integer size) {
		LineProperties props = getOrCreateLineProperties(key);
		props.symbolSize = size;
		chartChanged();
	}
	
	public void setXMin(Double value) {
		if (value != null)
			setArea(value, getMinY(), getMaxX(), getMaxY());
		chartChanged();
	}
	
	public void setXMax(Double value) {
		if (value != null)
			setArea(getMinX(), getMinY(), value, getMaxY());
		chartChanged();
	}
	
	public void setYMin(Double value) {
		if (value != null)
			setArea(getMinX(), value, getMaxX(), getMaxY());
		chartChanged();
	}
	
	public void setYMax(Double value) {
		if (value != null)
			setArea(getMinX(), getMinY(), getMaxX(), value);
		chartChanged();
	}
	
	public void setGridVisibility(Boolean value) {
		boolean b = value != null ? value : DEFAULT_SHOW_GRID;
		xAxis.setGridVisible(b);
		yAxis.setGridVisible(b);
		chartChanged();
	}
	
	public void setTickLabelFont(Font font) {
		if (font != null) {
			xAxis.setTickFont(font);
			yAxis.setTickFont(font);
		}
		chartChanged();
	}
	
	private void calculateArea() {
		if (dataset==null || dataset.getSeriesCount()==0)
			return;
	
		// calculate bounding box
		long startTime = System.currentTimeMillis();
		double minX = Double.MAX_VALUE;
		double minY = Double.MAX_VALUE;
		double maxX = Double.MIN_VALUE;
		double maxY = Double.MIN_VALUE;
		for (int series = 0; series < dataset.getSeriesCount(); series++) {
			int n = dataset.getItemCount(series);
			if (n > 0) {
				// X must be increasing
				minX = Math.min(minX, dataset.getX(series, 0));
				maxX = Math.max(maxX, dataset.getX(series, n-1));
				for (int i = 0; i < n; i++) {
					double y = dataset.getY(series, i);
					if (!Double.isNaN(y)) {
						minY = Math.min(minY, y);
						maxY = Math.max(maxY, y);
					}
				}
			}
		}
        double width = maxX - minX;
        double height = maxY - minY;
		long duration = System.currentTimeMillis() - startTime;
		System.out.println("calculateArea(): "+duration+" ms");
        
        // set the chart area, leaving some room around the data lines
        setArea((minX>=0 ? 0 : minX-width/80), (minY>=0 ? 0 : minY-height/3), 
        		(maxX<=0 ? 0 : maxX+width/80), (maxY<=0 ? 0 : maxY+height/3));
	}

	@Override
	protected void layoutChart() {
		// prevent nasty infinite layout recursions
		if (layoutDepth>0)
			return; 
		
		layoutDepth++;
		GC gc = new GC(Display.getCurrent());
		System.out.println("layoutChart(), level "+layoutDepth);

		try {
			// preserve zoomed-out state while resizing
			boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
			boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

			// Calculate space occupied by title and legend and set insets accordingly
			Rectangle area = new Rectangle(getClientArea());
			Rectangle remaining = title.layout(gc, area);
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
			layoutDepth--;
		}
	}
	
	@Override
	protected void paintCachableLayer(GC gc) {
		resetDrawingStylesAndColors(gc);
		xAxis.drawGrid(gc);
		yAxis.drawGrid(gc);

		if (dataset != null) {
			ICoordsMapping mapper = getOptimizedCoordinateMapper();
			long startTime = System.currentTimeMillis();
			for (int series=0; series<dataset.getSeriesCount(); series++) {
				String key = dataset.getSeriesKey(series).toString();
				IVectorPlotter plotter = getPlotter(key);
				IChartSymbol symbol = getSymbol(key);
				Color color = getLineColor(series);
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
	
	public IVectorPlotter getPlotter(String key) {
		LineStyle style = getLineStyle(key);
		switch (style) {
		case None: return new DotsVectorPlotter();
		case Linear: return new LinesVectorPlotter();
		case SampleHold: return new SampleHoldVectorPlotter();
		case Pins: return new PinsVectorPlotter();
		case Points: return new PointsVectorPlotter();
		default: throw new IllegalArgumentException("unknown line style: " + style);
		}
	}
	
	public IChartSymbol getSymbol(String key) {
		SymbolType type = getSymbolType(key);
		int size = getSymbolSize(key);
		switch (type) {
		case None: return null;
		case Cross: return new CrossSymbol(size);
		case Diamond: return new DiamondSymbol(size);
		case Oval: return new OvalSymbol(size);
		case Plus: return new PlusSymbol(size);
		case Square: return new SquareSymbol(size);
		case Triangle: return new TriangleSymbol(size);
		default: throw new IllegalArgumentException("unknown symbol type: " + type);
		}
	}
	
	public Color getLineColor(int series) {
		return ColorFactory.getGoodDarkColor(series);
	}

	@Override
	protected void paintNoncachableLayer(GC gc) {
		resetDrawingStylesAndColors(gc);
		gc.setAntialias(antialias ? SWT.ON : SWT.OFF);

		paintInsets(gc);
		title.draw(gc);
		legend.draw(gc);
		xAxis.drawAxis(gc);
		yAxis.drawAxis(gc);
		drawStatusText(gc);
		crosshair.draw(gc);
	}
}
