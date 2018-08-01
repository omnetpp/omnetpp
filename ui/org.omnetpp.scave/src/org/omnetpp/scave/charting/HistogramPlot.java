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
import org.eclipse.swt.graphics.RGB;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.SquareSymbol;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.HistogramChartProperties.HistogramBar;
import org.omnetpp.scave.charting.properties.HistogramChartProperties.HistogramDataType;

class HistogramPlot {

    HistogramChartViewer canvas;
    Rectangle area = Rectangle.SINGLETON;

    HistogramBar barType = ChartDefaults.DEFAULT_HIST_BAR;
    boolean showOverflowCell = ChartDefaults.DEFAULT_SHOW_OVERFLOW_CELL;
    ICellValueTransform valueTransform;
    double baseline = 0.0;

    RectangularArea bars[][] = new RectangularArea[0][];
    double transformedBaseline;

    HistogramPlot(HistogramChartViewer canvas) {
        this.canvas = canvas;
        setHistogramData(ChartDefaults.DEFAULT_HIST_DATA);
    }

    Rectangle getArea() {
        return area;
    }

    void setBarType(HistogramBar barType) {
        this.barType = barType;
    }

    void setHistogramData(HistogramDataType histData) {
        switch (histData) {
        case Count: valueTransform = new NullTransform(); break;
        case Pdf: valueTransform = new PdfTransform(); break;
        case Cdf: valueTransform = new CdfTransform(); break;
        default: Assert.isLegal(false, "Unknown histogram data transformation: " + histData); break;
        }
    }

    protected RectangularArea calculatePlotArea() {
        IHistogramDataset dataset = canvas.getDataset();
        double minX = Double.POSITIVE_INFINITY, maxX = Double.NEGATIVE_INFINITY;
        double minY = 0.0, maxY = 0.0;

        int histCount = dataset.getSeriesCount();
        bars = new RectangularArea[histCount][];

        transformedBaseline = calculateBaseline();

        for (int series = 0; series < histCount; ++series) {
            int count = dataset.getCellsCount(series);
            double minValue = dataset.getMinValue(series);
            double maxValue = dataset.getMaxValue(series);
            boolean hasUnderflow = count > 0 && Double.isInfinite(dataset.getCellLowerBound(series, 0));
            boolean hasOverflow = count > 0 && Double.isInfinite(dataset.getCellUpperBound(series, count-1));
            int barCount = count - (hasUnderflow && !showOverflowCell ? 1 : 0) -
                                   (hasOverflow && !showOverflowCell ? 1 : 0);
            int i = 0;
            bars[series] = new RectangularArea[barCount];
            for (int index = 0; index < count; ++index) {
                if (!showOverflowCell && (index == 0 && hasUnderflow || index == count-1 && hasOverflow))
                    continue;
                double lowerBound = dataset.getCellLowerBound(series, index);
                double upperBound = dataset.getCellUpperBound(series, index);
                double value = getCellValue(series, index);
                double transformedValue = transformValue(value);
                double yTop = Math.max(transformedValue, transformedBaseline);
                double yBottom = Math.min(transformedValue, transformedBaseline);
                double xLeft = (index == 0 && Double.isInfinite(lowerBound)) ?
                                (minValue < upperBound ? minValue : upperBound) : lowerBound;
                double xRight = (index == count - 1 && Double.isInfinite(upperBound)) ?
                                (maxValue > lowerBound ? maxValue : lowerBound) : upperBound;

                bars[series][i++] = new RectangularArea(xLeft, yBottom, xRight, yTop);
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

    private double calculateBaseline() {
        double baseline = getTransformedBaseline();

        if (Double.isInfinite(baseline)) {
            IHistogramDataset dataset = canvas.getDataset();
            int histCount = dataset.getSeriesCount();

            double newBaseline = baseline < 0.0 ? Double.POSITIVE_INFINITY : Double.NEGATIVE_INFINITY;
            for (int series = 0; series < histCount; ++series) {
                int cellCount = dataset.getCellsCount(series);
                for (int column = 0; column < cellCount; ++column) {
                    double value = transformValue(getCellValue(series, column));
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

    void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        switch (barType) {
        case Solid:
            graphics.setForegroundColor(ColorFactory.BLACK);
            for (int series = 0; series < bars.length; ++series) {
                graphics.setBackgroundColor(getHistogramColor(series));
                for (int index = 0; index < bars[series].length; ++index) {
                    double xl = bars[series][index].minX;
                    double xr = bars[series][index].maxX;
                    double yt = bars[series][index].maxY;
                    double yb = bars[series][index].minY;

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
            }
            break;
        case Outline:
            graphics.setLineWidth(4);
            graphics.setLineStyle(SWT.LINE_SOLID);
            graphics.setAlpha(128);
            long baselineY = coordsMapping.toCanvasY(transformedBaseline);
            for (int series = 0; series < bars.length; ++series) {
                graphics.setForegroundColor(getHistogramColor(series));
                long prevY = baselineY;
                int cellCount = bars[series].length;
                ArrayList<Long> points = new ArrayList<Long>(3*cellCount);
                for (int index = 0 ; index < cellCount; ++index) {
                    double xl = bars[series][index].minX;
                    double xr = bars[series][index].maxX;
                    double yt = bars[series][index].maxY;
                    double yb = bars[series][index].minY;
                    double y = yt > transformedBaseline ? yt : yb;

                    long left = coordsMapping.toCanvasX(xl);
                    long right = coordsMapping.toCanvasX(xr);
                    long yy;
                    if (Double.isInfinite(y)) {
                        yy = baselineY;
                        if (yy != prevY) {
                            points.add(left);
                            points.add(prevY);
                            points.add(left);
                            points.add(yy);
                        }
                        LargeGraphics.drawPolyline(graphics, ArrayUtils.toPrimitive(points.toArray(new Long[0])));
                        points.clear();
                    }
                    else {
                        yy = coordsMapping.toCanvasY(y);
                        points.add(left);
                        points.add(prevY);
                        points.add(left);
                        points.add(yy);
                        points.add(right);
                        points.add(yy);
                    }
                    prevY = yy;
                }
                if (points.size() > 0)
                    LargeGraphics.drawPolyline(graphics, ArrayUtils.toPrimitive(points.toArray(new Long[0])));
            }
            break;
        }
    }

    void updateLegend(ILegend legend) {
        legend.clearItems();
        IHistogramDataset dataset = canvas.getDataset();
        IChartSymbol symbol = new SquareSymbol();
        for (int series = 0; series < dataset.getSeriesCount(); ++series) {
            legend.addItem(getHistogramColor(series), dataset.getSeriesKey(series), symbol, false);
        }
    }

    String getTooltipText(int x, int y) {
        Integer[] seriesAndIndeces = findHistogramColumns(x,y);
        if (seriesAndIndeces.length == 0)
            return null;
        IHistogramDataset dataset = canvas.getDataset();
        StringBuilder result = new StringBuilder();
        for (int i = 0; i < seriesAndIndeces.length; i+=2) {
            int series = seriesAndIndeces[i];
            int index = seriesAndIndeces[i+1];
            String key = dataset.getSeriesKey(series);
            double lowerBound = dataset.getCellLowerBound(series, index);
            double upperBound = dataset.getCellUpperBound(series, index);
            double value = getCellValue(series, index);
            boolean isIntegerType = dataset.isIntegerType(series);
            result.append(key + "<br>");
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
        double xx = canvas.fromCanvasX(x);
        double yy = canvas.fromCanvasY(y);
        IHistogramDataset dataset = canvas.getDataset();
        List<Integer> result = new ArrayList<Integer>();
        for (int series = 0; series < dataset.getSeriesCount(); ++series) {
            int index = findBin(bars[series], series, xx);
            if (index >= 0) {
                RectangularArea bar = bars[series][index];
                boolean isOver;
                switch (barType) {
                case Solid: isOver = bar.contains(xx, yy);  break;
                case Outline:
                    double dy = yy > transformedBaseline ? yy - bar.maxY : yy - bar.minY;
                    isOver = Math.abs(canvas.toCanvasDistY(dy)) <= 2;
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
        String key = canvas.getDataset().getSeriesKey(series);
        RGB color = canvas.getHistogramColor(key);
        if (color != null)
            return new Color(null, color);
        else
            return ColorFactory.getGoodDarkColor(series);
    }

    protected double transformValue(double y) {
        return canvas.transformY(y);
    }

    protected double getTransformedBaseline() {
        double baseline = this.baseline;
        baseline = transformValue(baseline);
        return Double.isNaN(baseline) ? Double.NEGATIVE_INFINITY : baseline;
    }

    private double getCellValue(int series, int index) {
        return valueTransform.getCellValue(series, index);
    }

    private interface ICellValueTransform {
        double getCellValue(int series, int index);
    }

    private class NullTransform implements ICellValueTransform {
        public double getCellValue(int series, int index) {
            return canvas.getDataset().getCellValue(series, index);
        }
    }

    private class PdfTransform implements ICellValueTransform {
        public double getCellValue(int series, int index) {
            IHistogramDataset dataset = canvas.getDataset();
            double value = dataset.getCellValue(series, index);
            long count = dataset.getValueCount(series);
            double cellMin = dataset.getCellLowerBound(series, index);
            if (Double.isInfinite(cellMin))
                cellMin = dataset.getMinValue(series);
            double cellMax = dataset.getCellUpperBound(series, index);
            if (Double.isInfinite(cellMax))
                cellMax = dataset.getMaxValue(series);
            double cellWidth = cellMax > cellMin ? cellMax - cellMin : 0.0;

            return cellWidth > 0.0 ? value / cellWidth / count : 0.0;
        }
    }

    private class CdfTransform implements ICellValueTransform {
        int prevSeries=-1, prevIndex=-1;
        long valueCount;
        double prevValue = 0.0;

        public double getCellValue(int series, int index) {
            IHistogramDataset dataset = canvas.getDataset();
            if (series != prevSeries) {
                prevValue = 0.0;
                prevIndex = -1;
                valueCount = dataset.getValueCount(series);
            }
            double value = prevValue;
            if (index > prevIndex) {
                for (int i = prevIndex + 1; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }
            else if (index < prevIndex / 2) {
                value = 0.0;
                for (int i = 0; i <= index; ++i)
                    value += dataset.getCellValue(series, i);
            }
            else if (index < prevIndex) {
                for (int i = prevIndex - 1; i >= index; --i)
                    value -= dataset.getCellValue(series, i);
            }

            prevSeries = series;
            prevIndex = index;
            prevValue = value;
            return valueCount > 0 ? value / valueCount : 0.0;
        }
    }
}
