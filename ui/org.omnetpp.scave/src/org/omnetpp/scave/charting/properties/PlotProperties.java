/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.scave.ScaveImages;

/**
 * Property names and types plots.
 */
public class PlotProperties {

    /**
     * Property names for plots.
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
    PROP_BACKGROUND_COLOR   = "Plot.BackgroundColor",
    // Bar Plot
    PROP_WRAP_LABELS        = "X.Label.Wrap",
    PROP_BAR_PLACEMENT      = "Bar.Placement",
    PROP_BAR_COLOR          = "Bar.Color",
    PROP_BAR_BASELINE       = "Bars.Baseline",
    // Histograms
    PROP_HIST_BAR           = "Hist.Bar",
    PROP_SHOW_OVERFLOW_CELL = "Hist.ShowOverflowCell",
    PROP_HIST_COLOR         = "Hist.Color",
    PROP_HIST_CUMULATIVE    = "Hist.Cumulative",
    PROP_HIST_DENSITY       = "Hist.Density",
    // Line Plot
    PROP_X_AXIS_MIN         = "X.Axis.Min",
    PROP_X_AXIS_MAX         = "X.Axis.Max",
    PROP_X_AXIS_LOGARITHMIC = "X.Axis.Log",
    // Lines
    PROP_DISPLAY_LINE       = "Line.Display",
    PROP_LINE_DRAW_STYLE    = "Line.DrawStyle",
    PROP_LINE_COLOR         = "Line.Color",
    PROP_LINE_STYLE         = "Line.Style",
    PROP_LINE_WIDTH         = "Line.Width",
    PROP_SYMBOL_TYPE        = "Symbols.Type",
    PROP_SYMBOL_SIZE        = "Symbols.Size";


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

    public enum BarPlacement {
        Aligned,
        Overlap,
        InFront,
        Stacked,
    }

    public enum HistogramBar {
        Solid,
        Outline,
    }

    public enum SymbolType {
        None("None", ScaveImages.IMG_OBJ16_SYM_NONE),
        Point("Point", ScaveImages.IMG_OBJ16_SYM_POINT),
        Cross("Cross", ScaveImages.IMG_OBJ16_SYM_CROSS),
        Diamond("Diamond", ScaveImages.IMG_OBJ16_SYM_DIAMOND),
        ThinDiamond("thin_diamond", ScaveImages.IMG_OBJ16_SYM_DIAMOND),
        Dot("Dot", ScaveImages.IMG_OBJ16_SYM_DOT),
        Plus("Plus", ScaveImages.IMG_OBJ16_SYM_PLUS),
        HLine("HLine", ScaveImages.IMG_OBJ16_SYM_PLUS),
        VLine("VLine", ScaveImages.IMG_OBJ16_SYM_PLUS),
        Octagon("Octagon", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Square("Square", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Pentagon("Pentagon", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Star("Star", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Triangle_Up("triangle_up", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Down("triangle_down", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Left("triangle_left", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Right("triangle_right", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Up("tri_up", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Down("tri_down", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Left("tri_left", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Right("tri_right", ScaveImages.IMG_OBJ16_SYM_TRIANGLE);

        private String name;
        private String imageId;

        private SymbolType(String name, String img) {
            this.name = name;
            imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public String getImageId() {
            return imageId;
        }
    }

    public enum DrawStyle {
        None("none", ScaveImages.IMG_OBJ16_LINE_NONE),
        Linear("linear", ScaveImages.IMG_OBJ16_LINE_LINEAR),
        Pins("pins", ScaveImages.IMG_OBJ16_LINE_PINS),
        StepsPost("steps-post", ScaveImages.IMG_OBJ16_LINE_STEPS_POST),
        StepsPre("steps-pre", ScaveImages.IMG_OBJ16_LINE_STEPS_PRE),
        StepsMid("steps-mid", ScaveImages.IMG_OBJ16_LINE_STEPS_PRE);

        private String name;
        private String imageId;

        private DrawStyle(String name, String img) {
            this.name = name;
            this.imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public String getImageId() {
            return imageId;
        }
    }

    public enum LineStyle {
        None("none", Graphics.LINE_SOLID /*doesn't matter*/, null),
        Solid("solid", Graphics.LINE_SOLID, null),
        Dotted("dotted", Graphics.LINE_DOT, null),
        Dashed("dashed", Graphics.LINE_DASH, null),
        DashDot("dashdot", Graphics.LINE_DASHDOT, null);

        private String name;
        private int draw2dConstant;
        private String imageId;

        private LineStyle(String name, int draw2dConstant, String img) {
            this.name = name;
            this.draw2dConstant = draw2dConstant;
            this.imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public int getDraw2DConstant() {
            return draw2dConstant;
        }

        public String getImageId() {
            return imageId;
        }

    }

}
