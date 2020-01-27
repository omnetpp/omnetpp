/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LABELS_FONT;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_LINE_STYLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_SHOW_GRID;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.ChartDefaults.DEFAULT_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.LinePlotProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.LinePlotProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.LinePlotProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_DISPLAY_NAME;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.LineVisualProperties.PROP_SYMBOL_TYPE;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
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
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset.InterpolationMode;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.plotter.ILinePlotter;
import org.omnetpp.scave.charting.plotter.LinePlotterFactory;
import org.omnetpp.scave.charting.properties.PlotProperties.ShowGrid;
import org.omnetpp.scave.charting.properties.LinePlotProperties;
import org.omnetpp.scave.charting.properties.LineVisualProperties.LineType;
import org.omnetpp.scave.charting.properties.LineVisualProperties.SymbolType;
import org.omnetpp.scave.preferences.ScavePreferenceConstants;


/**
 * Line plot.
 */
public class LinePlotViewer extends PlotViewerBase {

    private static final boolean debug = false;

    private IXYDataset dataset = null;
    private List<LineProperties> lineProperties;
    private LineProperties defaultProperties;

    private LinearAxis xAxis = new LinearAxis(false, DEFAULT_X_AXIS_LOGARITHMIC, true);
    private LinearAxis yAxis = new LinearAxis(true, DEFAULT_Y_AXIS_LOGARITHMIC, true);
    private CrossHair crosshair;
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
        String displayName;
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

        public String getDisplayName() {
            String format = StringUtils.isEmpty(displayName) ? null : displayName;
            String name = "";
            if (dataset != null && series != -1)
                name = dataset.getSeriesTitle(series, format);
            return StringUtils.isEmpty(name) ? lineId : name;
        }

        public void setDisplayName(String name) {
            this.displayName = name;
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

        public ILinePlotter getPlotter() {
            Assert.isTrue(this != defaultProperties);
            LineType type = getLineType();
            ILinePlotter plotter = LinePlotterFactory.createVectorPlotter(type);
            return plotter;
        }

        public IPlotSymbol getSymbol() {
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
        case None: return LineType.Dots;
        case Linear: return LineType.Linear;
        case SampleHold: return LineType.StepsPost;
        case BackwardSampleHold: return LineType.StepsPre;
        }
        return LineType.Linear;
    }

    public LinePlotViewer(Composite parent, int style) {
        super(parent, style);
        // important: add the CrossHair to the chart AFTER the ZoomableCanvasMouseSupport added
        crosshair = new CrossHair(this);
        lineProperties = new ArrayList<LineProperties>();
        defaultProperties = new LineProperties();
        plot = new LinePlot(this);
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                List<CrossHair.DataPoint> points = new ArrayList<CrossHair.DataPoint>();
                int count = crosshair.dataPointsNear(e.x, e.y, 3, points, 1, getOptimizedCoordinateMapper());
                if (count > 0) {
                    CrossHair.DataPoint point = points.get(0);
                    setSelection(new LinePlotSelection(LinePlotViewer.this, point));
                }
                else
                    setSelection(null);
            }
        });
    }

    @Override
    public LinePlotSelection getSelection() {
        return (LinePlotSelection)selection;
    }

    public Rectangle getPlotRectangle() {
        return plot != null ? plot.getPlotRectangle() : Rectangle.SINGLETON;
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
            String[] keys = new String[dataset.getSeriesCount()];
            for (int i = 0; i < keys.length; ++i)
                keys[i] = dataset.getSeriesKey(i);
            Arrays.sort(keys, StringUtils.dictionaryComparator);

            for (String key : keys) {
                LineProperties props = getLineProperties(key);
                if (props.getDisplayLine()) {
                    String name = props.getDisplayName();
                    Color color = props.getColor();
                    IPlotSymbol symbol = props.getSymbol();
                    legend.addItem(color, name, symbol, true);
                }
            }
        }
    }

    /*==================================
     *          Properties
     *==================================*/
    @Override
    public void setProperty(String name, String value) {
        Assert.isLegal(name != null);
        if (debug) Debug.println("VectorChart.setProperty: "+name+"='"+value+"'");
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
        else if (PROP_X_AXIS_LOGARITHMIC.equals(name))
            setLogarithmicX(Converter.stringToBoolean(value));
        else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
            setLogarithmicY(Converter.stringToBoolean(value));
        else if (PROP_XY_GRID.equals(name))
            setShowGrid(Converter.stringToEnum(value, ShowGrid.class));
        // Lines
        else if (name.startsWith(PROP_DISPLAY_LINE))
            setDisplayLine(getElementId(name), Converter.stringToBoolean(value));
        else if (name.startsWith(PROP_DISPLAY_NAME))
            setDisplayName(getElementId(name), value);
        else if (name.startsWith(PROP_SYMBOL_TYPE))
            setSymbolType(getElementId(name), Converter.stringToEnum(value, SymbolType.class));
        else if (name.startsWith(PROP_SYMBOL_SIZE))
            setSymbolSize(getElementId(name), Converter.stringToInteger(value));
        else if (name.startsWith(PROP_LINE_TYPE))
            setLineStyle(getElementId(name), Converter.stringToEnum(value, LineType.class));
        else if (name.startsWith(PROP_LINE_COLOR))
            setLineColor(getElementId(name), ColorFactory.asRGB(value));
        else
            super.setProperty(name, value);
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

    public void setDisplayName(String key, String name) {
        LineProperties props = getOrCreateLineProperties(key);
        props.setDisplayName(name);
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

   public void setLogarithmicX(Boolean value) {
        boolean logarithmic = value != null ? value : DEFAULT_X_AXIS_LOGARITHMIC;
        xAxis.setLogarithmic(logarithmic);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setLogarithmicY(Boolean value) {
        boolean logarithmic = value != null ? value : DEFAULT_Y_AXIS_LOGARITHMIC;
        yAxis.setLogarithmic(logarithmic);
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

    @Override
    protected RectangularArea calculatePlotArea() {
        return plot.calculatePlotArea();
    }

    Rectangle mainArea; // containing plots and axes
    Insets axesInsets; // space occupied by axes

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
            axesInsets = xAxis.layout(graphics, mainArea, new Insets(), coordsMapping, pass);
            axesInsets = yAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);

            // tentative plotArea calculation (y axis ticks width missing from the picture yet)
            Rectangle plotArea = mainArea.getCopy().crop(axesInsets);
            return plotArea;
        }
        else if (pass == 2) {
            // now the coordinate mapping is set up, so the y axis knows what tick labels
            // will appear, and can calculate the occupied space from the longest tick label.
            yAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);
            xAxis.layout(graphics, mainArea, axesInsets, coordsMapping, pass);
            Rectangle plotArea = plot.layout(graphics, mainArea.getCopy().crop(axesInsets));
            crosshair.layout(graphics, plotArea);
            legend.layout(graphics, plotArea, pass);
            //FIXME how to handle it when plotArea.height/width comes out negative??
            return plotArea;
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

        boolean completed = plot.draw(graphics, coordsMapping, totalTimeLimitMillis, perLineTimeLimitMillis);

        if (!completed) {
            Rectangle clip = GraphicsUtils.getClip(graphics);
            graphics.setForegroundColor(ColorFactory.BLACK);
            graphics.drawText("Drawing operation timed out, plot is incomplete! Change zoom level to refresh.", clip.x+2, clip.y+2);
        }
    }

    @Override
    protected void doPaintNoncachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
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
        crosshair.draw(graphics, coordsMapping);
    }

    @Override
    String getHoverHtmlText(int x, int y) {
        return null;
    }

    @Override
    public void setDisplayAxesDetails(Boolean value) {
        xAxis.setDrawTickLabels(value);
        xAxis.setDrawTitle(value);
        yAxis.setDrawTickLabels(value);
        yAxis.setDrawTitle(value);
        chartChanged();
    }
}
