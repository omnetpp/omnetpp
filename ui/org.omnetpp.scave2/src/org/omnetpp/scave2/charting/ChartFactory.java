package org.omnetpp.scave2.charting;

import java.awt.Color;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
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
import org.omnetpp.common.canvas.RubberbandSupport;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.model.ChartProperties;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

/**
 * Factory for scalar and vector charts. 
 */
public class ChartFactory {
	
	public static Control createChart(Composite parent, Chart chart, ResultFileManager manager) {
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		switch (dataset.getType().getValue()) {
		case DatasetType.SCALAR: return createScalarChart(parent, chart, dataset, manager);
		case DatasetType.VECTOR: return createVectorChart2(parent, chart, dataset, manager);
		case DatasetType.HISTOGRAM: return createHistogramChart(parent, chart, dataset, manager);
		}
		throw new RuntimeException("invalid or unset dataset 'type' attribute: "+dataset.getType()); //XXX proper error handling
	}
	
	private static InteractiveChart createScalarChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		ScalarChart interactiveChart = new ScalarChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyScalarJFreeChart(chart.getName(), "Category", "Value");
		interactiveChart.setChart(jfreechart);
		// set chart data
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart);
		CategoryDataset categoryDataset = DatasetManager.createScalarDataset(idlist, manager);
		interactiveChart.setDataset(categoryDataset);
		// set chart properties
		setChartProperties(chart, interactiveChart);
		return interactiveChart;
	}
	
	private static ScalarChart2 createScalarChart2(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		ScalarChart2 scalarChart = new ScalarChart2(parent, SWT.NONE);
		// set chart data
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart);
		CategoryDataset categoryDataset = DatasetManager.createScalarDataset(idlist, manager);
		scalarChart.setDataset(categoryDataset);
		// set chart properties
		setChartProperties(chart, scalarChart);
		return scalarChart;
	}

	private static InteractiveChart createVectorChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		InteractiveChart interactiveChart = new InteractiveChart(parent, SWT.NONE);
		JFreeChart jfreechart = createEmptyVectorJFreeChart(chart.getName(), "X", "Y");
		interactiveChart.setChart(jfreechart);
		// set chart data
		XYDataset data = DatasetManager.createVectorDataset(chart, dataset, manager);
		jfreechart.getXYPlot().setDataset(data);
		if (data.getSeriesCount() <= 5)
			addLegend(jfreechart);
		
		return interactiveChart;
	}

	private static VectorChart createVectorChart2(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		OutputVectorDataset data = DatasetManager.createVectorDataset(chart, dataset, manager);

		final VectorChart vectorChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		vectorChart.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,true));
		vectorChart.setDataset(data);
		vectorChart.setCaching(true);
		setChartProperties(chart, vectorChart);

		vectorChart.setBackground(ColorConstants.white);
		new RubberbandSupport(vectorChart, SWT.CTRL) {
			@Override
			public void rubberBandSelectionMade(Rectangle r) {
				vectorChart.zoomToRectangle(new org.eclipse.draw2d.geometry.Rectangle(r));
			}
		};
		return vectorChart;
	}

	private static InteractiveChart createHistogramChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		InteractiveChart interactiveChart = new InteractiveChart(parent, SWT.NONE);
		// TODO: create JFreeChart
		return interactiveChart;
	}
	
	private static JFreeChart createEmptyScalarJFreeChart(String title, String categoryAxisLabel, String valueAxisLabel) {
		DefaultCategoryDataset categorydataset = new DefaultCategoryDataset();
		JFreeChart jfreechart = org.jfree.chart.ChartFactory.createBarChart3D(
				title, categoryAxisLabel, valueAxisLabel,
				categorydataset, PlotOrientation.VERTICAL,
				false, false, false);
		CategoryPlot categoryplot = jfreechart.getCategoryPlot();
		CategoryItemRenderer categoryitemrenderer = categoryplot.getRenderer();
		categoryitemrenderer.setItemLabelsVisible(true);
		BarRenderer barrenderer = (BarRenderer)categoryitemrenderer;
		barrenderer.setMaximumBarWidth(0.05D);
		
		return jfreechart;
	}
	
	private static JFreeChart createEmptyVectorJFreeChart(String title, String xAxisLabel, String yAxisLabel) {
		XYSeriesCollection xyseriescollection = new XYSeriesCollection();
		JFreeChart jfreechart = org.jfree.chart.ChartFactory.createXYLineChart(
				title, xAxisLabel, yAxisLabel,
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
	
	
	private static void setChartProperties(Chart chart, ScalarChart chartView) {
		ChartProperties chartProperties = ChartProperties.createPropertySource(chart);
		for (IPropertyDescriptor descriptor : chartProperties.getPropertyDescriptors()) {
			String id = (String)descriptor.getId();
			if (chartProperties.isPropertySet(id))
				chartView.setProperty(id, chartProperties.getStringProperty(id));
		}
	}
	
	private static void setChartProperties(Chart chart, VectorChart chartView) {
		ChartProperties chartProperties = ChartProperties.createPropertySource(chart);
		for (IPropertyDescriptor descriptor : chartProperties.getPropertyDescriptors()) {
			String id = (String)descriptor.getId();
			if (chartProperties.isPropertySet(id))
				chartView.setProperty(id, chartProperties.getStringProperty(id));
		}
	}

	private static void setChartProperties(Chart chart, ScalarChart2 chartView) {
		ChartProperties chartProperties = ChartProperties.createPropertySource(chart);
		for (IPropertyDescriptor descriptor : chartProperties.getPropertyDescriptors()) {
			String id = (String)descriptor.getId();
			if (chartProperties.isPropertySet(id))
				chartView.setProperty(id, chartProperties.getStringProperty(id));
		}
	}
}
