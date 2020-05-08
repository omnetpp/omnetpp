/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.pychart;

import java.util.Map;

/**
 * The methods in the omnetpp.scave.chart Python module are delegated through
 * this interface (and Py4J) to an implementation in Java.
 */
public interface IChartProvider {
    /**
     * The method behind "chart.get_name()"
     */
    String getChartName();

    /**
     * The method behind "chart.get_chart_type()"
     */
    String getChartType();

    /**
     * The method behind "chart.get_properties()"
     */
    Map<String, String> getChartProperties();

    /**
     * The method behind "chart.set_suggested_chart_name()"
     */
    void setSuggestedChartName(String name);
}
