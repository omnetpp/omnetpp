/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.apache.commons.math.util.MathUtils.addAndCheck;
import static org.apache.commons.math.util.MathUtils.mulAndCheck;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_AXIS_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_GRID;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_GRID_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LINE_DRAW_STYLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LINE_STYLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LINE_WIDTH;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_SHOW_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_SHOW_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_SHOW_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_SHOW_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_TITLE;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.DatasetUtils;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.ILinePlotter;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.plotter.LinePlotterFactory;
import org.omnetpp.scave.charting.plotter.NoLinePlotter;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.omnetpp.scave.charting.properties.PlotProperty.DrawStyle;
import org.omnetpp.scave.charting.properties.PlotProperty.LineStyle;
import org.omnetpp.scave.charting.properties.PlotProperty.ShowGrid;
import org.omnetpp.scave.charting.properties.PlotProperty.SymbolType;
import org.omnetpp.scave.preferences.ScavePreferenceConstants;
import org.omnetpp.scave.python.XYDataset;


/**
 * Line plot widget.
 */
public class LinePlot extends PlotBase {
    private static final boolean debug = false;

    private static final PlotProperty[] LINEPLOT_PROPERTIES = ArrayUtils.addAll(PLOTBASE_PROPERTIES, new PlotProperty[] {
            PROP_X_AXIS_TITLE,
            PROP_Y_AXIS_TITLE,
            PROP_AXIS_TITLE_FONT,
            PROP_AXIS_LABEL_FONT,
            PROP_X_AXIS_MIN,
            PROP_X_AXIS_MAX,
            PROP_X_AXIS_LOGARITHMIC,
            PROP_Y_AXIS_LOGARITHMIC,
            PROP_X_AXIS_SHOW_TITLE,
            PROP_Y_AXIS_SHOW_TITLE,
            PROP_X_AXIS_SHOW_LABELS,
            PROP_Y_AXIS_SHOW_LABELS,
            PROP_GRID,
            PROP_GRID_COLOR,
            PROP_DISPLAY_LINE,
            PROP_SYMBOL_TYPE,
            PROP_SYMBOL_SIZE,
            PROP_LINE_DRAW_STYLE,
            PROP_LINE_COLOR,
            PROP_LINE_STYLE,
            PROP_LINE_WIDTH,
    });

    private IXYDataset dataset = null;
    private List<LineProperties> lineProperties;

    private LinearAxis xAxis = new LinearAxis(false, false, true);
    private LinearAxis yAxis = new LinearAxis(true, false, true);
    private CrossHair crosshair;
    private Lines lines;

    private Rectangle mainArea; // containing plots and axes
    private Insets axesInsets; // space occupied by axes
    private Rectangle innerPlotArea; // the area inside of the axes and lines

    // plot-wide line properties, individual line may override
    private boolean defaultDisplayLine;
    private SymbolType defaultSymbolType;
    private int defaultSymbolSize;
    private DrawStyle defaultDrawStyle;
    private RGB defaultLineColor;
    private LineStyle defaultLineStyle;
    private float defaultLineWidth;

    /**
     * Class representing the properties of one line of the chart.
     * There is one instance for each series of the dataset + one extra for the default properties.
     * Property getters fallback to the default if the property is not set for the line.
     */
    class LineProperties {
        private int series;
        private String lineId;
        private Boolean displayLine;
        private SymbolType symbolType;
        private Integer symbolSize;
        private DrawStyle drawStyle;
        private RGB lineColor;
        private LineStyle lineStyle;
        private Float lineWidth;

        public LineProperties(String lineId, int series) {
            Assert.isLegal(lineId != null);
            this.lineId = lineId;
            this.series = series;
        }

        public String getLineId() {
            return lineId;
        }

        public boolean getEffectiveDisplayLine() {
            return displayLine != null ? displayLine : defaultDisplayLine;
        }

        public String getLabel() {
            String name = "";
            if (dataset != null && series != -1)
                name = dataset.getSeriesTitle(series);
            return StringUtils.isEmpty(name) ? lineId : name;
        }

        public SymbolType getEffectiveSymbolType() {
            return symbolType == null ? defaultSymbolType : symbolType;
        }

        public int getEffectiveSymbolSize() {
            return symbolSize == null ? defaultSymbolSize : symbolSize;
        }

        public DrawStyle getEffectiveDrawStyle() {
            return drawStyle == null ? defaultDrawStyle : drawStyle;
        }

        public Color getEffectiveLineColor() {
            return new Color(lineColor == null ? defaultLineColor : lineColor);
        }

        public LineStyle getEffectiveLineStyle() {
            return lineStyle == null ? defaultLineStyle : lineStyle;
        }

        public float getEffectiveLineWidth() {
            return lineWidth == null ? defaultLineWidth : lineWidth;
        }

        public ILinePlotter getLinePlotter() {
            if (getEffectiveLineStyle()==LineStyle.None)
                return new NoLinePlotter();
            DrawStyle type = getEffectiveDrawStyle();
            ILinePlotter plotter = LinePlotterFactory.createVectorPlotter(type);
            return plotter;
        }

        public IPlotSymbol getSymbolPlotter() {
            SymbolType type = getEffectiveSymbolType();
            int size = getEffectiveSymbolSize();
            return ChartSymbolFactory.createChartSymbol(type, size);
        }
    }

    // This is just a handle to one point of one series, so we can reference it.
    protected static class DataPoint {
        int series;
        int index;
        // int multiplicity;

        public DataPoint(int series, int index) {
            this.series = series;
            this.index = index;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj instanceof DataPoint) {
                DataPoint other = (DataPoint)obj;
                return series == other.series && index == other.index;
            }
            return false;
        }
    }

    public LinePlot(Composite parent, int style) {
        super(parent, style);
        // important: add the CrossHair to the chart AFTER the ZoomableCanvasMouseSupport added
        crosshair = new CrossHair(this);
        lineProperties = new ArrayList<LineProperties>();
        lines = new Lines(this);
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                List<DataPoint> points = new ArrayList<DataPoint>();
                int count = dataPointsNear(e.x, e.y, 3, points, 1, getOptimizedCoordinateMapper());
                if (count > 0) {
                    DataPoint point = points.get(0);
                    setSelection(new LinePlotSelection(LinePlot.this, point));
                }
                else
                    setSelection(null);
            }
        });

        resetProperties();
    }

    @Override
    public LinePlotSelection getSelection() {
        return (LinePlotSelection)selection;
    }

    public Rectangle getPlotRectangle() {
        return lines != null ? lines.getPlotRectangle() : Rectangle.SINGLETON;
    }

    @Override
    protected double transformX(double x) {
        return xAxis.transform(x);
    }

    @Override
    protected double transformY(double y) {
        return yAxis.transform(y);
    }

    @Override
    protected double inverseTransformX(double x) {
        return xAxis.inverseTransform(x);
    }

    @Override
    protected double inverseTransformY(double y) {
        return yAxis.inverseTransform(y);
    }

    public LineProperties getLineProperties(int series) {
        Assert.isTrue(series >= 0 && dataset != null && series < dataset.getSeriesCount(),
                String.format("Received series=%d, series count=%d", series, dataset!=null?dataset.getSeriesCount():0));
        return lineProperties.get(series); // index 0 is the default
    }

    public LineProperties getLineProperties(String lineId) {
        Assert.isLegal(lineId != null && !lineId.isEmpty());
        for (LineProperties props : lineProperties)
            if (lineId.equals(props.lineId))
                return props;
        return null;
    }

    public LineProperties getOrCreateLineProperties(String lineId) {
        Assert.isLegal(lineId != null && !lineId.isEmpty());
        LineProperties props = getLineProperties(lineId);
        if (props == null) {
            props = new LineProperties(lineId, -1);
            lineProperties.add(props);
        }
        return props;
    }

    public void updateLineProperties() {
        if (debug) Debug.println("updateLineProperties()");

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

        // We have to do this here because NativeChartPlotter can't do it in its reset().
        if (this.dataset != null)
            ((XYDataset)this.dataset).dispose();

        this.dataset = (IXYDataset)dataset;
        this.selection = null;
        updateLineProperties();
        updateLegends();
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    @Override
    protected void updateLegend(ILegend legend) {
        legend.clearItems();
        if (dataset != null) {
            String[] keys = new String[dataset.getSeriesCount()];
            for (int i = 0; i < keys.length; ++i)
                keys[i] = dataset.getSeriesKey(i);
            Arrays.sort(keys, StringUtils.dictionaryComparator);

            for (String key : keys) {
                LineProperties props = getLineProperties(key);
                if (props.getEffectiveDisplayLine()) {
                    String label = props.getLabel();
                    Color color = props.getEffectiveLineColor();
                    IPlotSymbol symbol = props.getSymbolPlotter();
                    boolean drawLine = props.getEffectiveDisplayLine() && props.getEffectiveDrawStyle() != DrawStyle.None && props.getEffectiveLineStyle() != LineStyle.None;
                    legend.addItem(color, label, symbol, drawLine);
                }
            }
        }
    }

    /*==================================
     *          Properties
     *==================================*/

    public PlotProperty[] getProperties() {
        return LINEPLOT_PROPERTIES;
    }

    @Override
    public void setProperty(PlotProperty prop, String value) {
        switch (prop) {
        // Titles
        case PROP_X_AXIS_TITLE: setXAxisTitle(value); break;
        case PROP_Y_AXIS_TITLE: setYAxisTitle(value); break;
        case PROP_AXIS_TITLE_FONT: setAxisTitleFont(Converter.stringToSwtfont(value)); break;
        case PROP_AXIS_LABEL_FONT: setTickLabelFont(Converter.stringToSwtfont(value)); break;
        // Axes
        case PROP_X_AXIS_MIN: setXMin(Converter.stringToDouble(value)); break;
        case PROP_X_AXIS_MAX: setXMax(Converter.stringToDouble(value)); break;
        case PROP_X_AXIS_LOGARITHMIC: setLogarithmicX(Converter.stringToBoolean(value)); break;
        case PROP_Y_AXIS_LOGARITHMIC: setLogarithmicY(Converter.stringToBoolean(value)); break;
        case PROP_GRID: setShowGrid(Converter.stringToEnum(value, ShowGrid.class)); break;
        case PROP_GRID_COLOR: setGridColor(Converter.stringToRGB(value)); break;
        case PROP_X_AXIS_SHOW_TITLE:  setXAxisShowTitle(Converter.stringToBoolean(value)); break;
        case PROP_X_AXIS_SHOW_LABELS:  setXAxisShowLabels(Converter.stringToBoolean(value)); break;
        case PROP_Y_AXIS_SHOW_TITLE:  setYAxisShowTitle(Converter.stringToBoolean(value)); break;
        case PROP_Y_AXIS_SHOW_LABELS:  setYAxisShowLabels(Converter.stringToBoolean(value)); break;
        // Line defaults
        case PROP_DISPLAY_LINE: setDisplayLine(Converter.stringToBoolean(value)); break;
        case PROP_SYMBOL_TYPE: setSymbolType(Converter.stringToEnum(value, SymbolType.class)); break;
        case PROP_SYMBOL_SIZE: setSymbolSize(Converter.stringToInteger(value)); break;
        case PROP_LINE_DRAW_STYLE: setDrawStyle(Converter.stringToEnum(value, DrawStyle.class)); break;
        case PROP_LINE_COLOR: setLineColor(Converter.stringToRGB(value)); break;
        case PROP_LINE_STYLE: setLineStyle(Converter.stringToEnum(value, LineStyle.class)); break;
        case PROP_LINE_WIDTH: setLineWidth(Converter.stringToFloat(value)); break;
        default: super.setProperty(prop, value);
        }
    }

    @Override
    public void setProperty(PlotProperty prop, String key, String value) {
        switch (prop) {
        // Lines
        case PROP_DISPLAY_LINE: setDisplayLine(key, Converter.stringToOptionalBoolean(value)); break;
        case PROP_SYMBOL_TYPE: setSymbolType(key, Converter.stringToOptionalEnum(value, SymbolType.class)); break;
        case PROP_SYMBOL_SIZE: setSymbolSize(key, Converter.stringToOptionalInteger(value)); break;
        case PROP_LINE_DRAW_STYLE: setDrawStyle(key, Converter.stringToOptionalEnum(value, DrawStyle.class)); break;
        case PROP_LINE_COLOR: setLineColor(key, Converter.stringToOptionalRGB(value)); break;
        case PROP_LINE_STYLE: setLineStyle(key, Converter.stringToOptionalEnum(value, LineStyle.class)); break;
        case PROP_LINE_WIDTH: setLineWidth(key, Converter.stringToOptionalFloat(value)); break;
        default: super.setProperty(prop, key, value);
        }
    }

    @Override
    public void clear() {
        super.clear();
        lineProperties.clear();
        setDataset(new XYDataset());
    }

    public void setXAxisTitle(String value) {
        Assert.isNotNull(value);
        xAxis.setTitle(value);
        chartChanged();
    }

    public void setYAxisTitle(String value) {
        Assert.isNotNull(value);
        yAxis.setTitle(value);
        chartChanged();
    }

    public void setAxisTitleFont(Font value) {
        Assert.isNotNull(value);
        xAxis.setTitleFont(value);
        yAxis.setTitleFont(value);
        chartChanged();
    }

    public void setDisplayLine(boolean value) {
        defaultDisplayLine = value;
        updateLegends();
        chartChanged();
    }

    public void setDrawStyle(DrawStyle value) {
        Assert.isNotNull(value);
        defaultDrawStyle = value;
        chartChanged();
    }

    public void setLineColor(RGB value) {
        Assert.isNotNull(value);
        defaultLineColor = value;
        updateLegends();
        chartChanged();
    }

    public void setLineWidth(float value) {
        defaultLineWidth = value;
        chartChanged();
    }

    public void setLineStyle(LineStyle value) {
        Assert.isNotNull(value);
        defaultLineStyle = value;
        chartChanged();
    }

    public void setSymbolType(SymbolType value) {
        Assert.isNotNull(value);
        defaultSymbolType = value;
        updateLegends();
        chartChanged();
    }

    public void setSymbolSize(int value) {
        defaultSymbolSize = value;
        chartChanged();
    }

    public void setDisplayLine(String key, Boolean value) {
        LineProperties props = getOrCreateLineProperties(key);
        props.displayLine = value;
        updateLegends();
        chartChanged();
    }

    public void setDrawStyle(String key, DrawStyle type) {
        LineProperties props = getOrCreateLineProperties(key);
        props.drawStyle = type;
        chartChanged();
    }

    public void setLineColor(String key, RGB color) {
        LineProperties props = getOrCreateLineProperties(key);
        props.lineColor = color;
        updateLegends();
        chartChanged();
    }

    public void setLineWidth(String key, Float lineWidth) {
        LineProperties props = getOrCreateLineProperties(key);
        props.lineWidth = lineWidth;
        chartChanged();
    }

    public void setLineStyle(String key, LineStyle style) {
        LineProperties props = getOrCreateLineProperties(key);
        props.lineStyle = style;
        chartChanged();
    }

    public void setSymbolType(String key, SymbolType symbolType) {
        LineProperties props = getOrCreateLineProperties(key);
        props.symbolType = symbolType;
        updateLegends();
        chartChanged();
    }

    public void setSymbolSize(String key, Integer size) {
        LineProperties props = getOrCreateLineProperties(key);
        props.symbolSize = size;
        chartChanged();
    }

   public void setLogarithmicX(boolean value) {
        xAxis.setLogarithmic(value);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setLogarithmicY(boolean value) {
        yAxis.setLogarithmic(value);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setShowGrid(ShowGrid value) {
        Assert.isNotNull(value);
        xAxis.setShowGrid(value);
        yAxis.setShowGrid(value);
        chartChanged();
    }

    public void setGridColor(RGB color) {
        Assert.isNotNull(color);
        xAxis.setGridColor(color);
        yAxis.setGridColor(color);
        chartChanged();
    }

    public void setXAxisShowTitle(boolean show) {
        xAxis.setDrawTitle(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setXAxisShowLabels(boolean show) {
        xAxis.setDrawTickLabels(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setYAxisShowTitle(boolean show) {
        yAxis.setDrawTitle(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setYAxisShowLabels(boolean show) {
        yAxis.setDrawTickLabels(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setTickLabelFont(Font font) {
        Assert.isNotNull(font);
        xAxis.setTickFont(font);
        yAxis.setTickFont(font);
        chartChanged();
    }

    @Override
    protected RectangularArea calculatePlotArea() {
        return lines.calculatePlotArea();
    }

    public void zoomToFitData() {
        setZoomedArea(lines.calculatePlotArea(Lines.PlotAreaCalculationMode.DATA_WITH_PADDING));
    }

    @Override
    protected Rectangle doLayoutChart(Graphics graphics, int pass) {
        ICoordsMapping coordsMapping = getOptimizedCoordinateMapper();

        if (pass == 1) {
            // Calculate space occupied by title and legend and set insets accordingly
            Rectangle area = new Rectangle(getClientArea());
            Rectangle remaining = legendTooltip.layout(graphics, area);
            remaining = title.layout(graphics, area);
            remaining = legend.layout(graphics, remaining, pass);

            mainArea = remaining.getCopy();
            axesInsets = yAxis.layout(graphics, mainArea, new Insets(), coordsMapping, pass);
            axesInsets = xAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);

            // tentative plotArea calculation (y axis ticks width missing from the picture yet)
            Rectangle plotArea = mainArea.getCopy().shrink(axesInsets);
            return plotArea;
        }
        else if (pass == 2) {
            // now the coordinate mapping is set up, so the y axis knows what tick labels
            // will appear, and can calculate the occupied space from the longest tick label.
            yAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);
            xAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);
            innerPlotArea = lines.layout(graphics, mainArea.getCopy().shrink(axesInsets));
            crosshair.layout(graphics, innerPlotArea);
            legend.layout(graphics, innerPlotArea, pass);
            //FIXME how to handle it when innerPlotArea.height/width comes out negative??
            return innerPlotArea;
        }
        else
            return null;
    }

    @Override
    protected void doPaintCachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
        graphics.fillRectangle(GraphicsUtils.getClip(graphics));
        xAxis.drawGrid(graphics, coordsMapping);
        yAxis.drawGrid(graphics, coordsMapping);

        IPreferenceStore store = ScavePlugin.getDefault().getPreferenceStore();
        int totalTimeLimitMillis = store.getInt(ScavePreferenceConstants.TOTAL_DRAW_TIME_LIMIT_MILLIS);
        int perLineTimeLimitMillis = store.getInt(ScavePreferenceConstants.PER_LINE_DRAW_TIME_LIMIT_MILLIS);

        boolean completed = lines.draw(graphics, coordsMapping, totalTimeLimitMillis, perLineTimeLimitMillis);

        if (!completed) {
            Rectangle clip = GraphicsUtils.getClip(graphics);
            graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText("Drawing operation timed out, lines is incomplete! Change zoom level to refresh.", clip.x+2, clip.y+2);
        }
    }

    @Override
    protected void doPaintNoncachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
        int highlightedItem = legend.getHighlightedItem();

        if (highlightedItem >= 0) {
            graphics.setAlpha(veilAlpha);
            graphics.setBackgroundColor(backgroundColor);
            graphics.fillRectangle(getViewportRectangle());
            graphics.setAlpha(255);
            xAxis.drawGrid(graphics, coordsMapping);
            yAxis.drawGrid(graphics, coordsMapping);

            if (legend.isItemEnabled(highlightedItem)) {
                IPreferenceStore store = ScavePlugin.getDefault().getPreferenceStore();
                int perLineTimeLimitMillis = store.getInt(ScavePreferenceConstants.PER_LINE_DRAW_TIME_LIMIT_MILLIS);
                lines.drawSingle(graphics, coordsMapping, highlightedItem, System.currentTimeMillis(), perLineTimeLimitMillis, perLineTimeLimitMillis);
            }
        }

        graphics.setAlpha(255);
        graphics.setLineWidth(1);

        paintInsets(graphics);
        title.draw(graphics);
        legend.draw(graphics);
        xAxis.drawAxis(graphics, coordsMapping);
        yAxis.drawAxis(graphics, coordsMapping);
        legendTooltip.draw(graphics);
        drawStatusText(graphics);
        if (getSelection() != null)
            getSelection().draw(graphics, coordsMapping);
        drawRubberband(graphics);
        Point crosshairPosition = crosshair.getCrosshairPosition();
        boolean overLegend = legend.isVisible() && legend.getBounds().contains(crosshairPosition);
        if (innerPlotArea.contains(crosshairPosition.x, crosshairPosition.y) && !overLegend)
            crosshair.draw(graphics, coordsMapping);
    }

    @Override
    String getHoverHtmlText(int x, int y) {
        return null;
    }

    @Override
    public void setDisplayAxesDetails(boolean value) {
        xAxis.setDrawTickLabels(value);
        xAxis.setDrawTitle(value);
        yAxis.setDrawTickLabels(value);
        yAxis.setDrawTitle(value);
        chartChanged();
    }

    public int dataPointsNear(int x, int y, int d, List<DataPoint> result, int maxCount, ICoordsMapping coordsMapping) {
        IXYDataset dataset = getDataset();
        if (dataset == null)
            return 0;

        // for each series, collect data points close to (x,y), at most maxCount of them
        int totalFound = 0;
        for (int series : legend.getEnabledItemIndices()) {
            LineProperties props = getLineProperties(series);
            if (!props.getEffectiveDisplayLine())
                continue;
            // find data point nearest to cursor x, using binary search
            int mid = DatasetUtils.findXLowerLimit(dataset, series, inverseTransformX(coordsMapping.fromCanvasX(x)));

            // then search downwards and upwards for data points close to (x,y)
            for (int i = mid; i >= 0; --i) {
                try {
                    double xx = transformX(dataset.getX(series, i));
                    double yy = transformY(dataset.getY(series, i));
                    int dx = Math.abs((int)coordsMapping.toCanvasX(xx) - x);
                    int dy = Math.abs((int)coordsMapping.toCanvasY(yy) - y);

                    if (dx > d)
                        break;

                    if (dy <= d && addAndCheck(mulAndCheck(dx, dx), mulAndCheck(dy, dy)) <= d * d) {
                        totalFound++;
                        if (result.size() < maxCount)  //XXX add at least one point for each series
                            result.add(new DataPoint(series, i));
                    }
                }
                catch (ArithmeticException e) {}
            }

            for (int i = mid + 1; i < dataset.getItemCount(series); ++i) {
                try {
                    double xx = transformX(dataset.getX(series, i));
                    double yy = transformY(dataset.getY(series, i));
                    int dx = Math.abs((int)coordsMapping.toCanvasX(xx) - x);
                    int dy = Math.abs((int)coordsMapping.toCanvasY(yy) - y);

                    if (dx > d)
                        break;

                    if (dy <= d && addAndCheck(mulAndCheck(dx, dx), mulAndCheck(dy, dy)) <= d * d) {
                        totalFound++;
                        if (result.size() < maxCount)
                            result.add(new DataPoint(series, i));
                    }
                }
                catch (ArithmeticException e) {}
            }
        }
        return totalFound;
    }
}
