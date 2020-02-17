/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

/**
 * Property source for charts.
 * @author tomi
 */
public class PlotProperties {

    /**
     * Property names used in the model.
     */
    public static final String
        // Titles
        PROP_PLOT_TITLE         = "Plot.Title",
        PROP_PLOT_TITLE_FONT    = "Plot.Title.Font",
        PROP_X_AXIS_TITLE       = "X.Axis.Title",
        PROP_Y_AXIS_TITLE       = "Y.Axis.Title",
        PROP_AXIS_TITLE_FONT    = "Axis.Title.Font",
        PROP_LABEL_FONT         = "Label.Font",
        PROP_X_LABELS_ROTATE_BY = "X.Label.RotateBy",
        // Axes
        PROP_Y_AXIS_MIN         = "Y.Axis.Min",
        PROP_Y_AXIS_MAX         = "Y.Axis.Max",
        PROP_Y_AXIS_LOGARITHMIC = "Y.Axis.Log",
        PROP_XY_GRID            = "Axes.Grid",
        // Legend
        PROP_DISPLAY_LEGEND     = "Legend.Display",
        PROP_LEGEND_BORDER      = "Legend.Border",
        PROP_LEGEND_FONT        = "Legend.Font",
        PROP_LEGEND_POSITION    = "Legend.Position",
        PROP_LEGEND_ANCHORING   = "Legend.Anchoring",
        // Plot
        PROP_ANTIALIAS          = "Plot.Antialias",
        PROP_CACHING            = "Plot.Caching",
        PROP_BACKGROUND_COLOR   = "Plot.BackgroundColor";

    public enum LegendPosition {
        Inside,
        Above,
        Below,
        Left,
        Right,
    }

    public enum LegendAnchor {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
    }

    public enum ShowGrid {
        None,
        Major,
        All,
    }

}
