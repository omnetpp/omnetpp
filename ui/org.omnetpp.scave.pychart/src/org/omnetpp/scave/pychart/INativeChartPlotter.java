package org.omnetpp.scave.pychart;

import java.util.List;
import java.util.Map;
import java.util.Set;

public interface INativeChartPlotter {
    void plotScalars(byte[] pickledData, Map<String, String> props);
    void plotVectors(byte[] pickledData, Map<String, String> props);
    void plotHistograms(byte[] pickledData, Map<String, String> props);

    boolean isEmpty();

    void setProperty(String key, String value);
    void setProperties(Map<String, String> properties);

    void setGroupTitles(List<String> titles);

    void setWarning(String warning);

    Set<String> getSupportedPropertyKeys();
}
