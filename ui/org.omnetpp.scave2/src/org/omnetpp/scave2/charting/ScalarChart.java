package org.omnetpp.scave2.charting;

import static org.omnetpp.scave2.model.ChartProperties.*;

import java.awt.Font;

import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.axis.Axis;
import org.jfree.chart.axis.AxisSpace;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.CategoryLabelPositions;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.chart.renderer.category.LayeredBarRenderer;
import org.jfree.chart.renderer.category.StackedBarRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.chart.title.TextTitle;
import org.jfree.ui.RectangleEdge;
import org.omnetpp.common.util.Converter;

public class ScalarChart extends InteractiveChart {

	public ScalarChart(Composite parent, int style) {
		super(parent, style);
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
			setXAxisRotatedBy(Converter.stringToDouble(value));
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
		// TODO: axes
		else if (PROP_X_LABELS_ROTATE_BY.equals(name))
			setXLabelsRotatedBy(Converter.stringToDouble(value));
		else if (PROP_XY_INVERT.equals(name))
			setInvertXY(Converter.stringToBoolean(value));
		else if (PROP_Y_AXIS_MIN.equals(name))
			setYMin(Converter.stringToDouble(value));
		else if (PROP_Y_AXIS_MAX.equals(name))
			setYMax(Converter.stringToDouble(value));
	}
	
	public void setTitle(String title) {
		if (chart != null)
			chart.setTitle(title);
		scheduleRefresh();
	}
	
	public void setTitleFont(Font font) {
		if (chart == null || font == null)
			return;
		TextTitle mainTitle = chart.getTitle();
		if (mainTitle != null)
			mainTitle.setFont(font);
		else
			chart.setTitle(new TextTitle("", font));
		scheduleRefresh();
	}
	
	public void setXAxisTitle(String title) {
		Axis xAxis = getDomainAxis();
		if (xAxis != null)
			xAxis.setLabel(title);
		scheduleRefresh();
	}
	
	public void setYAxisTitle(String title) {
		Axis yAxis = getRangeAxis();
		if (yAxis != null)
			yAxis.setLabel(title);
		scheduleRefresh();
	}
	
	public void setAxisTitleFont(Font font) {
		if (chart == null || font == null)
			return;

		Axis xAxis = getDomainAxis();
		Axis yAxis = getRangeAxis();
		if (xAxis != null)
			xAxis.setLabelFont(font);
		if (yAxis != null)
			yAxis.setLabelFont(font);
		scheduleRefresh();
	}
	
	public void setLabelFont(Font font) {
		if (chart == null || font == null)
			return;
		
		Axis xAxis = getDomainAxis();
		Axis yAxis = getRangeAxis();
		if (xAxis != null)
			xAxis.setTickLabelFont(font);
		if (yAxis != null)
			yAxis.setTickLabelFont(font);
		scheduleRefresh();
	}
	
	public void setXAxisRotatedBy(Double angle) {
		if (chart == null)
			return;
		
		if (angle == null)
			angle = 0.0;

		CategoryPlot plot = (CategoryPlot)chart.getPlot();
		CategoryAxis axis = plot.getDomainAxis();
		angle = Math.toRadians(angle);
		axis.setCategoryLabelPositions(CategoryLabelPositions.createDownRotationLabelPositions(angle));
		scheduleRefresh();
	}
	
	public void setDisplayLegend(Boolean value) {
		if (chart == null)
			return;
		
		if (value == null)
			value = Boolean.FALSE;
		
		if (value) {
			chart.removeLegend();
			chart.addLegend(new LegendTitle(chart.getPlot()));
		}
		else
			chart.removeLegend();
		scheduleRefresh();
	}
	
	public void setLegendBorder(Boolean value) {
		if (chart == null)
			return;
		
		if (value == null)
			value = Boolean.FALSE;
		
		if (chart.getLegend() != null)
			chart.getLegend().setBorder(value ? new BlockBorder() : BlockBorder.NONE);
		scheduleRefresh();
	}
	
	public void setLegendFont(Font font) {
		if (chart == null || font == null)
			return;
		
		if (chart.getLegend() != null)
			chart.getLegend().setItemFont(font);
		scheduleRefresh();
	}

	public void setLegendPosition(LegendPosition value) {
		if (chart == null || value == null)
			return;
		
		RectangleEdge position = null;
		switch (value) {
		case Inside: /*not supported?*/ break;
		case Above: position = RectangleEdge.TOP; break;
		case Below: position = RectangleEdge.BOTTOM; break;
		case Left: position = RectangleEdge.LEFT; break;
		case Right: position = RectangleEdge.RIGHT; break;
		}
		
		if (position != null && chart.getLegend() != null)
			chart.getLegend().setPosition(position);
		scheduleRefresh();
	}

	public void setLegendAnchoring(LegendAnchor value) {
		if (chart == null || value == null)
			return;
		// TODO
		scheduleRefresh();
	}

	public void setBarBaseline(Double value) {
		if (chart == null || value == null)
			return;
		
		CategoryPlot plot = (CategoryPlot)chart.getPlot();
		if (plot.getRenderer() instanceof BarRenderer) {
			BarRenderer renderer = (BarRenderer)plot.getRenderer();
			renderer.setBase(value);
		}
		scheduleRefresh();
	}
	
	public void setBarPlacement(BarPlacement value) {
		if (chart == null || value == null)
			return;
		
		BarRenderer renderer = null;
		switch (value) {
		case Aligned: renderer = new BarRenderer(); break; 
		case Overlap: renderer = new LayeredBarRenderer(); break;
		case InFront: renderer = new LayeredBarRenderer(); break;
		case Stacked: renderer = new StackedBarRenderer(); break;
		}
		
		if (renderer != null) {
			CategoryPlot plot = (CategoryPlot)chart.getPlot();
			plot.setRenderer(renderer);
		}
		scheduleRefresh();
	}
	
	public void setYMin(Double value) {
		if (chart == null || value == null)
			return;
		ValueAxis axis = getRangeAxis();
		axis.setLowerBound(value);
		scheduleRefresh();
	}
	
	public void setYMax(Double value) {
		if (chart == null || value == null)
			return;
		ValueAxis axis = getRangeAxis();
		axis.setUpperBound(value);
		scheduleRefresh();
	}
	
	public void setXLabelsRotatedBy(Double value) {
		if (chart == null)
			return;
		if (value == null)
			value = 0.0;
		// TODO
		scheduleRefresh();
	}
	
	public void setInvertXY(Boolean value) {
		if (chart == null)
			return;
		if (value == null)
			value = Boolean.FALSE;
		CategoryPlot plot = (CategoryPlot)chart.getPlot();
		plot.setOrientation(value ? PlotOrientation.HORIZONTAL : PlotOrientation.VERTICAL);
		scheduleRefresh();
	}
	
	private Axis getDomainAxis() {
		if (chart != null) {
			Plot plot = chart.getPlot();
			if (plot instanceof CategoryPlot)
				return ((CategoryPlot)plot).getDomainAxis();
			else if (plot instanceof XYPlot)
				return ((XYPlot)plot).getDomainAxis();
		}
		return null;
	}

	private ValueAxis getRangeAxis() {
		if (chart != null) {
			Plot plot = chart.getPlot();
			if (plot instanceof CategoryPlot)
				return ((CategoryPlot)plot).getRangeAxis();
			else if (plot instanceof XYPlot)
				return ((XYPlot)plot).getRangeAxis();
		}
		return null;
	}
}
