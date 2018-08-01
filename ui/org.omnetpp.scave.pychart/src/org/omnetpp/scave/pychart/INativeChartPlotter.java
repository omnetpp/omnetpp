package org.omnetpp.scave.pychart;

public interface INativeChartPlotter {
    void plotScalars(byte[] pickledData) throws UnsupportedOperationException;
    void plotVectors(byte[] pickledData);
    void plotHistograms(byte[] pickledData);

    void setChartProperty(String key, String value); // this is asymmetric, but.....
}
