/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_ANTIALIAS;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_BACKGROUND_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_CACHING;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LINE_DRAW_STYLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_HIST_BAR;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LEGEND_POSITION;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LINE_STYLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_LINE_WIDTH;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_PLOT_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_PLOT_TITLE_FONT;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_SHOW_OVERFLOW_CELL;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_WRAP_LABELS;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave.charting.properties.PlotProperties.PROP_Y_AXIS_TITLE;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.properties.PlotProperties.BarPlacement;
import org.omnetpp.scave.charting.properties.PlotProperties.DrawStyle;
import org.omnetpp.scave.charting.properties.PlotProperties.HistogramBar;
import org.omnetpp.scave.charting.properties.PlotProperties.HistogramDataType;
import org.omnetpp.scave.charting.properties.PlotProperties.LegendAnchor;
import org.omnetpp.scave.charting.properties.PlotProperties.LegendPosition;
import org.omnetpp.scave.charting.properties.PlotProperties.LineStyle;
import org.omnetpp.scave.charting.properties.PlotProperties.ShowGrid;
import org.omnetpp.scave.charting.properties.PlotProperties.SymbolType;

/**
 * Defines defaults for the chart. Used by both the chart widgets
 * and the property sheet.
 *
 * @author tomi, andras
 */
public class PlotDefaults {
    // general
    public static final boolean DEFAULT_ANTIALIAS = true;
    public static final boolean DEFAULT_CANVAS_CACHING = true;

    // title
    public static final String DEFAULT_TITLE = "";
    public static final FontData DEFAULT_TITLE_FONTDATA = new FontData("Arial", 10, SWT.NORMAL);
    public static final Font DEFAULT_TITLE_FONT = new Font(null, DEFAULT_TITLE_FONTDATA);
    public static final Color DEFAULT_TITLE_COLOR = ColorFactory.BLACK;

    // colors
    public static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.WHITE;
    public static final Color DEFAULT_FOREGROUND_COLOR = ColorFactory.BLACK;
    public static final Color DEFAULT_INSETS_BACKGROUND_COLOR = Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
    public static final Color DEFAULT_INSETS_LINE_COLOR = ColorFactory.BLACK;

    // legend
    public static final boolean DEFAULT_DISPLAY_LEGEND = true;
    public static final boolean DEFAULT_LEGEND_BORDER = false;
    public static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Inside;
    public static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;
    public static final FontData DEFAULT_LEGEND_FONTDATA = new FontData("Arial", 8, SWT.NORMAL);
    public static final Font DEFAULT_LEGEND_FONT = new Font(null, DEFAULT_LEGEND_FONTDATA);

    // axes
    public static final String DEFAULT_X_AXIS_TITLE = "";
    public static final String DEFAULT_Y_AXIS_TITLE = "";
    public static final FontData DEFAULT_AXIS_TITLE_FONTDATA = new FontData("Arial", 10, SWT.NORMAL);
    public static final Font DEFAULT_AXIS_TITLE_FONT = new Font(null, DEFAULT_AXIS_TITLE_FONTDATA);
    public static final Color DEFAULT_AXIS_COLOR = ColorFactory.BLACK;
    public static final FontData DEFAULT_LABELS_FONTDATA = new FontData("Arial", 8, SWT.NORMAL);
    public static final Font DEFAULT_LABELS_FONT = new Font(null, DEFAULT_LABELS_FONTDATA);
    public static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
    public static final double DEFAULT_X_AXIS_MIN = Double.NEGATIVE_INFINITY;
    public static final double DEFAULT_X_AXIS_MAX = Double.POSITIVE_INFINITY;
    public static final double DEFAULT_Y_AXIS_MIN = Double.NEGATIVE_INFINITY;
    public static final double DEFAULT_Y_AXIS_MAX = Double.POSITIVE_INFINITY;
    public static final boolean DEFAULT_X_AXIS_LOGARITHMIC = false;
    public static final boolean DEFAULT_Y_AXIS_LOGARITHMIC = false;

    // grid
    public static final ShowGrid DEFAULT_SHOW_GRID = ShowGrid.Major;
    public static final Color DEFAULT_GRID_COLOR = ColorFactory.GREY80;

    // bar chart
    public static final double DEFAULT_BAR_BASELINE = 0.0;
    public static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
    public static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.GREY80;
    public static final Color DEFAULT_BAR_BASELINE_COLOR = ColorFactory.GREY80;
    public static final boolean DEFAULT_WRAP_LABELS = true;

    // lines
    public static final boolean DEFAULT_DISPLAY_LINE = true;
    public static final DrawStyle DEFAULT_LINE_DRAW_STYLE = null; // = use interpolationmode attr of vectors
    public static final Integer DEFAULT_SYMBOL_SIZE = 4;
    public static final SymbolType DEFAULT_SYMBOL_TYPE = null;
    public static final LineStyle DEFAULT_LINE_STYLE = LineStyle.Solid;
    public static final Float DEFAULT_LINE_WIDTH = 1.5f;
    public static final Color DEFAULT_LINE_COLOR = null;

    // histogram chart
    public static final HistogramBar DEFAULT_HIST_BAR = HistogramBar.Solid;
    public static final HistogramDataType DEFAULT_HIST_DATA = HistogramDataType.Count;
    public static final boolean DEFAULT_SHOW_OVERFLOW_CELL = false;

    // Maps property names to default values
    static Map<String,Object> defaults;

    static {
        defaults = new HashMap<String,Object>();

        defaults.put(PROP_ANTIALIAS, DEFAULT_ANTIALIAS);
        defaults.put(PROP_CACHING, DEFAULT_CANVAS_CACHING);
        defaults.put(PROP_BACKGROUND_COLOR, DEFAULT_BACKGROUND_COLOR.getRGB());

        defaults.put(PROP_PLOT_TITLE, DEFAULT_TITLE);
        defaults.put(PROP_PLOT_TITLE_FONT, DEFAULT_TITLE_FONTDATA);

        // TODO: INSETS_BACKGROUND_COLOR
        // TODO: INSETS_LINE_COLOR

        defaults.put(PROP_DISPLAY_LEGEND, DEFAULT_DISPLAY_LEGEND);
        defaults.put(PROP_LEGEND_BORDER, DEFAULT_LEGEND_BORDER);
        defaults.put(PROP_LEGEND_POSITION, DEFAULT_LEGEND_POSITION);
        defaults.put(PROP_LEGEND_ANCHORING, DEFAULT_LEGEND_ANCHOR);
        defaults.put(PROP_LEGEND_FONT, DEFAULT_LEGEND_FONTDATA);

        defaults.put(PROP_X_AXIS_TITLE, DEFAULT_X_AXIS_TITLE);
        defaults.put(PROP_Y_AXIS_TITLE, DEFAULT_Y_AXIS_TITLE);
        defaults.put(PROP_AXIS_TITLE_FONT, DEFAULT_AXIS_TITLE_FONTDATA);
        // TODO: AXIS_COLOR
        defaults.put(PROP_LABEL_FONT, DEFAULT_LABELS_FONTDATA);
        defaults.put(PROP_X_LABELS_ROTATE_BY, DEFAULT_X_LABELS_ROTATED_BY);
        defaults.put(PROP_WRAP_LABELS, DEFAULT_WRAP_LABELS);
        defaults.put(PROP_X_AXIS_MIN, DEFAULT_X_AXIS_MIN);
        defaults.put(PROP_X_AXIS_MAX, DEFAULT_X_AXIS_MAX);
        defaults.put(PROP_Y_AXIS_MIN, DEFAULT_Y_AXIS_MIN);
        defaults.put(PROP_Y_AXIS_MAX, DEFAULT_Y_AXIS_MAX);
        defaults.put(PROP_X_AXIS_LOGARITHMIC, DEFAULT_X_AXIS_LOGARITHMIC);
        defaults.put(PROP_Y_AXIS_LOGARITHMIC, DEFAULT_Y_AXIS_LOGARITHMIC);

        defaults.put(PROP_XY_GRID, DEFAULT_SHOW_GRID);

        defaults.put(PROP_BAR_BASELINE, DEFAULT_BAR_BASELINE);
        defaults.put(PROP_BAR_PLACEMENT, DEFAULT_BAR_PLACEMENT);
        // TODO: BAR_OUTLINE_COLOR

        // Lines
        defaults.put(PROP_DISPLAY_LINE, DEFAULT_DISPLAY_LINE);
        defaults.put(PROP_LINE_DRAW_STYLE, DEFAULT_LINE_DRAW_STYLE);
        defaults.put(PROP_LINE_STYLE, DEFAULT_LINE_STYLE);
        defaults.put(PROP_LINE_WIDTH, DEFAULT_LINE_WIDTH);
        defaults.put(PROP_LINE_COLOR, DEFAULT_LINE_COLOR);
        defaults.put(PROP_SYMBOL_TYPE, DEFAULT_SYMBOL_TYPE);
        defaults.put(PROP_SYMBOL_SIZE, DEFAULT_SYMBOL_SIZE);

        defaults.put(PROP_HIST_BAR, DEFAULT_HIST_BAR);
        defaults.put(PROP_SHOW_OVERFLOW_CELL, DEFAULT_SHOW_OVERFLOW_CELL);

    }

    public static boolean hasDefaultValue(String propertyName) {
        return defaults.containsKey(getBasePropertyName(propertyName));
    }

    public static String getDefaultPropertyValueAsString(String propertyName) {
        return Converter.objectToString(getDefaultPropertyValue(propertyName));
    }

    public static Object getDefaultPropertyValue(String propertyName) {
        return defaults.get(getBasePropertyName(propertyName));
    }

    public static String getBasePropertyName(String propertyName) {
        int index = propertyName.indexOf('/');
        return index < 0 ? propertyName : propertyName.substring(0, index);
    }
}
