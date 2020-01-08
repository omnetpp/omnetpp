package org.omnetpp.scave.pychart;

import java.util.Map;

public interface IChartPropertiesProvider {
    String getChartName();

    Map<String, String> getChartProperties();
    Map<String, String> getDefaultChartProperties(); // unused at the moment

    // void setChartProperty(String key, String value); // this was moved to INativeChartPlotter

    void setMessage(String message);
}
