/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import org.apache.commons.lang3.StringEscapeUtils;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseTrackAdapter;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.LinePlot.DataPoint;
import org.omnetpp.scave.charting.LinePlot.LineProperties;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.properties.PlotProperty.DrawStyle;
import org.omnetpp.scave.charting.properties.PlotProperty.LineStyle;

/**
 * Displays crosshair mouse cursor for VectorChart.
 *
 * Note: add the CrossHair to the chart AFTER the {@link ZoomableCanvasMouseSupport}.
 */
class CrossHair {
    public static final Cursor CROSS_CURSOR = new Cursor(null, SWT.CURSOR_CROSS);
    public static final Font CROSSHAIR_NORMAL_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
    public static final Font CROSSHAIR_BOLD_FONT = new Font(null, "Arial", 8, SWT.BOLD);
    public static final Color TOOLTIP_COLOR = new Color(255, 255, 225);

    private static final int MAXCOUNT = 100;
    private static final int HALO = 3;

    private final LinePlot parent;

    private boolean detailedTooltip = false; // turned on while hovering

    Rectangle plotArea;

    // to transfer the coordinates from mouseMoved() to paint()
    private int canvasX = Integer.MAX_VALUE;
    private int canvasY = Integer.MAX_VALUE;

    private DeltaMeasurement deltaMeasurement;

    public CrossHair(LinePlot parent) {
        this.parent = parent;

        final LinePlot finalParent = parent;
        // add key listener to restore the cross cursor, after other cursor is turned off
        // (by ZoomableCanvasMouseSupport for example)
        parent.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (finalParent.getCursor() == null)
                    finalParent.setCursor(CROSS_CURSOR);

                // Handle delta measurement key events
                if (plotArea != null && plotArea.contains(canvasX, canvasY)) {
                    char key = e.character;
                    if (deltaMeasurement.handleKeyPress(key, canvasX, canvasY, finalParent.getOptimizedCoordinateMapper())) {
                        finalParent.redraw();
                    }
                }
            }
            @Override
            public void keyReleased(KeyEvent e) {
                if (finalParent.getCursor() == null)
                    finalParent.setCursor(CROSS_CURSOR);
            }
        });
        parent.addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if ((e.stateMask & SWT.BUTTON_MASK) != 0) {
                    invalidatePosition();
                    finalParent.redraw();
                    return;
                }

                canvasX = e.x;
                canvasY = e.y;
                detailedTooltip = false;

                finalParent.redraw();  //XXX this is killer if parent is not cached. unfortunately, graphics.setXORMode() cannot be used

                // set cursor
                if (plotArea != null && plotArea.contains(canvasX,canvasY)) {
                    if (finalParent.getCursor() == null)
                        finalParent.setCursor(CROSS_CURSOR);
                }
                else
                    finalParent.setCursor(null);
            }
        });
        parent.addMouseTrackListener(new MouseTrackAdapter() {
            @Override
            public void mouseEnter(MouseEvent e) {
                invalidatePosition();
                finalParent.redraw();
            }
            @Override
            public void mouseExit(MouseEvent e) {
                invalidatePosition();
                finalParent.redraw();
            }
            @Override
            public void mouseHover(MouseEvent e) {
                detailedTooltip = true;
                finalParent.redraw();
            }
        });
        parent.addMouseListener(new MouseListener() {
            public void mouseDoubleClick(MouseEvent e) {
            }

            public void mouseDown(MouseEvent e) {
                invalidatePosition();
                finalParent.redraw();
            }

            public void mouseUp(MouseEvent e) {
                if (finalParent.getCursor() == null)
                    finalParent.setCursor(CROSS_CURSOR);

                canvasX = e.x;
                canvasY = e.y;
                detailedTooltip = false;
                finalParent.redraw();
            }
        });
        HoverSupport hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(500, 400);
        hoverSupport.adapt(parent, new IHoverInfoProvider() {
            @Override
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                return new HtmlHoverInfo(getHoverText(x, y, finalParent.getOptimizedCoordinateMapper()), (imageName) -> SymbolImageFactory.getImage(imageName));
            }
        });

        deltaMeasurement = new DeltaMeasurement(parent);
    }

    private void invalidatePosition() {
        canvasX = Integer.MAX_VALUE;
        canvasY = Integer.MAX_VALUE;
        detailedTooltip = false;
    }

    public Rectangle layout(Graphics graphics, Rectangle rect) {
        this.plotArea = rect;
        return rect;
    }

    public Point getCrosshairPosition() {
        return new Point(canvasX, canvasY);
    }

    public void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        if (plotArea != null) {
            graphics.pushState();
            // collect points close to cursor (x,y)
            ArrayList<DataPoint> dataPoints = new ArrayList<DataPoint>();
            int totalFound = parent.dataPointsNear(canvasX, canvasY, HALO, dataPoints, 1, coordsMapping);

            // snap to data point
            DataPoint dataPoint = dataPoints.isEmpty() ? null : dataPoints.get(0);
            if (dataPoint != null) {
                IXYDataset dataset = parent.getDataset();
                double x = parent.transformX(dataset.getX(dataPoint.series, dataPoint.index));
                double y = parent.transformY(dataset.getY(dataPoint.series, dataPoint.index));
                canvasX = (int)coordsMapping.toCanvasX(x);
                canvasY = (int)coordsMapping.toCanvasY(y);
            }

            // draw crosshair
            graphics.setLineDash(new int[] {3, 3});
            graphics.setLineWidth(1);
            graphics.setForegroundColor(ColorFactory.RED);
            graphics.drawLine(plotArea.x, canvasY, plotArea.x + plotArea.width, canvasY);
            graphics.drawLine(canvasX, plotArea.y, canvasX, plotArea.y + plotArea.height);

            // draw tooltip
            drawTooltip(graphics, dataPoints, totalFound, coordsMapping);

            // draw delta measurement markers and information
            deltaMeasurement.draw(graphics, coordsMapping);

            graphics.popState();
        }
    }

    private void drawTooltip(Graphics graphics, ArrayList<DataPoint> dataPoints, int totalFound, ICoordsMapping coordsMapping) {
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.setBackgroundColor(TOOLTIP_COLOR);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setFont(CROSSHAIR_NORMAL_FONT);

        if (dataPoints.isEmpty() || !detailedTooltip) {
            double x = parent.inverseTransformX(coordsMapping.fromCanvasX(canvasX));
            double y = parent.inverseTransformY(coordsMapping.fromCanvasY(canvasY));
            String coordinates = String.format("%g, %g", x, y); //FIXME precision: the x distance one pixel represents!
            Point size = GraphicsUtils.getTextExtent(graphics, coordinates);
            int left = canvasX + 3;
            int top = canvasY - size.y - 4;
            if (left + size.x + 3 > plotArea.x + plotArea.width)
                left = canvasX - size.x - 6;
            if (top < plotArea.y)
                top = canvasY + 3;
            graphics.fillRectangle(left, top, size.x + 2, size.y + 2);
            graphics.drawRectangle(left, top, size.x + 2, size.y + 2);
            graphics.drawText(coordinates, left + 2, top + 1); // XXX set as tooltip, rather than draw it on the canvas!
        }
    }

    private String getHoverText(int x, int y, ICoordsMapping coordsMapping) {
        ArrayList<DataPoint> dataPoints = new ArrayList<DataPoint>();
        int totalFound = parent.dataPointsNear(x, y, HALO, dataPoints, MAXCOUNT, coordsMapping);
        if (!dataPoints.isEmpty()) {
            Collections.sort(dataPoints, new Comparator<DataPoint> () {
                @Override
                public int compare(DataPoint dp1, DataPoint dp2) {
                    String key1 = parent.getDataset().getSeriesKey(dp1.series);
                    String key2 = parent.getDataset().getSeriesKey(dp2.series);
                    return StringUtils.dictionaryCompare(key1, key2);
                }
            });

            StringBuilder htmlText = new StringBuilder();

            for (DataPoint dp : dataPoints) {
                LineProperties props = parent.getLineProperties(dp.series);
                Color color = props.getEffectiveLineColor();
                String text = getText(dp);
                IPlotSymbol symbol = props.getSymbolPlotter();
                boolean hasLine = props.getEffectiveLineStyle() != LineStyle.None && props.getEffectiveDrawStyle() != DrawStyle.None;
                String imageName = SymbolImageFactory.prepareImage(symbol, color, hasLine);

                if (imageName != null)
                    htmlText.append(String.format("<img src='%s'>", imageName));

                htmlText.append(StringEscapeUtils.escapeHtml4(text));
                htmlText.append("<br/>");
            }
            if (totalFound > dataPoints.size())
                htmlText.append(String.format("... and %d more", totalFound - dataPoints.size()));

            return HoverSupport.addHTMLStyleSheet(htmlText.toString());
        }
        else
            return null;
    }

    private String getText(DataPoint dataPoint) {
        IXYDataset dataset = parent.getDataset();
        //String coordinates = String.format("%g, %g", dataset.getXValue(dataPoint.series, dataPoint.index), dataset.getYValue(dataPoint.series, dataPoint.index));
        int series = dataPoint.series;
        int index = dataPoint.index;

        String xStr = dataset.getXAsString(series, index);
        String yStr = dataset.getYAsString(series, index);

        String seriesStr = dataset.getSeriesTitle(series);

        return String.format("(%s; %s) - %s", xStr, yStr, seriesStr);
    }
}