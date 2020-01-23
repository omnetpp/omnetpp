package org.omnetpp.scave.pychart;

import java.util.Map;

public interface INativeChartPlotter {
    void plotScalars(byte[] pickledData) throws UnsupportedOperationException;
    void plotVectors(byte[] pickledData);
    void plotHistograms(byte[] pickledData);

    void setProperty(String key, String value);
    void setProperties(Map<String, String> properties);

    void setWarning(String warning);
}
