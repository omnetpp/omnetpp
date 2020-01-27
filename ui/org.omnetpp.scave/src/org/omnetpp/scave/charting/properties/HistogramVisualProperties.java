/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;

public class HistogramVisualProperties extends PropertySource {

    public static final String
        PROP_HIST_COLOR         = "Hist.Color";

    private final PlotProperties chartProps;
    private String histogramId;

    public HistogramVisualProperties(PlotProperties chartProps, String histogramId) {
        this.chartProps = chartProps;
        this.histogramId = histogramId;
    }

    private String propertyName(String baseName) {
        return chartProps.propertyName(baseName, histogramId);
    }

    @org.omnetpp.common.properties.Property(category="Histograms",id=PROP_HIST_COLOR,
            descriptorClass=ColorPropertyDescriptor.class,optional=true,
            description="Color of the bar. Color name or #RRGGBB. Press Ctrl+Space for a list of color names.")
    public String getHistColor() { return chartProps.getStringProperty(propertyName(PROP_HIST_COLOR)); } // FIXME use RGB
    public void setHistColor(String color) { chartProps.setProperty(propertyName(PROP_HIST_COLOR), color); }
    public String defaultHistColor() { return null; }
}