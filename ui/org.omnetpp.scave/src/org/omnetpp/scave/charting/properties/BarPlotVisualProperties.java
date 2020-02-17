/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

public class BarPlotVisualProperties {

    public static final String
        // Titles
        PROP_WRAP_LABELS        = "X.Label.Wrap",
        // Bars
        PROP_BAR_BASELINE       = "Bars.Baseline",
        PROP_BAR_PLACEMENT      = "Bar.Placement";

    public enum BarPlacement {
        Aligned,
        Overlap,
        InFront,
        Stacked,
    }
}