/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_AXIS_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_BASELINE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_OUTLINE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_GRID;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_GRID_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_WRAP_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_SHOW_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_SHOW_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_SHOW_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_SHOW_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringEscapeUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.dataset.IGroupsSeriesDataset;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.omnetpp.scave.charting.properties.PlotProperty.BarPlacement;
import org.omnetpp.scave.charting.properties.PlotProperty.ShowGrid;
import org.omnetpp.scave.python.GroupsSeriesDataset;

/**
 * Bar plot widget.
 */
public class BarPlot extends PlotBase {
    //private static final boolean debug = false;

    protected final static PlotProperty[] BARPLOT_PROPERTIES = ArrayUtils.addAll(PLOTBASE_PROPERTIES, new PlotProperty[] {
            PROP_X_AXIS_TITLE,
            PROP_X_LABELS_ROTATE_BY,
            PROP_X_AXIS_SHOW_TITLE,
            PROP_X_AXIS_SHOW_LABELS,
            PROP_Y_AXIS_TITLE,
            PROP_Y_AXIS_LOGARITHMIC,
            PROP_Y_AXIS_SHOW_TITLE,
            PROP_Y_AXIS_SHOW_LABELS,
            PROP_AXIS_TITLE_FONT,
            PROP_AXIS_LABEL_FONT,
            PROP_WRAP_LABELS,
            PROP_BAR_BASELINE,
            PROP_BAR_BASELINE_COLOR,
            PROP_BAR_PLACEMENT,
            PROP_BAR_COLOR,
            PROP_BAR_OUTLINE_COLOR,
            PROP_GRID,
            PROP_GRID_COLOR,
    });

    private RGB defaultBarColor;
    private RGB defaultBarOutlineColor;

    private IGroupsSeriesDataset dataset;

    private LinearAxis valueAxis = new LinearAxis(true, false, false);
    private DomainAxis domainAxis = new DomainAxis(this);
    private Bars bars;

    static class BarProperties {
        RGB color;
        public RGB outlineColor;
    }

    private Map<String,BarProperties> barProperties = new HashMap<>();

    static class BarSelection implements IPlotSelection {
        // TODO selection on ScalarCharts
    }

    public BarPlot(Composite parent, int style) {
        super(parent, style);
        bars = new Bars(this);
        new Tooltip(this);

        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent e) {
                setSelection(new BarSelection());
            }
        });

        resetProperties();
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
        if (dataset != null && !(dataset instanceof IGroupsSeriesDataset))
            throw new IllegalArgumentException("must be an IScalarDataset");

        this.dataset = (IGroupsSeriesDataset)dataset;
        updateLegends();
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public IGroupsSeriesDataset getDataset() {
        return dataset;
    }

    public Bars getPlot() {
        return bars;
    }

    @Override
    protected RectangularArea calculatePlotArea() {
        return bars.calculatePlotArea();
    }

    private void updateLegends() {
        updateLegend(legend);
        updateLegend(legendTooltip);
    }

    private void updateLegend(ILegend legend) {
        legend.clearItems();
        IPlotSymbol symbol = new SquareSymbol(6);
        if (dataset != null) {
            for (int i = 0; i < dataset.getSeriesCount(); ++i) {
                legend.addItem(bars.getBarColor(i), dataset.getSeriesTitle(i), symbol, false);
            }
        }
    }

    /*=============================================
     *               Properties
     *=============================================*/

    public PlotProperty[] getProperties() {
        return BARPLOT_PROPERTIES;
    }

    @Override
    public void setProperty(PlotProperty prop, String value) {
        switch (prop) {
        // Titles
        case PROP_X_AXIS_TITLE: setXAxisTitle(value); break;
        case PROP_Y_AXIS_TITLE: setYAxisTitle(value); break;
        case PROP_AXIS_TITLE_FONT: setAxisTitleFont(Converter.stringToSwtfont(value)); break;
        case PROP_AXIS_LABEL_FONT: setLabelFont(Converter.stringToSwtfont(value)); break;
        case PROP_X_LABELS_ROTATE_BY: setXAxisLabelsRotatedBy(Converter.stringToDouble(value)); break;
        case PROP_WRAP_LABELS: setWrapLabels(Converter.stringToBoolean(value)); break;
        // Bars
        case PROP_BAR_BASELINE: setBarBaseline(Converter.stringToDouble(value)); break;
        case PROP_BAR_BASELINE_COLOR: setBarBaselineColor(Converter.stringToRGB(value)); break;
        case PROP_BAR_PLACEMENT: setBarPlacement(Converter.stringToEnum(value, BarPlacement.class)); break;
        case PROP_BAR_COLOR: setBarColor(Converter.stringToRGB(value)); break;
        case PROP_BAR_OUTLINE_COLOR: setBarOutlineColor(Converter.stringToRGB(value)); break;
        // Axes
        case PROP_GRID: setShowGrid(Converter.stringToEnum(value, ShowGrid.class)); break;
        case PROP_GRID_COLOR: setGridColor(Converter.stringToRGB(value)); break;
        case PROP_Y_AXIS_LOGARITHMIC: setLogarithmicY(Converter.stringToBoolean(value)); break;
        case PROP_X_AXIS_SHOW_TITLE:  setXAxisShowTitle(Converter.stringToBoolean(value)); break;
        case PROP_X_AXIS_SHOW_LABELS:  setXAxisShowLabels(Converter.stringToBoolean(value)); break;
        case PROP_Y_AXIS_SHOW_TITLE:  setYAxisShowTitle(Converter.stringToBoolean(value)); break;
        case PROP_Y_AXIS_SHOW_LABELS:  setYAxisShowLabels(Converter.stringToBoolean(value)); break;
        default: super.setProperty(prop, value);
        }
    }

    @Override
    public void setProperty(PlotProperty prop, String key, String value) {
        switch (prop) {
        case PROP_BAR_COLOR: setBarColor(key, Converter.stringToOptionalRGB(value)); break;
        case PROP_BAR_OUTLINE_COLOR: setBarOutlineColor(key, Converter.stringToOptionalRGB(value)); break;
        default: super.setProperty(prop, key, value);
        }
    }

    @Override
    public void clear() {
        super.clear();
        barProperties.clear();
        setDataset(new GroupsSeriesDataset(null));
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
        Assert.isNotNull(title);
        domainAxis.title = title;
        chartChanged();
    }

    public String getYAxisTitle() {
        return valueAxis.getTitle();
    }

    public void setYAxisTitle(String title) {
        Assert.isNotNull(title);
        valueAxis.setTitle(title);
        chartChanged();
    }

    public Font getAxisTitleFont() {
        return domainAxis.titleFont;
    }

    public void setAxisTitleFont(Font font) {
        Assert.isNotNull(font);
        domainAxis.titleFont = font;
        valueAxis.setTitleFont(font);
        chartChanged();
    }

    public void setLabelFont(Font font) {
        Assert.isNotNull(font);
        domainAxis.labelsFont = font;
        valueAxis.setTickFont(font);
        chartChanged();
    }

    public void setXAxisLabelsRotatedBy(double angle) {
        domainAxis.rotation = Math.max(0, Math.min(90, angle));
        chartChanged();
    }

    public void setWrapLabels(boolean value) {
        domainAxis.wrapLabels = value;
        chartChanged();
    }

    public Double getBarBaseline() {
        return bars.barBaseline;
    }

    public void setBarBaseline(double value) {
        bars.barBaseline = value;
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setBarBaselineColor(RGB value) {
        Assert.isNotNull(value);
        bars.barBaselineColor = value;
        updateArea();
        chartChanged();
    }

    public BarPlacement getBarPlacement() {
        return bars.barPlacement;
    }

    public void setBarPlacement(BarPlacement value) {
        Assert.isNotNull(value);
        bars.barPlacement = value;
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setBarColor(RGB color) {
        defaultBarColor = color;
        updateLegends();
        chartChanged();
    }

    public void setBarOutlineColor(RGB color) {
        defaultBarOutlineColor = color;
        updateLegends();
        chartChanged();
    }

    public void setLogarithmicY(boolean logarithmic) {
        valueAxis.setLogarithmic(logarithmic);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setXAxisShowTitle(boolean show) {
        domainAxis.setDrawTitle(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setXAxisShowLabels(boolean show) {
        domainAxis.setLabels(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setYAxisShowTitle(boolean show) {
        valueAxis.setDrawTitle(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setYAxisShowLabels(boolean show) {
        valueAxis.setDrawTickLabels(show);
        chartArea = calculatePlotArea();
        updateArea();
        chartChanged();
    }

    public void setShowGrid(ShowGrid value) {
        Assert.isNotNull(value);
        valueAxis.setShowGrid(value);
        chartChanged();
    }

    public void setGridColor(RGB color) {
        Assert.isNotNull(color);
        valueAxis.setGridColor(color);
        chartChanged();
    }

    public RGB getBarColor(String key) {
        BarProperties barProps = barProperties.get(key);
        return barProps == null ? null : barProps.color;
    }

    public RGB getEffectiveBarColor(String key) {
        BarProperties barProps = barProperties.get(key);
        return (barProps == null || barProps.color == null) ? defaultBarColor : barProps.color;
    }

    public void setBarColor(String key, RGB color) {
        BarProperties barProps = getOrCreateBarProperties(key);
        barProps.color = color;
        updateLegends();
        chartChanged();
    }

    public void setBarOutlineColor(String key, RGB color) {
        BarProperties barProps = getOrCreateBarProperties(key);
        barProps.outlineColor = color;
        updateLegends();
        chartChanged();
    }

    public RGB getEffectiveBarOutlineColor(String key) {
        BarProperties barProps = barProperties.get(key);
        return (barProps == null || barProps.outlineColor == null) ? defaultBarOutlineColor : barProps.outlineColor;
    }

    public String getKeyFor(int seriesIndex) {
        if (seriesIndex >= 0 && seriesIndex < dataset.getSeriesCount())
            return dataset.getSeriesKey(seriesIndex);
        else
            return null;
    }

    private BarProperties getOrCreateBarProperties(String key) {
        BarProperties properties = barProperties.get(key);
        if (properties == null) {
            properties = new BarProperties();
            barProperties.put(key, properties);
        }
        return properties;
    }

    /*=============================================
     *               Drawing
     *=============================================*/
    Rectangle mainArea; // containing plots and axes
    Insets axesInsets; // space occupied by axes

    @Override
    protected Rectangle doLayoutChart(Graphics graphics, int pass) {
        ICoordsMapping mapping = getOptimizedCoordinateMapper();

        if (pass == 1) {
            // Calculate space occupied by title and legend and set insets accordingly
            Rectangle area = new Rectangle(getClientArea());
            Rectangle remaining = legendTooltip.layout(graphics, area);
            remaining = title.layout(graphics, area);
            remaining = legend.layout(graphics, remaining, 1);

            mainArea = remaining.getCopy();
            axesInsets = domainAxis.layout(graphics, mainArea, new Insets(), mapping, pass);
            axesInsets = valueAxis.layout(graphics, mainArea, axesInsets, mapping, pass);

            // tentative plotArea calculation (y axis ticks width missing from the picture yet)
            Rectangle plotArea = mainArea.getCopy().shrink(axesInsets);
            return plotArea;
        }
        else if (pass == 2) {
            // now the coordinate mapping is set up, so the y axis knows what tick labels
            // will appear, and can calculate the occupied space from the longest tick label.
            valueAxis.layout(graphics, mainArea, axesInsets, mapping, pass);
            domainAxis.layout(graphics, mainArea, axesInsets, mapping, pass);
            Rectangle remaining = mainArea.getCopy().shrink(axesInsets);
            remaining = legend.layout(graphics, remaining, pass);
            //FIXME how to handle it when plotArea.height/width comes out negative??
            Rectangle plotArea = bars.layout(graphics, remaining);
            return plotArea;
        }
        else
            return null;
    }

    @Override
    protected void doPaintCachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
        graphics.fillRectangle(GraphicsUtils.getClip(graphics));
        valueAxis.drawGrid(graphics, coordsMapping);
        bars.draw(graphics, coordsMapping);
    }

    @Override
    protected void doPaintNoncachableLayer(Graphics graphics, ICoordsMapping coordsMapping) {
        paintInsets(graphics);
        bars.drawBaseline(graphics, coordsMapping);
        title.draw(graphics);
        legend.draw(graphics);
        valueAxis.drawAxis(graphics, coordsMapping);
        domainAxis.draw(graphics, coordsMapping);
        drawRubberband(graphics);
        legendTooltip.draw(graphics);
        drawStatusText(graphics);
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
    String getHoverHtmlText(int x, int y) {
        int groupSeries = bars.findGroupSeries(fromCanvasX(x), fromCanvasY(y));
        if (groupSeries != -1) {
            int numSeries = dataset.getSeriesCount();
            int group = groupSeries / numSeries;
            int series = groupSeries % numSeries;
            String valueStr = dataset.getValueAsString(series, group);
            String title = dataset.getSeriesTitle(series);

            String line1 = StringEscapeUtils.escapeHtml4(title);
            String line2 = "value: " + valueStr;

            TextLayout textLayout = new TextLayout(getDisplay());
            textLayout.setText(line1 + "\n" + line2);
            textLayout.setWidth(320);

            return line1 + "<br>" + line2;
        }
        return null;
    }

    @Override
    public void setDisplayAxesDetails(boolean value) {
        valueAxis.setDrawTickLabels(value);
        valueAxis.setDrawTitle(value);
        domainAxis.setLabels(value);
        domainAxis.setDrawTitle(value);
        chartChanged();
    }
}
