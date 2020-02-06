package org.omnetpp.scave.pychart;

import java.util.Map;
import java.util.Set;

public interface INativeChartPlotter {
    void plotScalars(byte[] pickledData) throws UnsupportedOperationException;
    void plotVectors(byte[] pickledData, Map<String, String> props);
    void plotHistograms(byte[] pickledData);

    boolean isEmpty();

    void setProperty(String key, String value);
    void setProperties(Map<String, String> properties);

    void setWarning(String warning);

    Set<String> getSupportedPropertyKeys();
}
