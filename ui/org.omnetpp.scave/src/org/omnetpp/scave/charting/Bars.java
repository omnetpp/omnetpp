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
import org.omnetpp.scave.charting.dataset.IScalarDataset;
import org.omnetpp.scave.charting.properties.PlotDefaults;
import org.omnetpp.scave.charting.properties.PlotProperty.BarPlacement;

/**
 * The content area of a bar plot.
 */
class Bars {
    private Rectangle rect = new Rectangle(0,0,1,1);

    double barBaseline;
    BarPlacement barPlacement;

    // coordinates of the bars, in row/column order
    // rows are sorted according to the x coordinate
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
            int cColumns = bars[0].length;
            int[] indices = getRowColumnsInRectangle(clip);
            for (int i = indices[1]; i >= indices[0]; --i) {
                int row = i / cColumns;
                int column = i % cColumns;
                drawBar(graphics, row, column, coordsMapping);
            }
            graphics.popState();
        }
    }

    public void drawBaseline(Graphics graphics, ICoordsMapping coordsMapping) {
        double baseline = getTransformedBaseline();
        if (!Double.isInfinite(baseline)) {
            graphics.pushState();

            long y = coordsMapping.toCanvasY(baseline);
            graphics.setForegroundColor(PlotDefaults.DEFAULT_BAR_BASELINE_COLOR);
            LargeGraphics.drawLine(graphics, rect.x + 1, y, rect.x + rect.width - 1, y);

            graphics.popState();
        }
    }

    protected void drawBar(Graphics graphics, int row, int column, ICoordsMapping coordsMapping) {
        LargeRect rect = getBarRectangle(row, column, coordsMapping);
        rect.width = Math.max(rect.width, 1);
        rect.height = Math.max(rect.height, 1);
        graphics.setBackgroundColor(getBarColor(column));
        graphics.setForegroundColor(getBarOutlineColor(column));
        LargeGraphics.fillRectangle(graphics, rect.x, rect.y, rect.width, rect.height);
        if (rect.width >= 4 && rect.height >= 3) {
            LargeGraphics.drawRectangle(graphics, rect.x, rect.y, rect.width, rect.height);
        }
    }

    protected int[] getRowColumnsInRectangle(org.eclipse.draw2d.geometry.Rectangle rect) {
        int[] result = new int[2];
        result[0] = getRowColumn(rect.x, true);
        result[1] = getRowColumn(rect.x + rect.width, false);
        return result;
    }

    private int getRowColumn(double x, boolean before) {
        int cRows = parent.getDataset().getRowCount();
        int cColumns = parent.getDataset().getColumnCount();
        return before ? 0 : (cRows*cColumns-1);
    }

    public int findRowColumn(double x, double y) {
        IScalarDataset dataset = parent.getDataset();
        if (dataset == null || bars == null)
            return -1;
        int cRows = parent.getDataset().getRowCount();
        int cColumns = parent.getDataset().getColumnCount();

        for (int row = 0; row < cRows; ++row)
            // search columns in Z-order
            for (int column = 0; column < cColumns; ++column)
                if (bars[row][column].contains(x, y))
                    return row * cColumns + column;

        return -1;
    }

    protected Color getBarColor(int column) {
        RGB color = parent.getEffectiveBarColor(parent.getKeyFor(column));
        return new Color(null, color);
    }

    protected Color getBarOutlineColor(int column) {
        RGB color = parent.getEffectiveBarOutlineColor(parent.getKeyFor(column));
        return new Color(null, color);
    }

    protected LargeRect getBarRectangle(int row, int column, ICoordsMapping coordsMapping) {
        RectangularArea bar = bars[row][column];
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

        IScalarDataset dataset = parent.getDataset();
        bars = null;
        if (dataset == null)
            return new RectangularArea(0, 0, 1, 1);

        int cRows = dataset.getRowCount();
        int cColumns = dataset.getColumnCount();
        double baseline = getTransformedBaseline();

        if (Double.isInfinite(baseline)) {
            double newBaseline = baseline < 0.0 ? Double.POSITIVE_INFINITY : Double.NEGATIVE_INFINITY;
            for (int row = 0; row < cRows; ++row)
                for (int column = 0; column < cColumns; ++column) {
                    double value = transformValue(dataset.getValue(row, column));
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
        bars = new RectangularArea[cRows][];
        for (int row = 0; row < cRows; ++row) {
            y = 0;
            bars[row] = new RectangularArea[cColumns];
            for (int column = 0; column < cColumns; ++column) {
                RectangularArea bar = bars[row][column] = new RectangularArea();
                double value = parent.getDataset().getValue(row, column);

                // calculate x coordinates
                switch (barPlacement) {
                case Aligned:
                    bar.minX = x;
                    bar.maxX = x + widthBar;
                    x += widthBar;
                    if (column < cColumns - 1)
                        x += hgapMinor;
                    else if (row < cRows - 1)
                        x += hgapMajor * (widthBar * cColumns + (hgapMinor * (cColumns - 1)));
                    break;
                case Overlap:
                    bar.minX = x + widthBar * column / 2.0;
                    bar.maxX = bar.minX + widthBar * cColumns / 2.0;
                    if (column == cColumns - 1 && row < cRows - 1)
                        x += widthBar * cColumns + hgapMajor * widthBar * cColumns;
                    break;
                case InFront:
                case Stacked:
                    bar.minX = x;
                    bar.maxX = x + widthBar * cColumns;
                    if (column == cColumns - 1 && row < cRows - 1)
                        x += widthBar * cColumns + hgapMajor * widthBar * cColumns;
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

