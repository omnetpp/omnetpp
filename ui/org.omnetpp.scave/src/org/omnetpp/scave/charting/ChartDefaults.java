package org.omnetpp.scave.charting;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;
import org.omnetpp.scave.charting.ChartProperties.LineStyle;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;

import static org.omnetpp.scave.charting.ChartProperties.*;

/**
 * Defines defaults for the chart. Used by both the chart widgets 
 * and the property sheet. 
 * 
 * @author tomi, andras
 */
public class ChartDefaults {
	// general
	public static final boolean DEFAULT_ANTIALIAS = true;
	public static final boolean DEFAULT_CANVAS_CACHING = false; //XXX for testing

	// title
	public static final String DEFAULT_TITLE = "";
	public static final Font DEFAULT_TITLE_FONT = new Font(null, "Arial", 10, SWT.NORMAL);

	// background colors
	public static final Color DEFAULT_BACKGROUND_COLOR = ColorFactory.asColor("white");
	public static final Color DEFAULT_INSETS_BACKGROUND_COLOR = new Color(null, 236, 233, 216);
	public static final Color DEFAULT_INSETS_LINE_COLOR = new Color(null, 0, 0, 0);

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
	public static final Font DEFAULT_TICK_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final Color DEFAULT_AXIS_COLOR = ColorFactory.asColor("black");
	public static final Font DEFAULT_LABELS_FONT = new Font(null, "Arial", 8, SWT.NORMAL);
	public static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	public static final boolean DEFAULT_INVERT_XY = false;

	// grid
	public static final boolean DEFAULT_SHOW_GRID = false;
	public static final Color DEFAULT_GRID_COLOR = ColorFactory.asColor("grey80");
	
	// bar chart
	public static final double DEFAULT_BAR_BASELINE = 0.0;
	public static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;
	public static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.asColor("grey80");

	// lines
	public static final LineStyle DEFAULT_LINE_STYLE = LineStyle.Linear;
	public static final Integer DEFAULT_SYMBOL_SIZE = Integer.valueOf(3);
	public static final SymbolType DEFAULT_SYMBOL_TYPE = SymbolType.Square;
	
	
	// Maps property names to default values
	static Map<String,Object> defaults;
	
	static {
		defaults = new HashMap<String,Object>();
		
		defaults.put(PROP_ANTIALIAS, DEFAULT_ANTIALIAS);
		defaults.put(PROP_CACHING, DEFAULT_CANVAS_CACHING);
		
		defaults.put(PROP_GRAPH_TITLE, DEFAULT_TITLE);
		defaults.put(PROP_GRAPH_TITLE_FONT, DEFAULT_TITLE_FONT);
		
		// TODO: BACKGROUND_COLOR
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
		// TODO: TICK_FONT
		// TODO: AXIS_COLOR
		defaults.put(PROP_LABEL_FONT, DEFAULT_LABELS_FONT);
		defaults.put(PROP_X_LABELS_ROTATE_BY, DEFAULT_X_LABELS_ROTATED_BY);
		defaults.put(PROP_XY_INVERT, DEFAULT_INVERT_XY);
		
		defaults.put(PROP_XY_GRID, DEFAULT_SHOW_GRID);
		
		defaults.put(PROP_BAR_BASELINE, DEFAULT_BAR_BASELINE);
		defaults.put(PROP_BAR_PLACEMENT, DEFAULT_BAR_PLACEMENT);
		// TODO: BAR_OUTLINE_COLOR
		// TODO: DEFAULT_LINE_STYLE, DEFAULT_SYMBOL_SIZE
	}
	
	public static Object getDefaultPropertyValue(String propertyName) {
		return defaults.get(propertyName); //XXX Assert if not found?
	}
}
