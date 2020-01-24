package org.omnetpp.scave.pychart;

import java.util.Map;

public interface IChartPropertiesProvider {
    String getChartName();
    String getChartType();

    Map<String, String> getChartProperties();
    Map<String, String> getDefaultChartProperties(); // unused at the moment
    void setSuggestedChartName(String name);
}
