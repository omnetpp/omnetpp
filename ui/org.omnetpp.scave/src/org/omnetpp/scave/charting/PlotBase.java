/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_ANTIALIAS;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_CACHING;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_INSETS_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_INSETS_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LEGEND_BORDER;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LEGEND_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_LEGEND_POSITION;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_PLOT_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_PLOT_TITLE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_PLOT_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.PlotProperty.PROP_Y_AXIS_MIN;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.charting.properties.PlotProperty;
import org.omnetpp.scave.charting.properties.PlotProperty.LegendAnchor;
import org.omnetpp.scave.charting.properties.PlotProperty.LegendPosition;

/**
 * Base class for all native plot widgets.
 *
 * @author tomi, andras
 */
public abstract class PlotBase extends ZoomableCachingCanvas implements IPlotViewer {
    private static final boolean debug = false;

    protected static final PlotProperty[] PLOTBASE_PROPERTIES = new PlotProperty[] {
            PROP_PLOT_TITLE,
            PROP_PLOT_TITLE_FONT,
            PROP_PLOT_TITLE_COLOR,
            PROP_DISPLAY_LEGEND,
            PROP_LEGEND_BORDER,
            PROP_LEGEND_FONT,
            PROP_LEGEND_POSITION,
            PROP_LEGEND_ANCHORING,
            PROP_ANTIALIAS,
            PROP_CACHING,
            PROP_BACKGROUND_COLOR,
            PROP_INSETS_LINE_COLOR,
            PROP_INSETS_BACKGROUND_COLOR,
            PROP_Y_AXIS_MIN,
            PROP_Y_AXIS_MAX
    };

    // when displaying confidence intervals, XXX chart parameter?
    protected static final double CONFIDENCE_LEVEL = 0.95;

    protected boolean antialias;
    protected Color backgroundColor;
    protected Color insetsBackgroundColor;
    protected Color insetsLineColor;
    protected int veilAlpha = 180;
    protected Title title = new Title();
    protected String titleText;
    protected Legend legend = new Legend(this);
    protected LegendTooltip legendTooltip;
    private boolean adjustViewOnLegendItemEnablementChange = false; //TODO make user-settable?

    private String statusText = "No data available."; // displayed when there's no dataset
    private String warningText = null;
    private static final Font warningFont = new Font(null, new FontData("Arial", 10, SWT.NORMAL));

    /* bounds specified by the user*/
    protected RectangularArea userDefinedArea =
        new RectangularArea(Double.NEGATIVE_INFINITY, Double.NEGATIVE_INFINITY,
                Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY);
    /* bounds calculated from the dataset */
    protected RectangularArea chartArea;
    /* If this is not null, it stores a request to change the data area upon the next relayout.
     * This is necessary because the layouting process can change the scrolled viewport size
     * (because of changing scrollbar visibilities and axis tick label lengths), so the initial
     * requested data range may not end up being exactly what's visible at the end without this.
     */
    private RectangularArea zoomedAreaRequest;

    private ZoomableCanvasMouseSupport mouseSupport;

    protected IPlotSelection selection;
    private ListenerList<IChartSelectionListener> listeners = new ListenerList<>();

    private int layoutDepth = 0; // how many layoutChart() calls are on the stack
    private IDataset dataset;

    protected int batchedUpdateDepth = 0; // if >0, suppress change notifications until the batch is done
    protected int batchChangeCount = 0; // how many times chartChanged was called in the current (top-level) batch

    public PlotBase(Composite parent, int style) {
        super(parent, style);
        setToolTipText(null); // prevents "Close" tooltip of the TabItem from coming up (Linux only)

        legendTooltip = new LegendTooltip(this);

        mouseSupport = new ZoomableCanvasMouseSupport(this); // add mouse handling; may be made optional

        addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                layoutChart();
            }
        });

        addPropertyChangeListener((e) -> {
            String prop = e.getProperty();
            if (prop.equals(PROP_ZOOM_X) || prop.equals(PROP_ZOOM_Y)
                    || prop.equals(PROP_VIEW_X) || prop.equals(PROP_VIEW_Y)) {
                layoutChart();
            }
        });
    }

    /**
     * Sets the data to be visualized by the chart.
     */
    public void setDataset(IDataset dataset) {
        if (debug) Debug.println("setDataset()");
        doSetDataset(dataset);
        this.dataset = dataset;
        updateTitle();
        updateZoomedArea();
    }

    abstract void doSetDataset(IDataset dataset);

    abstract String getHoverHtmlText(int x, int y);

    protected void layoutChart() {
        // prevent nasty infinite layout recursions
        if (layoutDepth>0)
            return;

        // ignore initial invalid layout request
        if (getClientArea().isEmpty())
            return;

        layoutDepth++;
        if (debug) Debug.println("layoutChart(), level "+layoutDepth);

        // NOTE: This is a bit of a HACK, to help setZoomedArea() be a bit more accurate.
        // We make an educated guess about the final visibility state of the scroll bars,
        // and set them so early, to start the layouting process with the final viewport size.
        if (zoomedAreaRequest != null) {
            if (debug) Debug.println("zoomedAreaRequest=" + zoomedAreaRequest);
            getHorizontalBar().setVisible(zoomedAreaRequest.minX > getMinX() || zoomedAreaRequest.maxX < getMaxX());
            getVerticalBar().setVisible(zoomedAreaRequest.minY > getMinY() || zoomedAreaRequest.maxY < getMaxY());
        }

        Image image = new Image(getDisplay(), 1, 1);
        GC gc = new GC(image);
        Graphics graphics = new SWTGraphics(gc);
        try {
            // preserve zoomed-out state while resizing
            boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
            boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

            for (int pass = 1; pass <= 2; ++pass) {
                Rectangle plotArea = doLayoutChart(graphics, pass);
                setViewportRectangle(plotArea, pass == 2);

                if (shouldZoomOutX)
                    zoomToFitX();
                if (shouldZoomOutY)
                    zoomToFitY();

                updateZoomedArea(false);
            }
            validateZoom(); //Note: scrollbar.setVisible() triggers Resize too
        }
        catch (Throwable e) {
            ScavePlugin.logError(e);
        }
        finally {
            layoutDepth--;
            graphics.dispose();
            gc.dispose();
            image.dispose();
        }
        // may trigger another layoutChart()
        updateZoomedArea();
    }

    /**
     * Sets the zoomed area of the chart.
     * The change will be applied when the chart is layouted next time.
     */
    public void setZoomedArea(RectangularArea area) {
        this.zoomedAreaRequest = area;
        layoutChart();
    }

    private void updateZoomedArea() {
        updateZoomedArea(true);
    }

    /**
     * Applies the zoomed area was set by {@code setZoomedArea}.
     * To be called after the has an area (calculated from the dataset)
     * and the virtual size of the canvas is calculated.
     */
    private void updateZoomedArea(boolean clear) {
        if (zoomedAreaRequest != null && !getBounds().isEmpty() && dataset != null) {
            if (debug) Debug.format("Restoring zoomed area: %s%n", zoomedAreaRequest);
            RectangularArea area = zoomedAreaRequest;
            if (clear)
                zoomedAreaRequest = null;
            zoomToArea(area);
        }
    }

    /**
     * Calculate positions of chart elements such as title, legend, axis labels, plot area.
     * @param pass TODO
     */
    abstract protected Rectangle doLayoutChart(Graphics graphics, int pass);

    /*-------------------------------------------------------------------------------------------
     *                                      Drawing
     *-------------------------------------------------------------------------------------------*/
    private ICoordsMapping coordsMapping;

    @Override
    protected void paintCachableLayer(Graphics graphics) {
        if (debug) {
            Debug.println("paintCachableLayer()");
            Debug.println(String.format("area=%f, %f, %f, %f, zoom: %f, %f", getMinX(), getMaxX(), getMinY(), getMaxY(), getZoomX(), getZoomY()));
            Debug.println(String.format("view port=%s, vxy=%d, %d", getViewportRectangle(), getViewportLeft(), getViewportTop()));
        }
        if (getClientArea().isEmpty())
            return;

        coordsMapping = getOptimizedCoordinateMapper();
        resetDrawingStylesAndColors(graphics);
        doPaintCachableLayer(graphics, coordsMapping);
    }

    @Override
    protected void paintNoncachableLayer(Graphics graphics) {
        if (debug) Debug.println("paintNoncachableLayer()");
        if (getClientArea().isEmpty())
            return;

        if (coordsMapping == null)
            coordsMapping = getOptimizedCoordinateMapper();
        resetDrawingStylesAndColors(graphics);

        doPaintNoncachableLayer(graphics, coordsMapping);

        coordsMapping = null;
    }

    abstract protected void doPaintCachableLayer(Graphics graphics, ICoordsMapping coordsMapping);
    abstract protected void doPaintNoncachableLayer(Graphics graphics, ICoordsMapping coordsMapping);

    /**
     *
     * @return
     */
    protected abstract RectangularArea calculatePlotArea();

    public IPlotSelection getSelection() {
        return selection;
    }

    public void setSelection(IPlotSelection selection) {
        if (!ObjectUtils.equals(this.selection, selection)) {
            this.selection = selection;
            chartChanged();
            fireChartSelectionChange(selection);
        }
    }

    public void addChartSelectionListener(IChartSelectionListener listener) {
        listeners.add(listener);
    }

    public void removeChartSelectionListener(IChartSelectionListener listener) {
        listeners.remove(listener);
    }

    protected void fireChartSelectionChange(IPlotSelection selection) {
        for (IChartSelectionListener listener : listeners)
            listener.selectionChanged(selection);
    }

    /**
     * Switches between zoom and pan mode.
     * @param mouseMode should be ZoomableCanvasMouseSupport.PAN_MODE or ZoomableCanvasMouseSupport.ZOOM_MODE
     */
    public void setMouseMode(int mouseMode) {
        mouseSupport.setMouseMode(mouseMode);
    }

    public int getMouseMode() {
        return mouseSupport.getMouseMode();
    }

    public String getStatusText() {
        return statusText;
    }

    public void setStatusText(String statusText) {
        this.statusText = statusText;
        chartChanged();
    }

    public String getWarningText() {
        return warningText;
    }

    public void setWarningText(String warningText) {
        this.warningText = warningText;
        chartChanged();
    }

    /*----------------------------------------------------------------------
     *                            Properties
     *----------------------------------------------------------------------*/

    public PlotProperty[] getProperties() {
        return PLOTBASE_PROPERTIES;
    }

    public String[] getPropertyNames() {
        PlotProperty[] props = getProperties();
        String[] names = new String[props.length];
        for (int i=0; i<props.length; i++)
            names[i] = props[i].getName();
        return names;
    }

    public void setProperty(String propertyName, String value) {
        int index = propertyName.indexOf('/');
        String name = index >= 0 ? propertyName.substring(0, index) : propertyName;
        String key = index >= 0 ? propertyName.substring(index+1) : null;

        PlotProperty prop = PlotProperty.lookup(name);
        if (prop == null)
            throw new RuntimeException("unrecognized plot property in " + getClass().getSimpleName() + ": "  + name + " = " + value);

        try {
            if (key == null)
                setProperty(prop, value);
            else
                setProperty(prop, key, value);
        } catch (Exception e) {
            throw new RuntimeException("Error while setting plot property '" + propertyName + "': " + e.getMessage());
        }
    }

    public void setProperty(PlotProperty prop, String value) {
        switch (prop) {
        // Titles
        case PROP_PLOT_TITLE: setTitle(value); break;
        case PROP_PLOT_TITLE_FONT: setTitleFont(Converter.stringToSwtfont(value)); break;
        case PROP_PLOT_TITLE_COLOR: setTitleColor(Converter.stringToRGB(value)); break;
        // Legend
        case PROP_DISPLAY_LEGEND: setDisplayLegend(Converter.stringToBoolean(value)); break;
        case PROP_LEGEND_BORDER: setLegendBorder(Converter.stringToBoolean(value)); break;
        case PROP_LEGEND_FONT: setLegendFont(Converter.stringToSwtfont(value)); break;
        case PROP_LEGEND_POSITION: setLegendPosition(Converter.stringToEnum(value, LegendPosition.class)); break;
        case PROP_LEGEND_ANCHORING: setLegendAnchor(Converter.stringToEnum(value, LegendAnchor.class)); break;
        // Plot
        case PROP_ANTIALIAS: setAntialias(Converter.stringToBoolean(value)); break;
        case PROP_CACHING: setCaching(Converter.stringToBoolean(value)); break;
        case PROP_BACKGROUND_COLOR: setBackgroundColor(Converter.stringToRGB(value)); break;
        case PROP_INSETS_LINE_COLOR: setInsetsLineColor(Converter.stringToRGB(value)); break;
        case PROP_INSETS_BACKGROUND_COLOR: setInsetsBackgroundColor(Converter.stringToRGB(value)); break;
        // Axes
        case PROP_Y_AXIS_MIN: setYMin(Converter.stringToDouble(value)); break;
        case PROP_Y_AXIS_MAX: setYMax(Converter.stringToDouble(value)); break;
        default: throw new RuntimeException("unsupported plot property in " + getClass().getSimpleName() + ": "  + prop.getName() + " = " + value);
        }
    }

    public void setProperty(PlotProperty prop, String key, String value) {
        throw new RuntimeException("unsupported per-item plot property in " + getClass().getSimpleName() + ": "  + prop.getName() + "/" + key + " = " + value);
    }

    protected void resetProperties() {
        for (PlotProperty prop : getProperties()) {
            String value = prop.getDefaultValueAsString();
            setProperty(prop, value);
        }
    }

    /*
     * Clears all data and resets the visual properties to their defaults.
     */
    public void clear() {
        resetProperties();
        setStatusText(null);
        setWarningText(null);
    }

    public boolean getAntialias() {
        return antialias;
    }

    public void setAntialias(boolean antialias) {
        this.antialias = antialias;
        chartChanged();
    }

    @Override
    public void setCaching(boolean caching) {
        super.setCaching(caching);
        chartChanged();
    }

    public void setBackgroundColor(RGB rgb) {
        Assert.isNotNull(rgb);
        this.backgroundColor = new Color(rgb);
        chartChanged();
    }

    public void setInsetsLineColor(RGB rgb) {
        Assert.isNotNull(rgb);
        this.insetsLineColor = new Color(rgb);
        chartChanged();
    }

    public void setInsetsBackgroundColor(RGB rgb) {
        Assert.isNotNull(rgb);
        this.insetsBackgroundColor = new Color(rgb);
        chartChanged();
    }

    public void setTitle(String value) {
        Assert.isNotNull(value);
        titleText = value;
        updateTitle();
    }

    private void updateTitle() {
        if (!ObjectUtils.equals(titleText, title.getText())) {
            title.setText(titleText);
            chartChanged();
        }
    }

    public void setTitleFont(Font value) {
        Assert.isNotNull(value);
        title.setFont(value);
        chartChanged();
    }

    public void setTitleColor(RGB value) {
        Assert.isNotNull(value);
        title.setColor(value);
        chartChanged();
    }

    public abstract void setDisplayAxesDetails(boolean value);

    public void setDisplayTitle(boolean value) {
        title.setVisible(value);
        chartChanged();
    }

    public void setDisplayLegend(boolean value) {
        legend.setVisible(value);
        chartChanged();
    }

    public void setDisplayLegendTooltip(boolean value) {
        legendTooltip.setVisible(value);
        chartChanged();
    }

    public void setLegendBorder(boolean value) {
        legend.setDrawBorder(value);
        chartChanged();
    }

    public void setLegendFont(Font value) {
        Assert.isNotNull(value);
        legend.setFont(value);
        chartChanged();
    }

    public void setLegendPosition(LegendPosition value) {
        Assert.isNotNull(value);
        legend.setPosition(value);
        chartChanged();
    }

    public void setLegendAnchor(LegendAnchor value) {
        Assert.isNotNull(value);
        legend.setAnchor(value);
        chartChanged();
    }

    public void setXMin(double value) {
        userDefinedArea.minX = value;
        updateArea();
        chartChanged();
    }

    public void setXMax(double value) {
        userDefinedArea.maxX = value;
        updateArea();
        chartChanged();
    }

    public void setYMin(double value) {
        userDefinedArea.minY = value;
        updateArea();
        chartChanged();
    }

    public void setYMax(double value) {
        userDefinedArea.maxY = value;
        updateArea();
        chartChanged();
    }

    /**
     * Sets the area of the zoomable canvas.
     * This method is called when the area changes because
     * <ul>
     * <li> the dataset changed and a new chart area calculated
     * <li> the user changed the bounds of the are by setting a chart property
     * </ul>
     */
    protected void updateArea() {
        if (batchedUpdateDepth > 0) {
            batchChangeCount++;
        }
        else {
            if (chartArea == null)
                return;

            RectangularArea area = transformArea(userDefinedArea);
            area.minX = Double.isInfinite(area.minX) ? chartArea.minX : area.minX;
            area.minY = Double.isInfinite(area.minY) ? chartArea.minY : area.minY;
            area.maxX = Double.isInfinite(area.maxX) ? chartArea.maxX : area.maxX;
            area.maxY = Double.isInfinite(area.maxY) ? chartArea.maxY : area.maxY;

            if (!area.equals(getArea())) {
                if (debug) Debug.format("Update area: %s --> %s%n", getArea(), area);
                setArea(area);
            }
        }
    }

    protected RectangularArea transformArea(RectangularArea area) {
        double minX = transformX(area.minX);
        double minY = transformY(area.minY);
        double maxX = transformX(area.maxX);
        double maxY = transformY(area.maxY);

        return new RectangularArea(
            Double.isNaN(minX) || Double.isInfinite(minX) ? Double.NEGATIVE_INFINITY : minX,
            Double.isNaN(minY) || Double.isInfinite(minY)? Double.NEGATIVE_INFINITY : minY,
            Double.isNaN(maxX) || Double.isInfinite(maxX)? Double.POSITIVE_INFINITY : maxX,
            Double.isNaN(maxY) || Double.isInfinite(maxY) ? Double.POSITIVE_INFINITY : maxY
        );
    }

    protected double transformX(double x) {
        return x;
    }

    protected double transformY(double y) {
        return y;
    }

    protected double inverseTransformX(double x) {
        return x;
    }

    protected double inverseTransformY(double y) {
        return y;
    }

    /**
     * Resets all graphics settings except clipping and transform.
     */
    public void resetDrawingStylesAndColors(Graphics graphics) {
        graphics.setAntialias(antialias ? SWT.ON : SWT.OFF);
        graphics.setAlpha(255);
        graphics.setBackgroundColor(backgroundColor);
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.setLineWidth(0);
        graphics.setLineStyle(SWT.LINE_SOLID);
        //graphics.setFillRule();
        //graphics.setLineCap();
        //graphics.setLineJoin();
        //graphics.setXORMode(false);
        //graphics.setFont(null);
        // TODO: these operations are not supported by SVGGraphics yet
        if (!GraphicsUtils.isSVGGraphics(graphics)) {
            graphics.setBackgroundPattern(null);
            graphics.setForegroundPattern(null);
            graphics.setInterpolation(SWT.DEFAULT);
            graphics.setLineDash((int[])null);
            graphics.setTextAntialias(SWT.DEFAULT);
        }
    }

    public void runBatchedUpdates(Runnable updater) {
        batchedUpdateDepth++;
        try {
            updater.run();
        }
        finally {
            batchedUpdateDepth--;
            if (batchedUpdateDepth == 0) {
                if (batchChangeCount > 0) {
                    updateLegends();
                    chartArea = calculatePlotArea();
                    updateArea();
                    chartChanged();
                }
                batchChangeCount = 0;
            }
        }
    }

    protected abstract void updateLegend(ILegend legend);

    protected void updateLegends() {
        if (batchedUpdateDepth > 0) {
            batchChangeCount++;
        }
        else {
            updateLegend(legendTooltip);
            updateLegend(legend);

            Image image = new Image(getDisplay(), 1, 1);
            GC gc = new GC(image);
            Graphics graphics = new SWTGraphics(gc);

            // Calculate space occupied by title and legend and set insets accordingly

            Rectangle area = new Rectangle(getClientArea());
            legendTooltip.layout(graphics, area);

            // XXX: this is not really nice (we shouldn't touch the title, only need the
            // "remaining" rect), but this is how the layout procedure was originally weaved
            Rectangle remaining = title.layout(graphics, area);

            legend.layout(graphics, remaining, 1);
            legend.layout(graphics, getViewportRectangle(), 2);

            graphics.dispose();
            gc.dispose();
            image.dispose();
        }
    }

    protected void chartChanged() {
        if (batchedUpdateDepth > 0) {
            batchChangeCount++;
        }
        else {
            layoutChart();
            clearCanvasCacheAndRedraw();
        }
    }

    public void legendItemEnablementChanged() {
        if (adjustViewOnLegendItemEnablementChange) {
            double zx = getZoomX();
            double zy = getZoomY();
            long vt = getViewportTop();
            long vl = getViewportLeft();

            boolean shouldZoomOutX = getZoomX()==0 || isZoomedOutX();
            boolean shouldZoomOutY = getZoomY()==0 || isZoomedOutY();

            chartChanged();
            chartArea = calculatePlotArea();
            updateArea();

            if (shouldZoomOutX)
                zoomToFitX();
            else {
                setZoomX(zx);
                scrollHorizontalTo(vl);
            }

            if (shouldZoomOutY)
                zoomToFitY();
            else {
                setZoomY(zy);
                scrollVerticalTo(vt);
            }
        }
        else {
            chartChanged();
        }
    }

    protected void paintInsets(Graphics graphics) {
        Insets insets = getInsets();
        // do not draw insets border when exporting to SVG
        if (!GraphicsUtils.isSVGGraphics(graphics)) {
            Rectangle canvasRect = new Rectangle(getClientArea());
            graphics.setForegroundColor(insetsBackgroundColor);
            graphics.setBackgroundColor(insetsBackgroundColor);
            graphics.fillRectangle(0, 0, canvasRect.width, insets.top); // top
            graphics.fillRectangle(0, canvasRect.bottom()-insets.bottom, canvasRect.width, insets.bottom); // bottom
            graphics.fillRectangle(0, 0, insets.left, canvasRect.height); // left
            graphics.fillRectangle(canvasRect.right()-insets.right, 0, insets.right, canvasRect.height); // right
            graphics.setForegroundColor(insetsLineColor);
        }
        graphics.drawRectangle(insets.left, insets.top, getViewportWidth(), getViewportHeight());
    }

    protected void drawStatusText(Graphics graphics) {
        String text;
        boolean isWarning = false;
        if (getWarningText() != null) {
            text = getWarningText();
            isWarning = true;
        }
        else if (getStatusText() != null && !StringUtils.isBlank(getStatusText())) {
            text = getStatusText();
        }
        else {
            return;
        }

        Rectangle rect = getViewportRectangle();

        TextLayout textLayout = new TextLayout(getDisplay());
        textLayout.setText(text);
        textLayout.setFont(warningFont);
        textLayout.setWidth(rect.width - 20);

        int textWidth = 0;
        for (int line = 0; line < textLayout.getLineCount(); ++line)
            textWidth = Integer.max(textWidth, textLayout.getLineBounds(line).width);
        int textHeight = textLayout.getBounds().height;

        resetDrawingStylesAndColors(graphics);

        graphics.setBackgroundColor(ColorFactory.WHITE);
        graphics.setAlpha(192);
        graphics.fillRectangle(rect.x + 4, rect.y + 4, textWidth + 12, textHeight + 12);

        if (isWarning)
            graphics.setForegroundColor(ColorFactory.RED);
        graphics.setAlpha(255);
        graphics.drawTextLayout(textLayout, rect.x + 10, rect.y + 10);

        textLayout.dispose();
    }

    protected void drawRubberband(Graphics graphics) {
        mouseSupport.drawRubberband(graphics);
    }

    protected String formatValue(double value, double halfInterval) {
        return !Double.isNaN(halfInterval) && halfInterval > 0.0 ?
                String.format("%.3g\u00b1%.3g", value, halfInterval) :
                String.format("%g", value);
    }

    public ZoomableCachingCanvas getCanvas() {
        return this;
    }

    public Legend getLegend() {
        return legend;
    }

    @Override
    public Point computeSize(int wHint, int hHint) {
        return new Point(800, 600);
    }
    @Override
    public Point computeSize(int wHint, int hHint, boolean changed) {
        return new Point(800, 600);
    }
}
