package org.omnetpp.scave.charting;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;

/**
 * Defines defaults for the chart. Used by both the chart widgets 
 * and the property sheet. 
 * 
 * @author tomi, andras
 */
public interface ChartDefaults {
	// general
	public static final boolean DEFAULT_ANTIALIAS = true;
	public static final boolean DEFAULT_CANVAS_CACHING = true;

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
}
