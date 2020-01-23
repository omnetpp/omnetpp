package org.omnetpp.scave.pychart;

import java.util.Map;

public interface INativeChartPlotter {
    void plotScalars(byte[] pickledData) throws UnsupportedOperationException;
    void plotVectors(byte[] pickledData);
    void plotHistograms(byte[] pickledData);

    void setChartProperty(String key, String value); // this is asymmetric, but.....
    void setChartProperties(Map<String, String> properties);

    void setWarning(String warning);
}
