/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.pychart.IChartProvider;

/**
 * Implements the methods in the "omnetpp.scave.chart" Python module. Grants
 * some access to the Chart object when executing its script in Python, and
 * the chart name suggested by the chart script. Also supports adding extra
 * properties to the returned properties map, that are not actually in the Chart.
 */
public class ChartProvider implements IChartProvider {
    /**
     * The Chart object being accessed.
     */
    private Chart chart;

    /**
     * Stores the last chart name suggested by the Python script.
     */
    private String suggestedChartName;

    /**
     * This map is appended to the properties of chart. Is useful for example to
     * pass in image and data export preferences to the chart script.
     */
    private Map<String, String> extraProperties;

    public ChartProvider(Chart chart) {
        this.chart = chart;
    }

    public ChartProvider(Chart chart, Map<String, String> extraProperties) {
        this.chart = chart;
        this.extraProperties = extraProperties;
    }

    @Override
    public Map<String, String> getChartProperties() {
        Map<String, String> props = new HashMap<>();

        for (Property prop : chart.getProperties())
            props.put(prop.getName(), prop.getValue());

        if (extraProperties != null)
            props.putAll(extraProperties);

        return props;
    }

    @Override
    public String getChartName() {
        return chart.getName();
    }

    @Override
    public String getChartType() {
        return chart.getType().toString().toLowerCase();
    }

    @Override
    public void setSuggestedChartName(String name) {
        suggestedChartName = name;
    }

    /**
     * Returns the chart name suggested by the chart script.
     * This is the only method (apart from the ctor) that is called from
     * Java, not Python.
     */
    public String getSuggestedChartName() {
        return suggestedChartName;
    }
}