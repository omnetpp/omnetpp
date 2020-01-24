package org.omnetpp.scave.pychart;

import py4j.Py4JException;

public interface IPythonEntryPoint {

    // Only used to check that we have a working connection -
    // so the Python process got to the point where a Gateway
    // server is listening
    boolean check();

    void setResultsProvider(IScaveResultsPickleProvider resultsProvider);
    void setChartProvider(IChartProvider chartProvider);

    void setPlotWidgetProvider(IPlotWidgetProvider widgetProvider);
    void setNativeChartPlotter(INativeChartPlotter chartPlotter);

    // This can be used to set arbitrary data in the execution
    // context of the Python scripts.
    void setGlobalObjectPickle(String name, byte[] pickle);

    void execute(String command) throws Py4JException;
}
