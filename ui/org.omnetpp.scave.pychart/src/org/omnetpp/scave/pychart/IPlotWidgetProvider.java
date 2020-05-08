/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.List;

import py4j.Py4JException;

public interface IPlotWidgetProvider {
    IMatplotlibWidget getWidget(int figureNumber, IMatplotlibFigureCanvas canvas) throws Py4JException;

    void setPlotActions(int figureNumber, List<ActionDescription> actions);

    void updateActiveAction(int figureNumber, String action);
    void setWarning(String warning);
}
