package org.omnetpp.scave2.charting;

import java.awt.Color;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleInsets;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;

public class ChartManager {

	public static InteractiveChart createScalarChart(Composite parent, IDList idlist, ResultFileManager manager) {
		InteractiveChart chart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyScalarJFreeChart();
		CategoryDataset dataset = ChartHelper.createChartWithRunsOnXAxis(idlist, manager);
		chart.setChart(jfreechart);
		jfreechart.getCategoryPlot().setDataset(dataset);
		return chart;
	}
	
	public static InteractiveChart createVectorChart(Composite parent, IDList idlist, ResultFileManager manager) {
		InteractiveChart chart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyVectorJFreeChart();
		XYDataset dataset = ChartHelper.createXYDataSet(idlist, manager);
		chart.setChart(jfreechart);
		jfreechart.getXYPlot().setDataset(dataset);
		return chart;
	}
	
	private static JFreeChart createEmptyScalarJFreeChart() {
		DefaultCategoryDataset categorydataset = new DefaultCategoryDataset();
		JFreeChart jfreechart = ChartFactory.createBarChart3D(
				"Title", "Category", "Value",
				categorydataset, PlotOrientation.VERTICAL,
				true, true, false);
		CategoryPlot categoryplot = jfreechart.getCategoryPlot();
		CategoryItemRenderer categoryitemrenderer = categoryplot.getRenderer();
		categoryitemrenderer.setItemLabelsVisible(true);
		BarRenderer barrenderer = (BarRenderer)categoryitemrenderer;
		barrenderer.setMaximumBarWidth(0.05D);
		
		return jfreechart;
	}
	
	private static JFreeChart createEmptyVectorJFreeChart() {
		// create dataset
		XYSeriesCollection xyseriescollection = new XYSeriesCollection();

		// create chart
		JFreeChart jfreechart = ChartFactory.createXYLineChart(
				"Line Chart", "X", "Y",
				xyseriescollection, PlotOrientation.VERTICAL,
				true, true, false);
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
}
