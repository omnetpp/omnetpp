package org.omnetpp.scave.charting;

import static org.omnetpp.scave.charting.ChartProperties.PROP_ANTIALIAS;
import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_CACHING;
import static org.omnetpp.scave.charting.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave.charting.ChartProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_POSITION;
import static org.omnetpp.scave.charting.ChartProperties.PROP_WRAP_LABELS;
import static org.omnetpp.scave.charting.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.charting.ChartProperties.*;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;
import org.omnetpp.scave.charting.ChartProperties.LineType;
import org.omnetpp.scave.charting.ChartProperties.ShowGrid;

/**
 * Defines defaults for the chart. Used by both the chart widgets 
 * and the property sheet. 
 * 
 * @author tomi, andras
 */
public class ChartDefaults {
	// general
	public static final boolean DEFAULT_ANTIALIAS = true;
	public static final boolean DEFAULT_CANVAS_CACHING = true;

	// title
	public static final String DEFAULT_TITLE = "";
	public static final Font DEFAULT_TITLE_FONT = new Font(null, "Arial", 10, SWT.NORMAL);

	// colors
	public static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.WHITE;
	public static final Color DEFAULT_FOREGROUND_COLOR = ColorFactory.BLACK;
	public static final Color DEFAULT_INSETS_BACKGROUND_COLOR = Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
	public static final Color DEFAULT_INSETS_LINE_COLOR = ColorFactory.BLACK;

	// legend
	public static final boolean DEFAULT_DISPLAY_LEGEND = false;
	public static final boolean DEFAULT_LEGEND_BORDER = false;
	public static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Above;
	public static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;
	public static final Font DEFAULT_LEGEND_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	
	// axes
	public static final String DEFAULT_X_AXIS_TITLE = "";
	public static final String DEFAULT_Y_AXIS_TITLE = "";
	public static final Font DEFAULT_AXIS_TITLE_FONT = new Font(null, "Arial", 10, SWT.NORMAL);
	public static final Color DEFAULT_AXIS_COLOR = ColorFactory.BLACK;
	public static final Font DEFAULT_LABELS_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	public static final boolean DEFAULT_Y_AXIS_LOGARITHMIC = false;

	// grid
	public static final ShowGrid DEFAULT_SHOW_GRID = ShowGrid.Major;
	public static final Color DEFAULT_GRID_COLOR = ColorFactory.GREY80;
	
	// bar chart
	public static final double DEFAULT_BAR_BASELINE = 0.0;
	public static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
	public static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.GREY80;
	public static final boolean DEFAULT_WRAP_LABELS = true;

	// lines
	public static final boolean DEFAULT_DISPLAY_LINE = true;
	public static final LineType DEFAULT_LINE_STYLE = null; // = use interpolationmode attr of vectors
	public static final Integer DEFAULT_SYMBOL_SIZE = 4;
	
	
	// Maps property names to default values
	static Map<String,Object> defaults;
	
	static {
		defaults = new HashMap<String,Object>();
		
		defaults.put(PROP_ANTIALIAS, DEFAULT_ANTIALIAS);
		defaults.put(PROP_CACHING, DEFAULT_CANVAS_CACHING);
		defaults.put(PROP_BACKGROUND_COLOR, DEFAULT_BACKGROUND_COLOR.getRGB());
		
		defaults.put(PROP_GRAPH_TITLE, DEFAULT_TITLE);
		defaults.put(PROP_GRAPH_TITLE_FONT, DEFAULT_TITLE_FONT);
		
		// TODO: INSETS_BACKGROUND_COLOR
		// TODO: INSETS_LINE_COLOR

		defaults.put(PROP_DISPLAY_LEGEND, DEFAULT_DISPLAY_LEGEND);
		defaults.put(PROP_LEGEND_BORDER, DEFAULT_LEGEND_BORDER);
		defaults.put(PROP_LEGEND_POSITION, DEFAULT_LEGEND_POSITION);
		defaults.put(PROP_LEGEND_ANCHORING, DEFAULT_LEGEND_ANCHOR);
		defaults.put(PROP_LEGEND_FONT, DEFAULT_LEGEND_FONT);
		
		defaults.put(PROP_X_AXIS_TITLE, DEFAULT_X_AXIS_TITLE);
		defaults.put(PROP_Y_AXIS_TITLE, DEFAULT_Y_AXIS_TITLE);
		defaults.put(PROP_AXIS_TITLE_FONT, DEFAULT_AXIS_TITLE_FONT);
		// TODO: AXIS_COLOR
		defaults.put(PROP_LABEL_FONT, DEFAULT_LABELS_FONT);
		defaults.put(PROP_X_LABELS_ROTATE_BY, DEFAULT_X_LABELS_ROTATED_BY);
		defaults.put(PROP_WRAP_LABELS, DEFAULT_WRAP_LABELS);
		
		defaults.put(PROP_XY_GRID, DEFAULT_SHOW_GRID);
		
		defaults.put(PROP_BAR_BASELINE, DEFAULT_BAR_BASELINE);
		defaults.put(PROP_BAR_PLACEMENT, DEFAULT_BAR_PLACEMENT);
		// TODO: BAR_OUTLINE_COLOR
		defaults.put(PROP_DISPLAY_LINE, DEFAULT_DISPLAY_LINE);
		defaults.put(PROP_SYMBOL_SIZE, DEFAULT_SYMBOL_SIZE);
		defaults.put(PROP_LINE_TYPE, DEFAULT_LINE_STYLE);
	}
	
	public static Object getDefaultPropertyValue(String propertyName) {
		return defaults.get(getBasePropertyName(propertyName)); //XXX Assert if not found?
	}
	
	public static String getBasePropertyName(String propertyName) {
		int index = propertyName.indexOf('/');
		return index < 0 ? propertyName : propertyName.substring(0, index);
	}
}
