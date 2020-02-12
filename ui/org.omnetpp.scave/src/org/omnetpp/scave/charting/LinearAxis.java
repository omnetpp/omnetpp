/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.math.BigDecimal;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.properties.PlotProperty.ShowGrid;

/**
 * Draws a (horizontal or vertical) chart axis, with the corresponding axis
 * title, ticks and tick labels, and (vertical or horizontal) grid lines.
 *
 * @author andras
 */
public class LinearAxis {
    /* Properties */
    private boolean vertical;    // horizontal or vertical axis
    private boolean logarithmic; // logarithmic axis
    private boolean drawAxisToPlot;
    private ShowGrid showGrid;
    private RGB gridColor;
    private boolean drawTickLabels = true;
    private boolean drawTitle = true;
    private String title;
    private Font titleFont;
    private Font tickFont;


    /* Layout parameters */
    private int gap = 2;  // space between axis line and plot area (usually 0)
    private int majorTickLength = 4;
    private int minorTickLength = 2;
    private Rectangle bounds;
    private Insets insets;  // plot area = bounds minus insets

    public LinearAxis(boolean vertical, boolean logarithmic, boolean drawAxisToPlot) {
        this.vertical = vertical;
        this.logarithmic = logarithmic;
        this.drawAxisToPlot = drawAxisToPlot;
    }

    public double transform(double coord) {
        return logarithmic ? Math.log10(coord) : coord;
    }

    public double inverseTransform(double coord) {
        return logarithmic ? Math.pow(10.0, coord) : coord;
    }

    /**
     * Modifies insets to accommodate room for axis title, ticks, tick labels etc.
     * Also returns insets for convenience.
     */
    public Insets layout(Graphics graphics, Rectangle bounds, Insets insets, ICoordsMapping mapping, int pass) {
        if (pass == 1) {
            if (vertical) {
                // postpone layoutHint() as it wants to use coordinate mapping which is not yet set up (to calculate ticks)
                insets.left = 0;
                insets.right = 0;
            }
            else {
                graphics.setFont(tickFont);
                int labelHeight = calculateTickLabelHeight(graphics);
                int titleHeight = calculateTitleSize(graphics).y;
                insets.top = Math.max(insets.top, gap + majorTickLength + labelHeight + 4);
                insets.bottom = Math.max(insets.bottom, gap + majorTickLength + labelHeight + titleHeight + 4);
            }
        }
        else if (pass == 2) {
            if (vertical) {
                graphics.setFont(tickFont);
                int labelWidth = calculateTickLabelLength(graphics, bounds, mapping);
                int titleHeight = calculateTitleSize(graphics).y;  // but will be drawn 90 deg rotated
                insets.left = Math.max(insets.left, gap + majorTickLength + labelWidth + titleHeight + 4);
                insets.right = Math.max(insets.right, gap + majorTickLength + labelWidth + 4);
            }
        }

        this.bounds = bounds.getCopy();
        this.insets = new Insets(insets);

        return insets;
    }

    private Point calculateTitleSize(Graphics graphics) {
        graphics.setFont(titleFont);
        return (!drawTitle || title.equals("")) ? new Point(0,0) : GraphicsUtils.getTextExtent(graphics, title);
    }

    private int calculateTickLabelHeight(Graphics graphics) {
        if (!drawTickLabels)
            return 0;
        graphics.setFont(tickFont);
        int labelHeight = GraphicsUtils.getTextExtent(graphics, "999").y;
        return labelHeight;
    }

    private int calculateTickLabelLength(Graphics graphics, Rectangle bounds, ICoordsMapping mapping) {
        // calculate longest tick label length
        if (!drawTickLabels)
            return 0;
        ITicks ticks = createTicks(bounds, mapping);
        int labelWidth = 0;
        if (ticks != null) {
            graphics.setFont(tickFont);
            for (BigDecimal tick : ticks) {
                if (ticks.isMajorTick(tick)) {
                    labelWidth = Math.max(labelWidth, GraphicsUtils.getTextExtent(graphics, tick.toPlainString()).x);
                }
            }
        }
        return labelWidth;
    }

    public void drawGrid(Graphics graphics, ICoordsMapping mapping) {
        if (showGrid == ShowGrid.None)
            return;

        // Note: when canvas caching is on, graphics is the cached image, so the grid must be drawn
        // to the whole clipping region (cached image area) not just the plot area
        Rectangle rect = GraphicsUtils.getClip(graphics);
        ITicks ticks = createTicks(rect, mapping);
        if (ticks != null) {
            graphics.pushState();
            graphics.setLineStyle(Graphics.LINE_DOT);
            graphics.setForegroundColor(new Color(Display.getDefault(), gridColor));
            for (BigDecimal tick : ticks) {
                if (showGrid == ShowGrid.All || ticks.isMajorTick(tick)) {
                    if (vertical) {
                        long y = mapping.toCanvasY(transform(tick.doubleValue()));
                        if (y >= rect.y && y <= rect.bottom()) {
                            LargeGraphics.drawLine(graphics, rect.x, y, rect.right(), y);
                        }
                    }
                    else {
                        long x = mapping.toCanvasX(transform(tick.doubleValue()));
                        if (x >= rect.x && x <= rect.right()) {
                            LargeGraphics.drawLine(graphics, x, rect.y, x, rect.bottom());
                        }
                    }
                }
            }
            graphics.popState();
        }
    }

    public void drawAxis(Graphics graphics, ICoordsMapping mapping) {
        Rectangle plotArea = bounds.getCopy().crop(insets);

        // draw axis line and title
        graphics.setLineWidth(1);
        graphics.setLineStyle(SWT.LINE_SOLID);
        graphics.setForegroundColor(ColorFactory.BLACK);
        graphics.setFont(titleFont);

        Point titleSize = GraphicsUtils.getTextExtent(graphics, title);
        if (vertical) {
            if (drawAxisToPlot && !logarithmic && mapping.fromCanvasY(plotArea.bottom()) < 0 && mapping.fromCanvasY(plotArea.y) > 0)
                LargeGraphics.drawLine(graphics, plotArea.x, mapping.toCanvasY(0), plotArea.right(), mapping.toCanvasY(0)); // x axis
            graphics.drawLine(plotArea.x - gap, plotArea.y, plotArea.x - gap, plotArea.bottom());
            graphics.drawLine(plotArea.right() + gap, plotArea.y, plotArea.right() + gap, plotArea.bottom());
            if (drawTitle) {
                // TODO: SVGGraphics does not support rotation yet
                boolean isSVGGraphics = GraphicsUtils.isSVGGraphics(graphics);
                if (!isSVGGraphics) {
                    graphics.pushState();
                    graphics.rotate(-90);
                    graphics.drawText(title, -(plotArea.y + plotArea.height / 2 + titleSize.x / 2), bounds.x);
                    graphics.popState();
                }
            }
        }
        else {
            if (drawAxisToPlot && !logarithmic && mapping.fromCanvasX(plotArea.x) < 0 && mapping.fromCanvasX(plotArea.right()) > 0)
                LargeGraphics.drawLine(graphics, mapping.toCanvasX(0), plotArea.y, mapping.toCanvasX(0), plotArea.bottom()); // y axis
            graphics.drawLine(plotArea.x, plotArea.y - gap, plotArea.right(), plotArea.y - gap);
            graphics.drawLine(plotArea.x, plotArea.bottom() + gap, plotArea.right(), plotArea.bottom() + gap);
            if (drawTitle)
                graphics.drawText(title, plotArea.x + plotArea.width / 2 - titleSize.x / 2, bounds.bottom() - titleSize.y);
        }

        // draw ticks and labels
        ITicks ticks = createTicks(plotArea, mapping);
        if (ticks != null) {
            graphics.setFont(tickFont);
            for (BigDecimal tick : ticks) {
                String label = tick.scale() < 0 ? tick.setScale(0).toString() : tick.toString();
                Point size = GraphicsUtils.getTextExtent(graphics, label);
                int tickLen = ticks.isMajorTick(tick) ? majorTickLength : minorTickLength;
                if (vertical) {
                    long y = mapping.toCanvasY(transform(tick.doubleValue()));
                    if (y >= plotArea.y && y <= plotArea.bottom()) {
                        LargeGraphics.drawLine(graphics, plotArea.x - gap - tickLen, y, plotArea.x - gap, y);
                        LargeGraphics.drawLine(graphics, plotArea.right() + gap + tickLen, y, plotArea.right() + gap, y);
                        if (drawTickLabels && ticks.isMajorTick(tick)) {
                            LargeGraphics.drawText(graphics, label, plotArea.x - gap - tickLen - size.x - 1, y - size.y / 2);
                            LargeGraphics.drawText(graphics, label, plotArea.right() + gap + tickLen + 3, y - size.y / 2);
                        }
                    }
                }
                else {
                    long x = mapping.toCanvasX(transform(tick.doubleValue()));
                    if (x >= plotArea.x && x <= plotArea.right()) {
                        LargeGraphics.drawLine(graphics, x, plotArea.y - gap - tickLen, x, plotArea.y - gap);
                        LargeGraphics.drawLine(graphics, x, plotArea.bottom() + gap + tickLen, x, plotArea.bottom() + gap);
                        if (drawTickLabels && ticks.isMajorTick(tick)) {
                            LargeGraphics.drawText(graphics, label, x - size.x / 2 + 1, plotArea.y - gap - tickLen - size.y - 1);
                            LargeGraphics.drawText(graphics, label, x - size.x / 2 + 1, plotArea.bottom() + gap + tickLen + 1);
                        }
                    }
                }
            }
        }
    }

    protected ITicks createTicks(Rectangle plotArea, ICoordsMapping mapping) {
        if (vertical)
            return createTicks(mapping.fromCanvasY(plotArea.bottom()), mapping.fromCanvasY(plotArea.y), plotArea.height);
        else
            return createTicks(mapping.fromCanvasX(plotArea.x), mapping.fromCanvasX(plotArea.right()), plotArea.width);
    }

    protected ITicks createTicks(double start, double end, int pixels) {
        if (logarithmic)
            return new LogarithmicTicks(Math.pow(10.0, start), Math.pow(10.0, end));
        else
            return pixels != 0 && start != end ? new LinearTicks(start, end, 50 * (end-start) / pixels ) : null;
    }

    public Rectangle getBounds() {
        return bounds;  // note: no setter! use setLayout()
    }

    public Insets getInsets() {
        return insets;  // note: no setter! use setLayout()
    }

    public boolean isVertical() {
        return vertical;
    }

    public void setVertical(boolean vertical) {
        this.vertical = vertical;
    }

    public boolean isLogarithmic() {
        return logarithmic;
    }

    public void setLogarithmic(boolean logarithmic) {
        this.logarithmic = logarithmic;
    }

    public int getGap() {
        return gap;
    }

    public void setGap(int gap) {
        this.gap = gap;
    }

    public Font getTickFont() {
        return tickFont;
    }

    public void setTickFont(Font tickFont) {
        this.tickFont = tickFont;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public Font getTitleFont() {
        return titleFont;
    }

    public void setTitleFont(Font titleFont) {
        this.titleFont = titleFont;
    }

    public ShowGrid getShowGrid() {
        return showGrid;
    }

    public void setShowGrid(ShowGrid showGrid) {
        this.showGrid = showGrid;
    }

    public void setGridColor(RGB color) {
        this.gridColor = color;
    }

    public boolean isDrawTickLabels() {
        return drawTickLabels;
    }

    public void setDrawTickLabels(boolean drawTickLabels) {
        this.drawTickLabels = drawTickLabels;
    }

    public boolean isDrawTitle() {
        return drawTitle;
    }

    public void setDrawTitle(boolean drawTitle) {
        this.drawTitle = drawTitle;
    }
}
