/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.ScaveImages;

/**
 * Property names and types plots.
 */
public enum PlotProperty {
    // Title
    PROP_PLOT_TITLE("Plot.Title", String.class, ""),
    PROP_PLOT_TITLE_FONT("Plot.Title.Font", Font.class, getArial10()),
    PROP_PLOT_TITLE_COLOR("Plot.Title.Color", Color.class, ColorFactory.BLACK),
    // Axes
    PROP_X_AXIS_TITLE("X.Axis.Title", String.class, ""),
    PROP_X_AXIS_MIN("X.Axis.Min", Double.class, Double.NEGATIVE_INFINITY),
    PROP_X_AXIS_MAX("X.Axis.Max", Double.class, Double.POSITIVE_INFINITY),
    PROP_X_AXIS_LOGARITHMIC("X.Axis.Log", Boolean.class, false),
    PROP_X_LABELS_ROTATE_BY("X.Label.RotateBy", Integer.class, 0),
    PROP_Y_AXIS_TITLE("Y.Axis.Title", String.class, ""),
    PROP_Y_AXIS_MIN("Y.Axis.Min", Double.class, Double.NEGATIVE_INFINITY),
    PROP_Y_AXIS_MAX("Y.Axis.Max", Double.class, Double.POSITIVE_INFINITY),
    PROP_Y_AXIS_LOGARITHMIC("Y.Axis.Log", Boolean.class, false),
    PROP_AXIS_TITLE_FONT("Axis.Title.Font", Font.class, getArial8()),
    PROP_AXIS_LABEL_FONT("Axis.Label.Font", Font.class, getArial8()),
    // Grid
    PROP_GRID("Axes.Grid", ShowGrid.class, ShowGrid.Major),
    PROP_GRID_COLOR("Axes.GridColor", Color.class, ColorFactory.GREY80),
    PROP_INSETS_LINE_COLOR("Insets.LineColor", Color.class, ColorFactory.BLACK),
    PROP_INSETS_BACKGROUND_COLOR("Insets.BackgroundColor", Color.class, Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND)),
    // Legend
    PROP_DISPLAY_LEGEND("Legend.Display", Boolean.class, true),
    PROP_LEGEND_BORDER("Legend.Border", Boolean.class, false),
    PROP_LEGEND_FONT("Legend.Font", Font.class, getArial8()),
    PROP_LEGEND_POSITION("Legend.Position", LegendPosition.class, PlotProperty.LegendPosition.Inside),
    PROP_LEGEND_ANCHORING("Legend.Anchoring", LegendAnchor.class, PlotProperty.LegendAnchor.North),
    // Plot
    PROP_ANTIALIAS("Plot.Antialias", Boolean.class, true),
    PROP_CACHING("Plot.Caching", Boolean.class, true),
    PROP_BACKGROUND_COLOR("Plot.BackgroundColor", Color.class, ColorFactory.WHITE),
    // Bar Plot
    PROP_WRAP_LABELS("X.Label.Wrap", Boolean.class, true),
    PROP_BAR_PLACEMENT("Bar.Placement", BarPlacement.class, PlotProperty.BarPlacement.Aligned),
    PROP_BAR_COLOR("Bar.Color", Color.class, ColorFactory.GREY80),
    PROP_BAR_OUTLINE_COLOR("Bar.OutlineColor", Color.class, ColorFactory.BLACK),
    PROP_BAR_BASELINE("Bars.Baseline", Double.class, 0.0),
    PROP_BAR_BASELINE_COLOR("Bars.Baseline.Color", Color.class, ColorFactory.GREY80),
    // Histograms
    PROP_HIST_BAR("Hist.Bar", HistogramBar.class, PlotProperty.HistogramBar.Solid),
    PROP_SHOW_OVERFLOW_CELL("Hist.ShowOverflowCell", Boolean.class, false),
    PROP_HIST_COLOR("Hist.Color", Color.class, ColorFactory.BLUE),
    PROP_HIST_CUMULATIVE("Hist.Cumulative", Boolean.class, false),
    PROP_HIST_DENSITY("Hist.Density", Boolean.class, false),
    // Lines
    PROP_DISPLAY_LINE("Line.Display", Boolean.class, true),
    PROP_LINE_DRAW_STYLE("Line.DrawStyle", DrawStyle.class, DrawStyle.Linear),
    PROP_LINE_COLOR("Line.Color", Color.class, ColorFactory.BLUE), // note: no auto-cycling
    PROP_LINE_STYLE("Line.Style", LineStyle.class, PlotProperty.LineStyle.Solid),
    PROP_LINE_WIDTH("Line.Width", Float.class, 1.5f),
    PROP_SYMBOL_TYPE("Symbols.Type", SymbolType.class, SymbolType.Square), // note: no auto-cycling
    PROP_SYMBOL_SIZE("Symbols.Size", Integer.class, 4);

    private static Font getArial10() {return new Font(null, new FontData("Arial", 10, SWT.NORMAL));}
    private static Font getArial8() {return new Font(null, new FontData("Arial", 8, SWT.NORMAL));}

    private String name;
    private Class<?> type;
    private Object defaultValue;
    private static Map<String,PlotProperty> map = null;

    PlotProperty(String name, Class<?> type, Object defaultValue) {
        this.name = name;
        this.type = type;
        this.defaultValue = defaultValue;
        if (defaultValue != null)
            Assert.isTrue(type.isInstance(defaultValue));
    }

    public String getName() {
        return name;
    }

    public Class<?> getType() {
        return type;
    }

    public Object getDefaultValue() {
        return defaultValue;
    }

    public String getDefaultValueAsString() {
        return Converter.objectToString(getDefaultValue());
    }

    public static PlotProperty loookup(String propertyName) {
        if (map == null) {
            map = new HashMap<>();
            for (PlotProperty p : PlotProperty.values())
               map.put(p.getName(), p);
        }
        String baseName = getBasePropertyName(propertyName);
        return map.get(baseName);

    }

     public static String getBasePropertyName(String propertyName) {
        int index = propertyName.indexOf('/');
        return index < 0 ? propertyName : propertyName.substring(0, index);
    }

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
        Circle("Circle", ScaveImages.IMG_OBJ16_SYM_DOT),
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
