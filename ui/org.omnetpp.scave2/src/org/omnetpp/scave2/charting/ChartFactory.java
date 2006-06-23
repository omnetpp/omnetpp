package org.omnetpp.scave2.charting;

import java.awt.Color;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleEdge;
import org.jfree.ui.RectangleInsets;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.DatasetType;

/**
 * Factory for scalar and vector charts. 
 */
public class ChartFactory {
	
	public static InteractiveChart createChart(Composite parent, DatasetType type, IDList idlist, ResultFileManagerEx manager) {
		if (type == DatasetType.SCALAR_LITERAL)
			return createScalarChart(parent, idlist, manager);
		else if (type == DatasetType.VECTOR_LITERAL)
			return createVectorChart(parent, idlist, manager);
		else if (type == DatasetType.HISTOGRAM_LITERAL)
			return createHistogramChart(parent, idlist, manager);
		else
			throw new RuntimeException("invalid or unset dataset 'type' attribute: "+type); //XXX proper error handling
	}

	public static InteractiveChart createScalarChart(Composite parent, IDList idlist, ResultFileManagerEx manager) {
		InteractiveChart chart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyScalarJFreeChart();
		CategoryDataset dataset = ChartHelper.createChartWithRunsOnXAxis(idlist, manager);
		chart.setChart(jfreechart);
		jfreechart.getCategoryPlot().setDataset(dataset);
		if (dataset.getRowCount() <= 5)
			addLegend(jfreechart);
		return chart;
	}
	
	public static InteractiveChart createVectorChart(Composite parent, IDList idlist, ResultFileManagerEx manager) {
		InteractiveChart chart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyVectorJFreeChart();
		XYDataset dataset = ChartHelper.createXYDataSet(idlist, manager);
		chart.setChart(jfreechart);
		jfreechart.getXYPlot().setDataset(dataset);
		if (dataset.getSeriesCount() <= 5)
			addLegend(jfreechart);
		return chart;
	}
	
	public static InteractiveChart createHistogramChart(Composite parent, IDList idlist, ResultFileManagerEx manager) {
		// TODO
		return null;
	}
	
	private static JFreeChart createEmptyScalarJFreeChart() {
		DefaultCategoryDataset categorydataset = new DefaultCategoryDataset();
		JFreeChart jfreechart = org.jfree.chart.ChartFactory.createBarChart3D(
				"Title", "Category", "Value",
				categorydataset, PlotOrientation.VERTICAL,
				false, false, false);
		CategoryPlot categoryplot = jfreechart.getCategoryPlot();
		CategoryItemRenderer categoryitemrenderer = categoryplot.getRenderer();
		categoryitemrenderer.setItemLabelsVisible(true);
		BarRenderer barrenderer = (BarRenderer)categoryitemrenderer;
		barrenderer.setMaximumBarWidth(0.05D);
		
		return jfreechart;
	}
	
	private static JFreeChart createEmptyVectorJFreeChart() {
		XYSeriesCollection xyseriescollection = new XYSeriesCollection();
		JFreeChart jfreechart = org.jfree.chart.ChartFactory.createXYLineChart(
				"Line Chart", "X", "Y",
				xyseriescollection, PlotOrientation.VERTICAL,
				false, false, false);
		jfreechart.setAntiAlias(false);
		jfreechart.setBackgroundPaint(Color.white);
		XYPlot xyplot = (XYPlot)jfreechart.getPlot();
		xyplot.setBackgroundPaint(Color.lightGray);
		xyplot.setAxisOffset(new RectangleInsets(5D, 5D, 5D, 5D));
		xyplot.setDomainGridlinePaint(Color.white);
		xyplot.setRangeGridlinePaint(Color.white);
		XYLineAndShapeRenderer xylineandshaperenderer = (XYLineAndShapeRenderer)xyplot.getRenderer();
		xylineandshaperenderer.setShapesVisible(false);
		xylineandshaperenderer.setShapesFilled(false);
		NumberAxis numberaxis = (NumberAxis)xyplot.getRangeAxis();
		numberaxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
		return jfreechart;
	}
	
	private static void addLegend(JFreeChart jfreechart) {
        LegendTitle legend = new LegendTitle(jfreechart.getPlot());
        legend.setMargin(new RectangleInsets(1.0, 1.0, 1.0, 1.0));
        legend.setBorder(new BlockBorder());
        legend.setBackgroundPaint(Color.white);
        legend.setPosition(RectangleEdge.BOTTOM);
        jfreechart.addSubtitle(legend);
	}
}
