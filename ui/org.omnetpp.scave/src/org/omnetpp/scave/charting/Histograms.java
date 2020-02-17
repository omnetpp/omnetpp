/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.AbstractList;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.plotter.IPlotSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.properties.PlotProperty;

/**
 * The content area of a histogram plot.
 */
class Histograms {

    HistogramPlot parent;
    Rectangle area = Rectangle.SINGLETON;

    boolean showOverflowCell;
    double baseline;

    RectangularArea underflowBars[] = new RectangularArea[0];
    RectangularArea binBars[][] = new RectangularArea[0][];
    RectangularArea overflowBars[] = new RectangularArea[0];

    double transformedBaseline;

    Histograms(HistogramPlot parent) {
        this.parent = parent;
        calculatePlotArea();
    }

    Rectangle getArea() {
        return area;
    }

    protected RectangularArea calculatePlotArea() {
        IHistogramDataset dataset = parent.getDataset();
        double minX = Double.POSITIVE_INFINITY, maxX = Double.NEGATIVE_INFINITY;
        double minY = 0.0, maxY = 0.0;

        int histCount = dataset.getSeriesCount();
        underflowBars = new RectangularArea[histCount];
        binBars = new RectangularArea[histCount][];
        overflowBars = new RectangularArea[histCount];

        transformedBaseline = calculateBaseline();

        for (int series = 0; series < histCount; ++series) {
            ICellValueTransform valueTransform = getEffectiveValueTransform(series);
            int barCount = dataset.getCellsCount(series);
            double minValue = dataset.getMinValue(series);
            double maxValue = dataset.getMaxValue(series);
            boolean hasUnderflow = barCount > 0 && (valueTransform.getUnderflowsValue(series) > 0 || minValue < dataset.getCellLowerBound(series, 0));
            boolean hasOverflow = barCount > 0 && (valueTransform.getOverflowsValue(series) > 0 || maxValue >= dataset.getCellUpperBound(series, barCount-1));

            // underflow cell
            if (showOverflowCell && hasUnderflow) {
                double transformedValue = transformValue(valueTransform.getUnderflowsValue(series));
                double yTop = Math.max(transformedValue, transformedBaseline);
                double yBottom = Math.min(transformedValue, transformedBaseline);
                double xLeft = minValue;
                double xRight = dataset.getCellLowerBound(series, 0);
                underflowBars[series] = new RectangularArea(xLeft, yBottom, xRight, yTop);

                minX = Math.min(minX, xLeft);
                maxX = Math.max(maxX, xRight);
                if (!Double.isNaN(yTop))
                    maxY = Math.max(maxY, yTop);
                if (!Double.isNaN(yBottom) && !Double.isInfinite(yBottom))
                    minY = Math.min(minY, yBottom);
            }

            // regular bins
            binBars[series] = new RectangularArea[barCount];
            for (int index = 0; index < barCount; ++index) {
                double lowerBound = dataset.getCellLowerBound(series, index);
                double upperBound = dataset.getCellUpperBound(series, index);
                double value = valueTransform.getCellValue(series, index);
                double transformedValue = transformValue(value);
                double yTop = Math.max(transformedValue, transformedBaseline);
                double yBottom = Math.min(transformedValue, transformedBaseline);
                double xLeft = lowerBound;
                double xRight = upperBound;

                binBars[series][index] = new RectangularArea(xLeft, yBottom, xRight, yTop);
                minX = Math.min(minX, xLeft);
                maxX = Math.max(maxX, xRight);
                if (!Double.isNaN(yTop))
                    maxY = Math.max(maxY, yTop);
                if (!Double.isNaN(yBottom) && !Double.isInfinite(yBottom))
                    minY = Math.min(minY, yBottom);
            }

            // overflow cell
            if (showOverflowCell && hasOverflow) {
                double transformedValue = transformValue(valueTransform.getOverflowsValue(series));
                double yTop = Math.max(transformedValue, transformedBaseline);
                double yBottom = Math.min(transformedValue, transformedBaseline);
                double xLeft = dataset.getCellUpperBound(series, barCount-1);
                double xRight = dataset.getMaxValue(series);
                overflowBars[series] = new RectangularArea(xLeft, yBottom, xRight, yTop);

                minX = Math.min(minX, xLeft);
                maxX = Math.max(maxX, xRight);
                if (!Double.isNaN(yTop))
                    maxY = Math.max(maxY, yTop);
                if (!Double.isNaN(yBottom) && !Double.isInfinite(yBottom))
                    minY = Math.min(minY, yBottom);
            }
        }

        if (minX >= maxX || minY >= maxY) // empty
            return new RectangularArea(0.0, 0.0, 1.0, 1.0);

        double width = maxX - minX;
        double height = maxY - minY;
        minX = minX - width/10;
        maxX = maxX + width/10;
        //minY = (minY>=0 ? 0 : minY-height/3);
        //maxY = (maxY<=0 ? 0 : maxY+height/3); // not ok for logarithmic cdf where maxY=0.0
        maxY = maxY + height / 3;

        return new RectangularArea(minX, minY, maxX, maxY);
    }

    private ICellValueTransform getEffectiveValueTransform(int series) {
        String key = parent.getDataset().getSeriesKey(series);
        boolean cumulative = parent.getOrCreateHistogramProperties(key).getEffectiveCumulative();
        boolean density = parent.getOrCreateHistogramProperties(key).getEffectiveDensity();

        if (cumulative && density)
            return new CdfTransform();
        else if (!cumulative && density)
            return new PdfTransform();
        else if (!cumulative && !density)
            return new NullTransform();
        else
            return new AccTransform();
}

    private double calculateBaseline() {
        double baseline = getTransformedBaseline();

        if (Double.isInfinite(baseline)) {
            IHistogramDataset dataset = parent.getDataset();
            int histCount = dataset.getSeriesCount();

            double newBaseline = baseline < 0.0 ? Double.POSITIVE_INFINITY : Double.NEGATIVE_INFINITY;
            for (int series = 0; series < histCount; ++series) {
                ICellValueTransform valueTransform = getEffectiveValueTransform(series);
                int cellCount = dataset.getCellsCount(series);
                for (int column = 0; column < cellCount; ++column) {
                    double value = transformValue(valueTransform.getCellValue(series, column));
                    if (!Double.isNaN(value) && !Double.isInfinite(value)) {
                        if (baseline < 0.0)
                            newBaseline = Math.min(newBaseline, value);
                        else
                            newBaseline = Math.max(newBaseline, value);
                    }
                }
            }
            baseline = newBaseline;
        }

        return baseline;
    }

    Rectangle layout(Graphics graphics, Rectangle area) {
        Assert.isNotNull(area);
        this.area = area.getCopy();
        return area;
    }

    // utility method for the Solid draw style
    protected void drawRect(Graphics graphics, ICoordsMapping coordsMapping, RectangularArea area) {
        double xl = area.minX;
        double xr = area.maxX;
        double yt = area.maxY;
        double yb = area.minY;

        if (!Double.isInfinite(yt) && !Double.isInfinite(yb)) {
            long left = coordsMapping.toCanvasX(xl);
            long right = coordsMapping.toCanvasX(xr);
            long bottom = coordsMapping.toCanvasY(yb);
            long top = coordsMapping.toCanvasY(yt);
            if (top != bottom) {
                LargeGraphics.fillRectangle(graphics, left, top, right-left, bottom-top);
                LargeGraphics.drawRectangle(graphics, left, top, right-left, bottom-top);
            }
            else {
                LargeGraphics.drawLine(graphics, left, top, right, top);
            }
        }
    }

    void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        graphics.setLineStyle(SWT.LINE_SOLID);

        for (int series = 0; series < binBars.length; ++series) {
            String key = parent.getDataset().getSeriesKey(series);
            PlotProperty.HistogramBar barType = parent.getBarType(key);
            switch (barType) {
            case Solid:
                graphics.setLineWidth(1);
                graphics.setForegroundColor(ColorFactory.BLACK);
                graphics.setBackgroundColor(getHistogramColor(series));

                graphics.setAlpha(128);
                graphics.setLineStyle(SWT.LINE_DOT);

                if (underflowBars[series] != null)
                    drawRect(graphics, coordsMapping, underflowBars[series]);

                graphics.setAlpha(255);
                graphics.setLineStyle(SWT.LINE_SOLID);
                for (int index = 0; index < binBars[series].length; ++index)
                    drawRect(graphics, coordsMapping, binBars[series][index]);

                graphics.setAlpha(128);
                graphics.setLineStyle(SWT.LINE_DOT);

                if (overflowBars[series] != null)
                    drawRect(graphics, coordsMapping, overflowBars[series]);

                break;
            case Outline:
                graphics.setLineWidth(2);
                graphics.setForegroundColor(getHistogramColor(series));
                graphics.setAlpha(192);

                long baselineY = coordsMapping.toCanvasY(transformedBaseline);
                long prevY = baselineY;

                if (underflowBars[series] != null ) {
                    double xl = underflowBars[series].minX;
                    double xr = underflowBars[series].maxX;
                    double yt = underflowBars[series].maxY;
                    double yb = underflowBars[series].minY;
                    double y1 = transformedBaseline;
                    double y2 = yt > transformedBaseline ? yt : yb;

                    double nextYt = binBars[series][0].maxY;
                    double nextYb = binBars[series][0].minY;
                    long nextY = coordsMapping.toCanvasY(nextYt > transformedBaseline ? nextYt : nextYb);

                    long left = coordsMapping.toCanvasX(xl);
                    long right = coordsMapping.toCanvasX(xr);
                    graphics.setLineStyle(Graphics.LINE_DASH);

                    long cy1 = coordsMapping.toCanvasY(y1);
                    long cy2 = coordsMapping.toCanvasY(y2);

                    prevY = cy2;

                    LargeGraphics.drawPolyline(graphics, new long[] {
                            left, cy1,
                            left, cy2,
                            right, cy2,
                            right, nextY,
                            });

                }


                graphics.setLineStyle(Graphics.LINE_SOLID);

                int cellCount = binBars[series].length;
                ArrayList<Long> points = new ArrayList<Long>(3*cellCount);
                for (int index = 0 ; index < cellCount; ++index) {
                    double xl = binBars[series][index].minX;
                    double xr = binBars[series][index].maxX;
                    double yt = binBars[series][index].maxY;
                    double yb = binBars[series][index].minY;
                    double y = yt > transformedBaseline ? yt : yb;

                    long left = coordsMapping.toCanvasX(xl);
                    long right = coordsMapping.toCanvasX(xr);
                    long yy;
                    if (Double.isInfinite(y)) {
                        yy = baselineY;
                        if (yy != prevY) {
                            if (index != 0) {
                                points.add(left);
                                points.add(prevY);
                            }
                            points.add(left);
                            points.add(yy);
                        }
                        LargeGraphics.drawPolyline(graphics, ArrayUtils.toPrimitive(points.toArray(new Long[0])));
                        points.clear();
                    }
                    else {
                        yy = coordsMapping.toCanvasY(y);
                        if (index != 0) {
                            points.add(left);
                            points.add(prevY);
                        }
                        points.add(left);
                        points.add(yy);
                        points.add(right);
                        points.add(yy);
                    }
                    prevY = yy;
                }
                if (points.size() > 0)
                    LargeGraphics.drawPolyline(graphics, ArrayUtils.toPrimitive(points.toArray(new Long[0])));


                if (overflowBars[series] != null ) {
                    double xl = overflowBars[series].minX;
                    double xr = overflowBars[series].maxX;
                    double yt = overflowBars[series].maxY;
                    double yb = overflowBars[series].minY;
                    double y1 = yt > transformedBaseline ? yt : yb;
                    double y2 = transformedBaseline;

                    long left = coordsMapping.toCanvasX(xl);
                    long right = coordsMapping.toCanvasX(xr);
                    long cy1 = coordsMapping.toCanvasY(y1);
                    long cy2 = coordsMapping.toCanvasY(y2);
                    graphics.setLineStyle(Graphics.LINE_DASH);
                    LargeGraphics.drawPolyline(graphics, new long[] {
                            left, prevY,
                            left, cy1,
                            right, cy1,
                            right, cy2,
                            });

                }


                break;
            }
        }
    }

    void updateLegend(ILegend legend) {
        legend.clearItems();
        IHistogramDataset dataset = parent.getDataset();
        IPlotSymbol symbol = new SquareSymbol(6);
        for (int series = 0; series < dataset.getSeriesCount(); ++series) {
            String key = parent.getDataset().getSeriesKey(series);
            switch (parent.getBarType(key)) {
            case Outline:
                legend.addItem(getHistogramColor(series), dataset.getSeriesTitle(series), null, true);
                break;
            case Solid:
                legend.addItem(getHistogramColor(series), dataset.getSeriesTitle(series), symbol, false);
                break;
            }
        }
    }

    String getTooltipText(int x, int y) {
        Integer[] seriesAndIndeces = findHistogramColumns(x,y);
        if (seriesAndIndeces.length == 0)
            return null;
        IHistogramDataset dataset = parent.getDataset();
        StringBuilder result = new StringBuilder();
        for (int i = 0; i < seriesAndIndeces.length; i+=2) {
            int series = seriesAndIndeces[i];
            int index = seriesAndIndeces[i+1];
            String title = dataset.getSeriesTitle(series);
            double lowerBound = dataset.getCellLowerBound(series, index);
            double upperBound = dataset.getCellUpperBound(series, index);
            ICellValueTransform valueTransform = getEffectiveValueTransform(series);
            double value = valueTransform.getCellValue(series, index);
            boolean isIntegerType = dataset.isIntegerType(series);
            result.append(title + "<br>");
            result.append("[" + toIntegerAwareString(lowerBound, isIntegerType) + ", ");
            if (isIntegerType)
                result.append(toIntegerAwareString(upperBound - 1, isIntegerType) + "]");
            else
                result.append(String.valueOf(upperBound) + ")");
            result.append(" : " + toIntegerAwareString(value, true));
            if (i != seriesAndIndeces.length - 1)
                result.append("<br>");
        }
        return result.toString();
    }

    protected String toIntegerAwareString(double value, boolean isIntegerType) {
        if (!isIntegerType || Double.isInfinite(value) || Double.isNaN(value) || Math.floor(value) != value)
            return String.valueOf(value);
        else
            return String.valueOf((long)value);
    }

    private Integer[] findHistogramColumns(int x, int y) {
        double xx = parent.fromCanvasX(x);
        double yy = parent.fromCanvasY(y);
        IHistogramDataset dataset = parent.getDataset();
        List<Integer> result = new ArrayList<Integer>();
        for (int series = 0; series < dataset.getSeriesCount(); ++series) {
            String key = parent.getDataset().getSeriesKey(series);
            PlotProperty.HistogramBar barType = parent.getBarType(key);
            int index = findBin(binBars[series], series, xx);
            if (index >= 0) {
                RectangularArea bar = binBars[series][index];
                boolean isOver;
                switch (barType) {
                case Solid: isOver = bar.contains(xx, yy);  break;
                case Outline:
                    double dy = yy > transformedBaseline ? yy - bar.maxY : yy - bar.minY;
                    isOver = Math.abs(parent.toCanvasDistY(dy)) <= 2;
                    break;
                default: isOver = false; Assert.isTrue(false, "Unknown HistogramBar type: " + barType); break;
                }
                if (isOver) {
                    if (!showOverflowCell && Double.isInfinite(dataset.getCellLowerBound(series, 0)))
                        ++index;
                    result.add(series);
                    result.add(index);
                }
            }
        }
        return result.toArray(new Integer[result.size()]);
    }

    private static int findBin(final RectangularArea[] bars, final int series, double x) {
        List<Double> binsList = new AbstractList<Double>() {
            @Override public Double get(int index) { return bars[index].minX; }
            @Override public int size()            { return bars.length; }
        };
        int index = Collections.binarySearch(binsList, x);
        if (index >= 0)
            return index;
        index = -index-1;
        if (0 < index && index <= bars.length && x <= bars[index-1].maxX)
            return index-1;
        else
            return -1;
    }

    protected Color getHistogramColor(int series) {
        String key = parent.getDataset().getSeriesKey(series);
        return new Color(null, parent.getHistogramColor(key));
    }

    protected double transformValue(double y) {
        return parent.transformY(y);
    }

    protected double getTransformedBaseline() {
        double baseline = this.baseline;
        baseline = transformValue(baseline);
        return Double.isNaN(baseline) ? Double.NEGATIVE_INFINITY : baseline;
    }

    private interface ICellValueTransform {
        double getCellValue(int series, int index);
        double getUnderflowsValue(int series);
        double getOverflowsValue(int series);
    }

    private class NullTransform implements ICellValueTransform {
        public double getCellValue(int series, int index) {
            return parent.getDataset().getCellValue(series, index);
        }

        @Override
        public double getUnderflowsValue(int series) {
            return parent.getDataset().getUnderflows(series);
        }

        @Override
        public double getOverflowsValue(int series) {
            return parent.getDataset().getOverflows(series);
        }
    }

    private class PdfTransform implements ICellValueTransform {
        public double getCellValue(int series, int index) {
            IHistogramDataset dataset = parent.getDataset();
            double value = dataset.getCellValue(series, index);
            double sumWeights = dataset.getSumWeights(series);
            double cellMin = dataset.getCellLowerBound(series, index);
            double cellMax = dataset.getCellUpperBound(series, index);
            double cellWidth = cellMax > cellMin ? cellMax - cellMin : 0.0;
            return cellWidth > 0.0 ? value / cellWidth / sumWeights : 0.0;
        }

        @Override
        public double getUnderflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double value = dataset.getUnderflows(series);
            double sumWeights = dataset.getSumWeights(series);
            double cellMin = dataset.getMinValue(series);
            double cellMax = dataset.getCellLowerBound(series, 0);
            double cellWidth = cellMax > cellMin ? cellMax - cellMin : 0.0;
            return cellWidth > 0.0 ? value / cellWidth / sumWeights : 0.0;
        }

        @Override
        public double getOverflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double value = dataset.getOverflows(series);
            double sumWeights = dataset.getSumWeights(series);
            int cellsCount = dataset.getCellsCount(series);
            double cellMin = dataset.getCellUpperBound(series, cellsCount-1);
            double cellMax = dataset.getMaxValue(series);
            double cellWidth = cellMax > cellMin ? cellMax - cellMin : 0.0;
            return cellWidth > 0.0 ? value / cellWidth / sumWeights : 0.0;
        }
    }

    private class CdfTransform implements ICellValueTransform {
        int prevSeries=-1, prevIndex=-1; // for some caching
        double sumWeights;
        double prevValue = 0.0;

        public double getCellValue(int series, int index) {
            IHistogramDataset dataset = parent.getDataset();
            if (series != prevSeries) {
                prevValue = dataset.getUnderflows(series);
                prevIndex = -1;
                sumWeights = dataset.getSumWeights(series);
            }
            double value = prevValue;
            if (index >= prevIndex) {
                // optimization, to make the usual sequential querying faster
                for (int i = prevIndex + 1; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }
            else {
                // starting from the beginning for non-sequential querying
                value = dataset.getUnderflows(series);
                for (int i = 0; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }

            prevSeries = series;
            prevIndex = index;
            prevValue = value;
            return sumWeights > 0 ? value / sumWeights : 0.0;
        }

        @Override
        public double getUnderflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double sumWeights = dataset.getSumWeights(series);
            return sumWeights > 0 ? dataset.getUnderflows(series) / sumWeights : 0.0;
        }

        @Override
        public double getOverflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double sumWeights = dataset.getSumWeights(series);
            return sumWeights > 0 ? 1.0 : 0.0;
        }
    }

    private class AccTransform implements ICellValueTransform {
        int prevSeries=-1, prevIndex=-1; // for some caching
        double sumWeights;
        double prevValue = 0.0;

        public double getCellValue(int series, int index) {
            IHistogramDataset dataset = parent.getDataset();
            if (series != prevSeries) {
                prevValue = dataset.getUnderflows(series);
                prevIndex = -1;
                sumWeights = dataset.getSumWeights(series);
            }
            double value = prevValue;
            if (index >= prevIndex) {
                // optimization, to make the usual sequential querying faster
                for (int i = prevIndex + 1; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }
            else {
                // starting from the beginning for non-sequential querying
                value = dataset.getUnderflows(series);
                for (int i = 0; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }

            prevSeries = series;
            prevIndex = index;
            prevValue = value;
            return sumWeights > 0 ? value : 0.0;
        }

        @Override
        public double getUnderflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double sumWeights = dataset.getSumWeights(series);
            return sumWeights > 0 ? dataset.getUnderflows(series) : 0.0;
        }

        @Override
        public double getOverflowsValue(int series) {
            IHistogramDataset dataset = parent.getDataset();
            double sumWeights = dataset.getSumWeights(series);
            return sumWeights > 0 ? sumWeights : 0.0;
        }
    }
}
