/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.List;

import py4j.Py4JException;

/**
 * An interface used by our matplotlib backend (from Python) to construct the
 * Java half of the custom FigureCanvas implementation.
 *
 * It also has some other methods regarding the user actions and warning text,
 * because we don't have a separate NavigationToolbar2 implementation.
 */
// TODO move the action and warning related methods to IMatplotlibWidget, or
// to a separate interface/class corresponding to NavigationToolbar2 in matplotlib
public interface IPlotWidgetProvider {
    IMatplotlibWidget getWidget(int figureNumber, IMatplotlibFigureCanvas canvas) throws Py4JException;

    void setPlotActions(int figureNumber, List<ActionDescription> actions);
    void updateActiveAction(int figureNumber, String action);

    void setWarning(String warning);
}
