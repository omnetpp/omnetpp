package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LEGEND_POSITION;

import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave2.model.ChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.ChartProperties.LegendPosition;

public abstract class ChartCanvas extends ZoomableCachingCanvas {

	protected static final String DEFAULT_TITLE = "";
	protected static final boolean DEFAULT_DISPLAY_LEGEND = false;
	protected static final boolean DEFAULT_LEGEND_BORDER = false;
	protected static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Above;
	protected static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;
	
	protected Title title = new Title(DEFAULT_TITLE, null);
	protected Legend legend = new Legend(DEFAULT_DISPLAY_LEGEND, DEFAULT_LEGEND_BORDER, null, DEFAULT_LEGEND_POSITION, DEFAULT_LEGEND_ANCHOR);
	
	private Runnable scheduledRedraw;
	
	public ChartCanvas(Composite parent, int style) {
		super(parent, style);
	}
	
	public void setProperty(String name, String value) {
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToSwtfont(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			setDisplayLegend(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_BORDER.equals(name))
			setLegendBorder(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_FONT.equals(name))
			setLegendFont(Converter.stringToSwtfont(value));
		else if (PROP_LEGEND_POSITION.equals(name))
			setLegendPosition(Converter.stringToEnum(value, LegendPosition.class));
		else if (PROP_LEGEND_ANCHORING.equals(name))
			setLegendAnchor(Converter.stringToEnum(value, LegendAnchor.class));
	}
	
	public void setTitle(String value) {
		if (value == null)
			value = DEFAULT_TITLE;
		title.setText(value);
		scheduleRedraw();
	}

	public void setTitleFont(Font value) {
		if (value == null)
			return;
		title.setFont(value);
		scheduleRedraw();
	}
	
	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;
		legend.setVisible(value);
		scheduleRedraw();
	}
	
	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;
		legend.setDrawBorder(value);
		scheduleRedraw();
	}
	
	public void setLegendFont(Font value) {
		if (value == null)
			return;
		legend.setFont(value);
		scheduleRedraw();
	}
	
	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;
		legend.setPosition(value);
		scheduleRedraw();
	}
	
	public void setLegendAnchor(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;
		legend.setAnchor(value);
		scheduleRedraw();
	}
	

	protected void scheduleRedraw() {
		if (scheduledRedraw == null) {
			scheduledRedraw = new Runnable() {
				public void run() {
					scheduledRedraw = null;
					clearCanvasCacheAndRedraw();
				}
			};
			getDisplay().asyncExec(scheduledRedraw);
		}
	}
}
