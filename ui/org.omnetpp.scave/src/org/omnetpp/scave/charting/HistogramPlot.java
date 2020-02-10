/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_HIST_BAR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_HIST_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_HIST_CUMULATIVE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_HIST_DENSITY;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_SHOW_OVERFLOW_CELL;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_XY_GRID;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.omnetpp.scave.charting.properties.PlotProperty.HistogramBar;
import org.omnetpp.scave.python.PythonHistogramDataset;

/**
 * Histogram plot widget.
 */
public class HistogramPlot extends PlotBase {
    private static final boolean debug = false;

    private static final PlotProperty[] HISTOGRAMPLOT_PROPERTIES = ArrayUtils.addAll(PLOTBASE_PROPERTIES, new PlotProperty[] {
            PROP_X_AXIS_TITLE,
            PROP_Y_AXIS_TITLE,
            PROP_XY_GRID,
            PROP_AXIS_TITLE_FONT,
            PROP_LABEL_FONT,
            PROP_HIST_BAR,
            PROP_SHOW_OVERFLOW_CELL,
            PROP_BAR_BASELINE,
            PROP_HIST_COLOR,
            PROP_HIST_CUMULATIVE,
            PROP_HIST_DENSITY,
            PROP_Y_AXIS_LOGARITHMIC,
    });

    private IHistogramDataset dataset = IHistogramDataset.EMPTY;
    private LinearAxis xAxis = new LinearAxis(false, false, false);
    private LinearAxis yAxis = new LinearAxis(true, false, false);
    private Histograms histograms;

    private RGB defaultHistogramColor; // note: null is allowed and means "auto"
    private boolean defaultHistogramCumulative;
    private boolean defaultHistogramDensity;
    private PlotProperty.HistogramBar defaultBarType;

    class HistogramProperties {
        Boolean cumulative;
        Boolean density;
        RGB color;
        HistogramBar drawStyle;

        public boolean getEffectiveCumulative() {
            return cumulative != null ? cumulative : defaultHistogramCumulative;
        }

        public boolean getEffectiveDensity() {
            return density != null ? density : defaultHistogramDensity;
        }

        public Color getEffectiveColor() {
            return new Color(Display.getCurrent(), color != null ? color : defaultHistogramColor);
        }
    }

    private Map<String,HistogramProperties> histogramProperties = new HashMap<>();

    public HistogramPlot(Composite parent, int style) {
        super(parent, style);
        histograms = new Histograms(this);
        new Tooltip(this);

        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                setSelection(new IPlotSelection() {
                });
            }
        });

        resetProperties();
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

    /*=============================================
     *               Properties
     *=============================================*/

    public PlotProperty[] getProperties() {
        return HISTOGRAMPLOT_PROPERTIES;
    }

    @Override
    public void setProperty(PlotProperty prop, String value) {
        switch (prop) {
        case PROP_X_AXIS_TITLE: setXAxisTitle(value); break;
        case PROP_Y_AXIS_TITLE: setYAxisTitle(value); break;
        case PROP_XY_GRID:  break; // TODO
        case PROP_AXIS_TITLE_FONT: setAxisTitleFont(Converter.stringToSwtfont(value)); break;
        case PROP_LABEL_FONT: setTickLabelFont(Converter.stringToSwtfont(value)); break;
        case PROP_HIST_BAR: setBarType(Converter.stringToEnum(value, HistogramBar.class)); break;
        case PROP_HIST_COLOR: setHistogramColor(Converter.stringToOptionalRGB(value)); break;
        case PROP_HIST_CUMULATIVE: setHistogramCumulative(Converter.stringToBoolean(value)); break;
        case PROP_HIST_DENSITY: setHistogramDensity(Converter.stringToBoolean(value)); break;
        case PROP_SHOW_OVERFLOW_CELL: setShowOverflowCell(Converter.stringToBoolean(value)); break;
        case PROP_BAR_BASELINE: setBarBaseline(Converter.stringToDouble(value)); break;
        case PROP_Y_AXIS_LOGARITHMIC: setLogarithmicY(Converter.stringToBoolean(value)); break;
        default: super.setProperty(prop, value);
        }
    }

    @Override
    public void setProperty(PlotProperty prop, String key, String value) {
        switch (prop) {
        case PROP_HIST_COLOR: setHistogramColor(key, Converter.stringToOptionalRGB(value)); break;
        case PROP_HIST_CUMULATIVE: setHistogramCumulative(key, Converter.stringToOptionalBoolean(value)); break;
        case PROP_HIST_DENSITY: setHistogramDensity(key, Converter.stringToOptionalBoolean(value)); break;
        case PROP_HIST_BAR: setBarType(key, Converter.stringToEnum(value, HistogramBar.class)); break;
        default: super.setProperty(prop, key, value);
        }
    }

    @Override
    public void clear() {
        super.clear();
        histogramProperties.clear();
        setDataset(new PythonHistogramDataset(null));
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

    public void setTickLabelFont(Font font) {
        Assert.isNotNull(font);
        xAxis.setTickFont(font);
        yAxis.setTickFont(font);
        chartChanged();
    }

    public void setHistogramColor(RGB color) {
        defaultHistogramColor = color;
        updateLegends();
        chartChanged();
    }

    public void setHistogramCumulative(boolean value) {
        defaultHistogramCumulative = value;
        chartArea = calculatePlotArea();
        updateArea();
        updateLegends();
        chartChanged();
    }

    public void setHistogramDensity(boolean value) {
        defaultHistogramDensity = value;
        chartArea = calculatePlotArea();
        updateArea();
        updateLegends();
        chartChanged();
    }

    public void setBarType(PlotProperty.HistogramBar barType) {
        defaultBarType = barType;
        updateLegends();
        chartChanged();
    }


    public void setHistogramCumulative(String key, Boolean value) {
        HistogramProperties histProps = getOrCreateHistogramProperties(key);
        histProps.cumulative = value;
        chartArea = calculatePlotArea();
        updateArea();
        updateLegends();
        chartChanged();
    }

    public void setHistogramDensity(String key, boolean value) {
        HistogramProperties histProps = getOrCreateHistogramProperties(key);
        histProps.density = value;
        chartArea = calculatePlotArea();
        updateArea();
        updateLegends();
        chartChanged();
    }

    public void setBarType(String key, PlotProperty.HistogramBar barType) {
        HistogramProperties histProps = getOrCreateHistogramProperties(key);
        histProps.drawStyle = barType;
        chartArea = calculatePlotArea();
        updateLegends();
        chartChanged();
    }

    public void setShowOverflowCell(boolean value) {
        histograms.showOverflowCell = value;
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }


    public void setBarBaseline(double value) {
        histograms.baseline = value;
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

    public RGB getHistogramColor(String key) {
        HistogramProperties histProps = histogramProperties.get(key);
        RGB color = (histProps == null || histProps.color == null) ? defaultHistogramColor : histProps.color;
        return color;
    }

    public PlotProperty.HistogramBar getBarType(String key) {
        HistogramProperties histProps = histogramProperties.get(key);
        PlotProperty.HistogramBar barType = (histProps == null || histProps.drawStyle == null) ? defaultBarType: histProps.drawStyle;
        return barType;
    }

    public void setHistogramColor(String key, RGB color) {
        Assert.isNotNull(color);
        HistogramProperties histProps = getOrCreateHistogramProperties(key);
        histProps.color = color;
        updateLegends();
        chartChanged();
    }

    protected HistogramProperties getOrCreateHistogramProperties(String key) {
        HistogramProperties properties = histogramProperties.get(key);
        if (properties == null) {
            properties = new HistogramProperties();
            histogramProperties.put(key, properties);
        }
        return properties;
    }

    @Override
    void doSetDataset(IDataset dataset) {
        Assert.isLegal(dataset instanceof IHistogramDataset, "must be an IScalarDataset");
        this.dataset = (IHistogramDataset)dataset;
        updateLegends();
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    IHistogramDataset getDataset() {
        return dataset;
    }

    private void updateLegends() {
        histograms.updateLegend(legendTooltip);
        histograms.updateLegend(legend);
    }

    @Override
    String getHoverHtmlText(int x, int y) {
        if (histograms.getArea().contains(x, y))
            return histograms.getTooltipText(x, y);
        else
            return null;
    }

    @Override
    protected RectangularArea calculatePlotArea() {
        return histograms.calculatePlotArea();
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
            Rectangle remaining = mainArea.getCopy().crop(axesInsets);
            Rectangle plotArea = histograms.layout(graphics, remaining);
            legend.layout(graphics, plotArea, pass);
            //FIXME how to handle it when plotArea.height/width comes out negative??
            return plotArea;
        }
        return null;
    }

    @Override
    protected void doPaintCachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
        graphics.fillRectangle(GraphicsUtils.getClip(graphics));
        yAxis.drawGrid(graphics, coordsMapping);
        histograms.draw(graphics, coordsMapping);
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
        drawRubberband(graphics);
    }

    @Override
    public void setDisplayAxesDetails(boolean value) {
        xAxis.setDrawTickLabels(value);
        xAxis.setDrawTitle(value);
        yAxis.setDrawTickLabels(value);
        yAxis.setDrawTitle(value);
        chartChanged();
    }
}
