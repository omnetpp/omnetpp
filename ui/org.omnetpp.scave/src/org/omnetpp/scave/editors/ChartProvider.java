package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.pychart.IChartProvider;

/**
 * Implements the methods in the "omnetpp.scave.chart" Python module.
 */
public class ChartProvider implements IChartProvider {
    private Chart chart;
    private String suggestedChartName;
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

    public String getSuggestedChartName() {
        return suggestedChartName;
    }
}