package org.omnetpp.scave.pychart;

import java.util.Map;

public interface IChartPropertiesProvider {
    String getChartName();

    Map<String, String> getChartProperties();
    Map<String, String> getDefaultChartProperties();

    // void setChartProperty(String key, String value); // this was moved to INativeChartPlotter
}
