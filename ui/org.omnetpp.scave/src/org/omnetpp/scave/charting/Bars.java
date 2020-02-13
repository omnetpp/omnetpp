/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.canvas.LargeRect;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.scave.charting.dataset.IGroupsSeriesDataset;
import org.omnetpp.scave.charting.properties.PlotProperty.BarPlacement;

/**
 * The content area of a bar plot.
 */
class Bars {
    private Rectangle rect = new Rectangle(0,0,1,1);

    double barBaseline;
    BarPlacement barPlacement;
    RGB barBaselineColor;

    // coordinates of the bars, in group/series order
    // groups are sorted according to the x coordinate
    // y coordinates might be logarithmic
    private RectangularArea[][] bars;

    private BarPlot parent;

    public Bars(BarPlot parent) {
        this.parent = parent;
    }

    public Rectangle getRectangle() {
        return rect;
    }

    public Rectangle layout(Graphics graphics, Rectangle rect) {
        this.rect = rect.getCopy();
        return rect;
    }

    public void draw(Graphics graphics, ICoordsMapping coordsMapping) {

        if (bars != null && bars.length > 0 && bars[0].length > 0) {
            graphics.pushState();

            Rectangle clip = graphics.getClip(new Rectangle());
            int numSeries = bars[0].length;
            int[] indices = getGroupSeriessInRectangle(clip);
            for (int i = indices[1]; i >= indices[0]; --i) {
                int group = i / numSeries;
                int series = i % numSeries;
                drawBar(graphics, group, series, coordsMapping);
            }
            graphics.popState();
        }
    }

    public void drawBaseline(Graphics graphics, ICoordsMapping coordsMapping) {
        double baseline = getTransformedBaseline();
        if (!Double.isInfinite(baseline)) {
            graphics.pushState();

            long y = coordsMapping.toCanvasY(baseline);
            graphics.setForegroundColor(new Color(null, barBaselineColor));
            LargeGraphics.drawLine(graphics, rect.x + 1, y, rect.x + rect.width - 1, y);

            graphics.popState();
        }
    }

    protected void drawBar(Graphics graphics, int group, int series, ICoordsMapping coordsMapping) {
        LargeRect rect = getBarRectangle(group, series, coordsMapping);
        rect.width = Math.max(rect.width, 1);
        rect.height = Math.max(rect.height, 1);
        graphics.setBackgroundColor(getBarColor(series));
        graphics.setForegroundColor(getBarOutlineColor(series));
        LargeGraphics.fillRectangle(graphics, rect.x, rect.y, rect.width, rect.height);
        if (rect.width >= 4 && rect.height >= 3) {
            LargeGraphics.drawRectangle(graphics, rect.x, rect.y, rect.width, rect.height);
        }
    }

    protected int[] getGroupSeriessInRectangle(org.eclipse.draw2d.geometry.Rectangle rect) {
        int[] result = new int[2];
        result[0] = getGroupSeries(rect.x, true);
        result[1] = getGroupSeries(rect.x + rect.width, false);
        return result;
    }

    private int getGroupSeries(double x, boolean before) {
        int numGroups = parent.getDataset().getGroupCount();
        int numSeries = parent.getDataset().getSeriesCount();
        return before ? 0 : (numGroups*numSeries-1);
    }

    public int findGroupSeries(double x, double y) {
        IGroupsSeriesDataset dataset = parent.getDataset();
        if (dataset == null || bars == null)
            return -1;
        int numGroups = parent.getDataset().getGroupCount();
        int numSeries = parent.getDataset().getSeriesCount();

        for (int group = 0; group < numGroups; ++group)
            // search seriess in Z-order
            for (int series = 0; series < numSeries; ++series)
                if (bars[group][series].contains(x, y))
                    return group * numSeries + series;

        return -1;
    }

    protected Color getBarColor(int series) {
        RGB color = parent.getEffectiveBarColor(parent.getKeyFor(series));
        return new Color(null, color);
    }

    protected Color getBarOutlineColor(int series) {
        RGB color = parent.getEffectiveBarOutlineColor(parent.getKeyFor(series));
        return new Color(null, color);
    }

    protected LargeRect getBarRectangle(int group, int series, ICoordsMapping coordsMapping) {
        RectangularArea bar = bars[group][series];
        double top =  bar.maxY;
        double bottom = bar.minY;
        double left = bar.minX;
        double right = bar.maxX;

        if (Double.isInfinite(top))
            top = top < 0.0 ? parent.chartArea.minY : parent.chartArea.maxY;
        if (Double.isInfinite(bottom))
            bottom = bottom < 0.0 ? parent.chartArea.minY : parent.chartArea.maxY;

        long x = coordsMapping.toCanvasX(left);
        long y = coordsMapping.toCanvasY(top);
        long width = coordsMapping.toCanvasDistX(right - left);
        long height = coordsMapping.toCanvasDistY(top - bottom);
        return new LargeRect(x, y, width, height);
    }

    protected RectangularArea calculatePlotArea() {
        final double widthBar = 1.0;
        final double hgapMinor = 0.5;
        final double hgapMajor = 0.25;
        final double horizontalInset = 1.0;   // left/right inset relative to the bars' width
        final double verticalInset = 0.1;   // top inset relative to the height of the highest bar

        IGroupsSeriesDataset dataset = parent.getDataset();
        bars = null;
        if (dataset == null)
            return new RectangularArea(0, 0, 1, 1);

        int numGroups = dataset.getGroupCount();
        int numSeries = dataset.getSeriesCount();
        double baseline = getTransformedBaseline();

        if (Double.isInfinite(baseline)) {
            double newBaseline = baseline < 0.0 ? Double.POSITIVE_INFINITY : Double.NEGATIVE_INFINITY;
            for (int group = 0; group < numGroups; ++group)
                for (int series = 0; series < numSeries; ++series) {
                    double value = transformValue(dataset.getValue(group, series));
                    if (!Double.isNaN(value) && !Double.isInfinite(value)) {
                        if (baseline < 0.0)
                            newBaseline = Math.min(newBaseline, value);
                        else
                            newBaseline = Math.max(newBaseline, value);
                    }
                }
            baseline = newBaseline;
            if (Double.isInfinite(baseline))
                return new RectangularArea(0, 0, 1, 1);
        }

        RectangularArea plotArea = new RectangularArea(0, baseline, 0, baseline);

        double x = horizontalInset * widthBar;
        double y = 0;
        bars = new RectangularArea[numGroups][];
        for (int group = 0; group < numGroups; ++group) {
            y = 0;
            bars[group] = new RectangularArea[numSeries];
            for (int series = 0; series < numSeries; ++series) {
                RectangularArea bar = bars[group][series] = new RectangularArea();
                double value = parent.getDataset().getValue(group, series);

                // calculate x coordinates
                switch (barPlacement) {
                case Aligned:
                    bar.minX = x;
                    bar.maxX = x + widthBar;
                    x += widthBar;
                    if (series < numSeries - 1)
                        x += hgapMinor;
                    else if (group < numGroups - 1)
                        x += hgapMajor * (widthBar * numSeries + (hgapMinor * (numSeries - 1)));
                    break;
                case Overlap:
                    bar.minX = x + widthBar * series / 2.0;
                    bar.maxX = bar.minX + widthBar * numSeries / 2.0;
                    if (series == numSeries - 1 && group < numGroups - 1)
                        x += widthBar * numSeries + hgapMajor * widthBar * numSeries;
                    break;
                case InFront:
                case Stacked:
                    bar.minX = x;
                    bar.maxX = x + widthBar * numSeries;
                    if (series == numSeries - 1 && group < numGroups - 1)
                        x += widthBar * numSeries + hgapMajor * widthBar * numSeries;
                    break;
                }

                // calculate y coordinates
                switch (barPlacement) {
                case Aligned:
                case InFront:
                case Overlap:
                    y = transformValue(value);
                    if (y > baseline) {
                        bar.minY = baseline;
                        bar.maxY = y;
                    }
                    else if (y < baseline) {
                        bar.minY = y;
                        bar.maxY = baseline;
                    }
                    else
                        bar.minY = bar.maxY = baseline;
                    break;
                case Stacked:
                    value = Math.abs(value);
                    bar.minY = transformValue(y);
                    bar.maxY = transformValue(y + value);
                    y += value;
                    break;
                }

                // extend plot area with the new bar
                addRectangle(plotArea, bar);
            }
        }

        if (plotArea.minY > plotArea.maxY)
            return new RectangularArea(0, 0, 1, 1);

        // set minimum height (empty plot, or each value == barBaseline)
        if (plotArea.minY == plotArea.maxY) {
            plotArea.minY -= 0.5;
            plotArea.maxY += 0.5;
        }
        // add vertical insets
        double height = plotArea.maxY - plotArea.minY;
        plotArea.maxX += horizontalInset * widthBar;
        if (plotArea.minY < baseline)
            plotArea.minY -= verticalInset * height;
        if (plotArea.maxY > baseline)
            plotArea.maxY += verticalInset * height;
        return plotArea;
    }

    private void addRectangle(RectangularArea area, RectangularArea rect) {
        if (!Double.isNaN(rect.minX) && !Double.isInfinite(rect.minX))
            area.minX = Math.min(area.minX, rect.minX);
        if (!Double.isNaN(rect.minY) && !Double.isInfinite(rect.minY))
            area.minY = Math.min(area.minY, rect.minY);
        if (!Double.isNaN(rect.maxX) && !Double.isInfinite(rect.maxX))
            area.maxX = Math.max(area.maxX, rect.maxX);
        if (!Double.isNaN(rect.maxY) && !Double.isInfinite(rect.maxY))
            area.maxY = Math.max(area.maxY, rect.maxY);
    }

    protected double transformValue(double y) {
        return parent.transformY(y);
    }

    protected double getTransformedBaseline() {
        double baseline = barPlacement == BarPlacement.Stacked ? 0.0 : barBaseline;
        baseline = transformValue(baseline);
        return Double.isNaN(baseline) ? Double.NEGATIVE_INFINITY : baseline;
    }
}

