/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_HIST_BAR;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_HIST_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_HIST_DATA;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_SHOW_OVERFLOW_CELL;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.properties.PlotProperties.HistogramBar;
import org.omnetpp.scave.charting.properties.PlotProperties.HistogramDataType;
import org.omnetpp.scave.python.PythonHistogramDataset;

/**
 * Histogram plot widget.
 */
public class HistogramPlot extends PlotViewerBase {
    private static final boolean debug = false;

    private static final String[] HISTOGRAMPLOT_PROPERTY_NAMES = ArrayUtils.addAll(PLOTBASE_PROPERTY_NAMES, new String[] {
            PROP_X_AXIS_TITLE,
            PROP_Y_AXIS_TITLE,
            PROP_AXIS_TITLE_FONT,
            PROP_LABEL_FONT,
            PROP_X_LABELS_ROTATE_BY,
            PROP_HIST_BAR,
            PROP_HIST_DATA,
            PROP_SHOW_OVERFLOW_CELL,
            PROP_BAR_BASELINE,
            PROP_HIST_COLOR,
            PROP_Y_AXIS_LOGARITHMIC,
    });

    private IHistogramDataset dataset = IHistogramDataset.EMPTY;
    private LinearAxis xAxis = new LinearAxis(false, false, false);
    private LinearAxis yAxis = new LinearAxis(true, false, false);
    private Histograms histograms;

    static class HistogramProperties {
        RGB color;
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

    public String[] getPropertyNames() {
        return HISTOGRAMPLOT_PROPERTY_NAMES;
    }

    @Override
    public void setProperty(String name, String value) {
        Assert.isNotNull(name);
        Assert.isNotNull(value);
        if (debug) Debug.println("HistogramChartViewer.setProperty: "+name+"='"+value+"'");

        if (PROP_X_AXIS_TITLE.equals(name))
            setXAxisTitle(value);
        else if (PROP_Y_AXIS_TITLE.equals(name))
            setYAxisTitle(value);
        else if (PROP_AXIS_TITLE_FONT.equals(name))
            setAxisTitleFont(Converter.tolerantStringToOptionalSwtfont(value));
        else if (PROP_LABEL_FONT.equals(name))
            setTickLabelFont(Converter.tolerantStringToOptionalSwtfont(value));
        else if (PROP_X_LABELS_ROTATE_BY.equals(name))
            ; //TODO PROP_X_LABELS_ROTATE_BY
        else if (PROP_HIST_BAR.equals(name))
            setBarType(Converter.tolerantStringToOptionalEnum(value, HistogramBar.class));
        else if (PROP_HIST_DATA.equals(name))
            setHistogramDataTransform(Converter.tolerantStringToOptionalEnum(value, HistogramDataType.class));
        else if (PROP_SHOW_OVERFLOW_CELL.equals(name))
            setShowOverflowCell(Converter.tolerantStringToOptionalBoolean(value));
        else if (PROP_BAR_BASELINE.equals(name))
            setBarBaseline(Converter.tolerantStringToOptionalDouble(value));
        else if (name.startsWith(PROP_HIST_COLOR))
            setHistogramColor(getElementId(name), ColorFactory.asRGB(value));
        else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
            setLogarithmicY(Converter.tolerantStringToOptionalBoolean(value));
        else
            super.setProperty(name, value);
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

    public void setBarType(HistogramBar barType) {
        Assert.isNotNull(barType);
        histograms.setBarType(barType);
        chartChanged();
    }

    public void setHistogramDataTransform(HistogramDataType dataTransform) {
        Assert.isNotNull(dataTransform);
        histograms.setHistogramData(dataTransform);
        chartArea = calculatePlotArea();
        updateArea();
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
        return histProps != null ? histProps.color : null;
    }

    public void setHistogramColor(String key, RGB color) {
        Assert.isNotNull(color);
        HistogramProperties histProps = getOrCreateHistogramProperties(key);
        histProps.color = color;
        updateLegends();
        chartChanged();
    }

    private HistogramProperties getOrCreateHistogramProperties(String key) {
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
