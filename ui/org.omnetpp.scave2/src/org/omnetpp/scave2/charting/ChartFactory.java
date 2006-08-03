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
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

/**
 * Factory for scalar and vector charts. 
 */
public class ChartFactory {
	
	public static InteractiveChart createChart(Composite parent, Chart chart, ResultFileManagerEx manager) {
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		switch (dataset.getType().getValue()) {
		case DatasetType.SCALAR: return createScalarChart(parent, chart, dataset, manager);
		case DatasetType.VECTOR: return createVectorChart(parent, chart, dataset, manager);
		case DatasetType.HISTOGRAM: return createHistogramChart(parent, chart, dataset, manager);
		}
		throw new RuntimeException("invalid or unset dataset 'type' attribute: "+dataset.getType()); //XXX proper error handling
	}
	
	public static InteractiveChart createScalarChart(Composite parent, Chart chart, Dataset dataset, ResultFileManagerEx manager) {
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart);
		return createScalarChart(parent, idlist, manager);
	}
	
	public static InteractiveChart createScalarChart(Composite parent, IDList idlist, ResultFileManagerEx manager) {
		InteractiveChart chart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyScalarJFreeChart();
		CategoryDataset dataset = createChartWithRunsOnXAxis(idlist, manager);
		chart.setChart(jfreechart);
		jfreechart.getCategoryPlot().setDataset(dataset);
		if (dataset.getRowCount() <= 5)
			addLegend(jfreechart);
		return chart;
	}
	
	public static InteractiveChart createVectorChart(Composite parent, Chart chart, Dataset dataset, ResultFileManagerEx manager) {
		XYDataset data = new OutputVectorDataset(DatasetManager.getDataFromDataset(manager, dataset, chart));
		InteractiveChart interactiveChart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyVectorJFreeChart();
		interactiveChart.setChart(jfreechart);
		jfreechart.getXYPlot().setDataset(data);
		if (data.getSeriesCount() <= 5)
			addLegend(jfreechart);
		
		return interactiveChart;
	}
	
	public static InteractiveChart createHistogramChart(Composite parent, Chart chart, Dataset dataset, ResultFileManagerEx manager) {
		return null; // TODO
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
	
	private static CategoryDataset createChartWithRunsOnXAxis(IDList idlist, ResultFileManagerEx manager) {
		DefaultCategoryDataset ds = new DefaultCategoryDataset();

		int sz = (int)idlist.size();
		for (int i=0; i<sz; i++) {
			ScalarResult d = manager.getScalar(idlist.get(i));
			ds.addValue(d.getValue(),
					d.getFileRun().getRun().getRunName(),
					d.getModuleName()+"\n"+d.getName());
		}
		return ds;
	}
}
