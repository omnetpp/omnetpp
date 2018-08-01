/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 *
 */
package org.omnetpp.scave.charting;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.canvas.LargeGraphics;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.GraphicsUtils;
import org.omnetpp.scave.charting.VectorChartViewer.LineProperties;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;

/**
 * Draws the selection to a vector chart.
 *
 * @author tomi
 */
public class VectorChartSelection implements IChartSelection {

    private final VectorChartViewer vectorChart;

    // the series of the line within the chart's dataset
    private int series;
    // the key of the line within the chart
    private String key;
    // the id of the result item that the line represents
    private long id;
    // attributes of the selected data point
    private int index;
    private long eventNum;
    private BigDecimal preciseX;
    private double x,y;


    public VectorChartSelection(VectorChartViewer vectorChart, CrossHair.DataPoint point) {
        IXYDataset dataset = vectorChart.getDataset();
        this.vectorChart = vectorChart;
        this.series = point.series;
        this.key = dataset.getSeriesKey(series);
        this.id = dataset instanceof VectorDataset ? ((VectorDataset)dataset).getID(series) : -1L;
        this.index = point.index;
        this.eventNum = -1L; // TODO set eventNum
        this.preciseX = dataset.getPreciseX(series, index);
        this.x = dataset.getX(series, index);
        this.y = dataset.getY(series, index);
    }

    public int getSeries() {
        return series;
    }

    public String getSeriesKey() {
        return key;
    }

    public long getID() {
        return id;
    }

    public int getIndex() {
        return index;
    }

    public long getEventNum() {
        return eventNum;
    }

    public BigDecimal getPreciseX() {
        return preciseX;
    }

    public double getX() {
        return x;
    }

    public double getY() {
        return y;
    }

    protected void draw(Graphics graphics, ICoordsMapping coordsMapping) {
        LineProperties props = this.vectorChart.getLineProperties(series);
        if (props != null && props.getDisplayLine()) {
            long xx = coordsMapping.toCanvasX(this.vectorChart.transformX(x));
            long yy = coordsMapping.toCanvasY(this.vectorChart.transformY(y));
            Rectangle clipping = GraphicsUtils.getClip(graphics);
            org.eclipse.draw2d.geometry.Rectangle plotArea = vectorChart.getPlotRectangle();
            graphics.setClip(clipping.intersect(new Rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height)));
            graphics.setForegroundColor(ColorFactory.RED);
            graphics.setLineWidth(1);
            LargeGraphics.drawPoint(graphics, xx+5, yy);
            LargeGraphics.drawOval(graphics, xx-5, yy-5, 10, 10);
            graphics.setClip(clipping);
        }
    }
}