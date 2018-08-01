package org.omnetpp.scave.pychart;

import java.util.List;

import py4j.Py4JException;

public interface IPlotWidgetProvider {
    IPlotWidget getWidget(int figureNumber, IPyFigureCanvas canvas) throws Py4JException;

    void setPlotActions(int figureNumber, List<ActionDescription> actions);

    void updateActiveAction(int figureNumber, String action);
}
