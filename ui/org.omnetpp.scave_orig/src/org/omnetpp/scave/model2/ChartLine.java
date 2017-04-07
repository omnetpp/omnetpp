/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;

/**
 * Data class to identify a line on a chart.
 * If the line represents the content of a vector,
 * the vector is also accessible from this class.
 *
 * @author tomi
 */
public class ChartLine {
    // the chart containing the line
    private Chart chart;
    // the series of the line within the chart's dataset
    private int series;
    // the key of the line within the chart
    private String key;
    // the reference to the result item that the line represents
    private ResultItemRef itemRef;

    public ChartLine(Chart chart, int series, String key, long id, ResultFileManager manager) {
        this.chart = chart;
        this.series = series;
        this.key = key;
        this.itemRef = id != -1L && manager != null ? new ResultItemRef(id, manager) : null;
    }

    public Chart getChart() {
        return chart;
    }

    public int getSeries() {
        return series;
    }

    public String getKey() {
        return key;
    }

    public ResultItemRef getResultItemRef() {
        return itemRef;
    }

    public String toString() {
        return "Line \""+key+"\"";
    }
}
