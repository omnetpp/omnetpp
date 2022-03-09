package org.omnetpp.scave.pychart;

/**
 * A common interface to show warning text on both native and Matplotlib charts,
 * and to add a problem marker (and error annotation) to the ChartScriptEditor.
 */
public interface IPlotWarningAnnotator {
    void setWarning(String warning);
    void setErrorMarkerAnnotation(int lineNumber, String message);
}