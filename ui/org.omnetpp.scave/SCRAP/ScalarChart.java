/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import static org.omnetpp.scave.model2.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_BAR_BASELINE;
import static org.omnetpp.scave.model2.ChartProperties.PROP_BAR_PLACEMENT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_DISPLAY_LEGEND;
import static org.omnetpp.scave.model2.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave.model2.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_LEGEND_ANCHORING;
import static org.omnetpp.scave.model2.ChartProperties.PROP_LEGEND_BORDER;
import static org.omnetpp.scave.model2.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_LEGEND_POSITION;
import static org.omnetpp.scave.model2.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave.model2.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave.model2.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave.model2.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave.model2.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.model2.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave.model2.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave.model2.ChartProperties.PROP_Y_AXIS_TITLE;

import java.awt.Font;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.axis.CategoryLabelPositions;
import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.LayeredBarRenderer;
import org.jfree.chart.renderer.category.StackedBarRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.general.Dataset;
import org.jfree.ui.RectangleEdge;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.model2.ChartProperties.BarPlacement;
import org.omnetpp.scave.model2.ChartProperties.LegendAnchor;
import org.omnetpp.scave.model2.ChartProperties.LegendPosition;

public class ScalarChart extends InteractiveChart {

	private static final String DEFAULT_TITLE = "";
	private static final String DEFAULT_X_AXIS_TITLE = "";
	private static final String DEFAULT_Y_AXIS_TITLE = "";

	private static final double DEFAULT_BAR_BASELINE = 0.0;
	private static final BarPlacement DEFAULT_BAR_PLACEMENT = BarPlacement.Aligned;

	private static final boolean DEFAULT_DISPLAY_LEGEND = false;
	private static final boolean DEFAULT_LEGEND_BORDER = false;
	private static final LegendPosition DEFAULT_LEGEND_POSITION = LegendPosition.Below;
	private static final LegendAnchor DEFAULT_LEGEND_ANCHOR = LegendAnchor.North;

	private static final boolean DEFAULT_INVERT_XY = false;
	private static final boolean DEFAULT_SHOW_GRID = false;
	private static final double DEFAULT_X_LABELS_ROTATED_BY = 0.0;
	private static final boolean DEFAULT_Y_LOGARITHMIC = false;

	private String title;
	private Font titleFont;
	private String xAxisTitle;
	private String yAxisTitle;
	private Font axisTitleFont;
	private Font tickLabelFont;
	private double xAxisLabelsRotatedBy;

	private boolean displayLegend;
	private Font legendFont;
	private boolean legendBorder;
	private LegendPosition legendPosition;
	private LegendAnchor legendAnchor;

	private Double barBaseline;
	private BarPlacement barPlacement;

	private Double yMin;
	private Double yMax;
	private Boolean invertXY;
	private Boolean gridVisible;

	public ScalarChart(Composite parent, int style) {
		super(parent, style);
	}

	public Dataset getDataset() {
		return getPlot() != null ? getPlot().getDataset() : null;
	}

	public void setDataset(CategoryDataset dataset) {
		if (getPlot() != null) {
			getPlot().setDataset(dataset);
			scheduleRefresh();
		}
	}

	/*=============================================
	 *               Properties
	 *=============================================*/
	public void setProperty(String name, String value) {
		if (chart == null)
			return;
		// Titles
		if (PROP_GRAPH_TITLE.equals(name))
			setTitle(value);
		else if (PROP_GRAPH_TITLE_FONT.equals(name))
			setTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_X_AXIS_TITLE.equals(name))
			setXAxisTitle(value);
		else if (PROP_Y_AXIS_TITLE.equals(name))
			setYAxisTitle(value);
		else if (PROP_AXIS_TITLE_FONT.equals(name))
			setAxisTitleFont(Converter.stringToAwtfont(value));
		else if (PROP_LABEL_FONT.equals(name))
			setLabelFont(Converter.stringToAwtfont(value));
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			setXAxisLabelsRotatedBy(Converter.stringToDouble(value));
		// Legend
		else if (PROP_DISPLAY_LEGEND.equals(name))
			setDisplayLegend(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_BORDER.equals(name))
			setLegendBorder(Converter.stringToBoolean(value));
		else if (PROP_LEGEND_FONT.equals(name))
			setLegendFont(Converter.stringToAwtfont(value));
		else if (PROP_LEGEND_POSITION.equals(name))
			setLegendPosition(Converter.stringToEnum(value, LegendPosition.class));
		else if (PROP_LEGEND_ANCHORING.equals(name))
			setLegendAnchoring(Converter.stringToEnum(value, LegendAnchor.class));
		// Bars
		else if (PROP_BAR_BASELINE.equals(name))
			setBarBaseline(Converter.stringToDouble(value));
		else if (PROP_BAR_PLACEMENT.equals(name))
			setBarPlacement(Converter.stringToEnum(value, BarPlacement.class));
		// Axes
		else if (PROP_XY_INVERT.equals(name))
			setInvertXY(Converter.stringToBoolean(value));
		else if (PROP_Y_AXIS_MIN.equals(name))
			setYMin(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MAX.equals(name))
			setYMax(Converter.stringToDouble(value));
		else if (PROP_XY_GRID.equals(name))
			setGridVisible(Converter.stringToBoolean(value));
		else if (PROP_Y_AXIS_LOGARITHMIC.equals(name))
			setYLogarithmic(Converter.stringToBoolean(value));
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String value) {
		if (value == null)
			value = DEFAULT_TITLE;

		title = value;

		if (chart != null) {
			chart.setTitle(value);
			scheduleRefresh();
		}
	}

	public Font getTitleFont() {
		return titleFont;
	}

	public void setTitleFont(Font font) {
		if (font == null)
			return;

		titleFont = font;

		if (chart != null) {
			if (chart.getTitle() != null)
				chart.getTitle().setFont(font);
			else
				chart.setTitle(new TextTitle("", font));
			scheduleRefresh();
		}
	}

	public String getXAxisTitle() {
		return xAxisTitle;
	}

	public void setXAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_X_AXIS_TITLE;

		xAxisTitle = title;

		if (chart != null) {
			getPlot().getDomainAxis().setLabel(title);
			scheduleRefresh();
		}
	}

	public String getYAxisTitle() {
		return yAxisTitle;
	}

	public void setYAxisTitle(String title) {
		if (title == null)
			title = DEFAULT_Y_AXIS_TITLE;

		yAxisTitle = title;

		if (chart != null) {
			getPlot().getRangeAxis().setLabel(title);
			scheduleRefresh();
		}
	}

	public Font getAxisTitleFont() {
		return axisTitleFont;
	}

	public void setAxisTitleFont(Font font) {
		if (font == null)
			return;

		axisTitleFont = font;

		if (chart != null) {
			getPlot().getDomainAxis().setLabelFont(font);
			getPlot().getRangeAxis().setLabelFont(font);
			scheduleRefresh();
		}
	}

	public Font getLabelFont() {
		return tickLabelFont;
	}

	public void setLabelFont(Font font) {
		if (font == null)
			return;

		tickLabelFont = font;

		if (chart != null) {
			getPlot().getDomainAxis().setTickLabelFont(font);
			getPlot().getRangeAxis().setTickLabelFont(font);
			scheduleRefresh();
		}
	}

	public Double getXAxisLabelsRotatedBy() {
		return xAxisLabelsRotatedBy;
	}

	public void setXAxisLabelsRotatedBy(Double angle) {
		if (angle == null)
			angle = DEFAULT_X_LABELS_ROTATED_BY;

		xAxisLabelsRotatedBy = angle;

		if (chart != null) {
			getPlot().getDomainAxis().setCategoryLabelPositions(
				CategoryLabelPositions.createDownRotationLabelPositions(Math.toRadians(angle)));
			scheduleRefresh();
		}
	}

	public Boolean getDisplayLegend() {
		return displayLegend;
	}

	public void setDisplayLegend(Boolean value) {
		if (value == null)
			value = DEFAULT_DISPLAY_LEGEND;

		displayLegend = value;

		if (chart != null) {
			if (value) {
				chart.removeLegend();
				chart.addLegend(new LegendTitle(chart.getPlot()));
				setLegendBorder(legendBorder);
				setLegendFont(legendFont);
				setLegendPosition(legendPosition);
				setLegendAnchoring(legendAnchor);
			}
			else
				chart.removeLegend();
			scheduleRefresh();
		}
	}

	public Boolean getLegendBorder() {
		return legendBorder;
	}

	public void setLegendBorder(Boolean value) {
		if (value == null)
			value = DEFAULT_LEGEND_BORDER;

		legendBorder = value;

		if (chart != null && chart.getLegend() != null) {
			chart.getLegend().setBorder(value ? new BlockBorder() : BlockBorder.NONE);
			scheduleRefresh();
		}
	}

	public Font getLegendFont() {
		return legendFont;
	}

	public void setLegendFont(Font font) {
		if (font == null)
			return;

		legendFont = font;

		if (chart != null && chart.getLegend() != null) {
			chart.getLegend().setItemFont(font);
			scheduleRefresh();
		}
	}

	public LegendPosition getLegendPosition() {
		return legendPosition;
	}

	public void setLegendPosition(LegendPosition value) {
		if (value == null)
			value = DEFAULT_LEGEND_POSITION;

		legendPosition = value;

		if (chart != null && chart.getLegend() != null) {
			RectangleEdge position = null;
			switch (value) {
			case Inside: /*not supported?*/ break;
			case Above: position = RectangleEdge.TOP; break;
			case Below: position = RectangleEdge.BOTTOM; break;
			case Left: position = RectangleEdge.LEFT; break;
			case Right: position = RectangleEdge.RIGHT; break;
			default: Assert.isLegal(false, "Unknown LegendPosition: " + value);
			}

			if (position != null)
				chart.getLegend().setPosition(position);
			scheduleRefresh();
		}
	}

	public LegendAnchor getLegendAnchoring() {
		return legendAnchor;
	}

	public void setLegendAnchoring(LegendAnchor value) {
		if (value == null)
			value = DEFAULT_LEGEND_ANCHOR;

		legendAnchor = value;

		if (chart != null && chart.getLegend() != null) {
			// TODO
			scheduleRefresh();
		}
	}

	public Double getBarBaseline() {
		return barBaseline;
	}

	public void setBarBaseline(Double value) {
		if (value == null)
			value = DEFAULT_BAR_BASELINE;

		barBaseline = value;

		if (chart != null) {
			CategoryPlot plot = (CategoryPlot)chart.getPlot();
			if (plot.getRenderer() instanceof BarRenderer) {
				BarRenderer renderer = (BarRenderer)plot.getRenderer();
				renderer.setBase(value);
			}
			scheduleRefresh();
		}
	}

	public BarPlacement getBarPlacement() {
		return barPlacement;
	}

	public void setBarPlacement(BarPlacement value) {
		if (value == null)
			value = DEFAULT_BAR_PLACEMENT;

		barPlacement = value;

		if (chart != null) {
			BarRenderer renderer = null;
			switch (value) {
			case Aligned: renderer = new BarRenderer(); break;
			case Overlap: renderer = new LayeredBarRenderer(); break;
			case InFront: renderer = new LayeredBarRenderer(); break;
			case Stacked: renderer = new StackedBarRenderer(); break;
			default: Assert.isLegal(false, "Unknown BarPlacement value: " + value); break;
			}

			if (renderer != null) {
				CategoryPlot plot = (CategoryPlot)chart.getPlot();
				plot.setRenderer(renderer);
			}
			scheduleRefresh();
		}
	}

	public Double getYMin() {
		return yMin;
	}

	public void setYMin(Double value) {
		yMin = value;

		if (chart != null) {
			if (value != null)
				getPlot().getRangeAxis().setLowerBound(value);
			else
				getPlot().getRangeAxis().setAutoRange(true);
			scheduleRefresh();
		}
	}

	public Double getYMax() {
		return yMax;
	}

	public void setYMax(Double value) {
		yMax = value;

		if (chart != null) {
			if (value != null)
				getPlot().getRangeAxis().setUpperBound(value);
			else
				getPlot().getRangeAxis().setAutoRange(true);
			scheduleRefresh();
		}
	}

	public Boolean getInvertXY() {
		return invertXY;
	}

	public void setInvertXY(Boolean value) {
		if (value == null)
			value = DEFAULT_INVERT_XY;

		invertXY = value;

		if (chart != null) {
			getPlot().setOrientation(value ? PlotOrientation.HORIZONTAL : PlotOrientation.VERTICAL);
			scheduleRefresh();
		}
	}

	public Boolean getGridVisible() {
		return gridVisible;
	}

	public void setGridVisible(Boolean value) {
		if (value == null)
			value = DEFAULT_SHOW_GRID;

		gridVisible = value;

		if (chart != null) {
			CategoryPlot plot = getPlot();
			if (gridVisible) {
				plot.setDomainGridlinesVisible(true);
				plot.setRangeGridlinesVisible(true);
			}
			else {
				plot.setDomainGridlinesVisible(false);
				plot.setRangeGridlinesVisible(false);
			}
			scheduleRefresh();
		}
	}

	private void setYLogarithmic(Boolean value) {
		if (value == null)
			value = DEFAULT_Y_LOGARITHMIC;

		if (chart != null) {
			if (value) {
				LogarithmicAxis axis = new LogarithmicAxis(yAxisTitle);
				axis.setStrictValuesFlag(false);
				getPlot().setRangeAxis(axis);
			}
			else
				getPlot().setRangeAxis(new NumberAxis(yAxisTitle));
			scheduleRefresh();
		}
	}

	private CategoryPlot getPlot() {
		Assert.isTrue(chart != null);
		return (CategoryPlot)chart.getPlot();
	}
}
