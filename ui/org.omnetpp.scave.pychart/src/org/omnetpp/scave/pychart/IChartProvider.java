package org.omnetpp.scave.pychart;

import java.util.Map;

/**
 * Implements the methods in the "omnetpp.scave.chart" Python module.
 */
public interface IChartProvider {
    /**
     * The method behind "chart.get_name()"
     */
    String getChartName();

    /**
     * The method behind "chart.get_chart_type()"
     */
    String getChartType();

    /**
     * The method behind "chart.get_properties()"
     */
    Map<String, String> getChartProperties();

    /**
     * The method behind "chart.set_suggested_chart_name()"
     */
    void setSuggestedChartName(String name);
}
