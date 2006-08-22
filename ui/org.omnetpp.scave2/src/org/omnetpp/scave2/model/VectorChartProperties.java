package org.omnetpp.scave2.model;

public class VectorChartProperties {
	
	public static final String
	// Titles
	PROP_GRAPH_TITLE		= "Graph.Title",
	PROP_GRAPH_TITLE_FONT	= "Graph.Title.Font",
	PROP_X_AXIS_TITLE		= "X.Axis.Title",
	PROP_Y_AXIS_TITLE		= "Y.Axis.Title",
	PROP_AXIS_TITLE_FONT	= "Axis.Title.Font",
	PROP_LABEL_FONT			= "Label.Font",
	PROP_X_LABELS_ROTATE_BY	= "X.Label.RotateBy",
	// Axes
	PROP_X_AXIS_MIN			= "X.Axis.Min",
	PROP_X_AXIS_MAX			= "X.Axis.Max",
	PROP_Y_AXIS_MIN			= "Y.Axis.Min",
	PROP_Y_AXIS_MAX			= "Y.Axis.Max",
	PROP_X_AXIS_LOGARITHMIC = "X.Axis.Log",
	PROP_Y_AXIS_LOGARITHMIC	= "Y.Axis.Log",
	PROP_XY_INVERT			= "Axes.Invert",
	PROP_XY_GRID			= "Axes.Grid",
	// Lines
	PROP_DISPLAY_SYMBOLS	= "Symbols.Display",
	PROP_SYMBOL_TYPE		= "Symbols.Type",
	PROP_SYMBOL_SIZE		= "Symbols.Size",
	PROP_LINE_TYPE			= "Line.Type",
	PROP_HIDE_LINE			= "Lines.Hide",
	// Legend
	PROP_DISPLAY_LEGEND		= "Legend.Display",
	PROP_LEGEND_BORDER		= "Legend.Border",
	PROP_LEGEND_FONT		= "Legend.Font",
	PROP_LEGEND_POSITION	= "Legend.Position",
	PROP_LEGEND_ANCHORING	= "Legend.Anchoring";

	public enum ShowGrid {
		None,
		AtMajorTicks,
		AtAllTicks,
	}
	
	public enum LineStyle {
		None,
		Linear,
		Step,
		Spline,
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
}
