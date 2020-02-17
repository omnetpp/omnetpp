/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

public class HistogramPlotProperties {

    public static final String
        PROP_HIST_BAR           = "Hist.Bar",
        PROP_HIST_DATA          = "Hist.Data",
        PROP_SHOW_OVERFLOW_CELL = "Hist.ShowOverflowCell",
        PROP_BAR_BASELINE       = "Bars.Baseline";

    public enum HistogramBar {
        Solid,
        Outline,
    }

    public enum HistogramDataType {
        Count("count"),
        Pdf("probability density"),
        Cdf("cumulative density");

        private String displayName;

        private HistogramDataType(String displayName) {
            this.displayName = displayName;
        }

        @Override public String toString() {
            return displayName;
        }
    }
}