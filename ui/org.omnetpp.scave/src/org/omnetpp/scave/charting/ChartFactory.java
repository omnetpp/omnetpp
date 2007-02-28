package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Factory for scalar and vector charts.
 */
public class ChartFactory {

	public static ChartCanvas createChart(Composite parent, Chart chart, ResultFileManager manager) {
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		if (chart instanceof BarChart)
			return createScalarChart(parent, chart, dataset, manager);
		if (chart instanceof LineChart)
			return createVectorChart(parent, chart, dataset, manager);
		if (chart instanceof HistogramChart)
			return createHistogramChart(parent, chart, dataset, manager);
		throw new RuntimeException("unknown chart type");
	}

	private static ScalarChart createScalarChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		ScalarChart scalarChart = new ScalarChart(parent, SWT.NONE);
		// set chart data
		CategoryDataset categoryDataset = DatasetManager.createScalarDataset(chart, dataset, manager);
		scalarChart.setDataset(categoryDataset);
		// set chart properties
		setChartProperties(chart, scalarChart);
		return scalarChart;
	}

	private static VectorChart createVectorChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		OutputVectorDataset data = DatasetManager.createVectorDataset(chart, dataset, manager);

		final VectorChart vectorChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		vectorChart.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,true));
		vectorChart.setDataset(data);
		setChartProperties(chart, vectorChart);

		vectorChart.setBackground(ColorConstants.white);
		return vectorChart;
	}

	private static ChartCanvas createHistogramChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		return null; //TODO
	}

	private static void setChartProperties(Chart chart, ChartCanvas chartView) {
		List<Property> properties = (List<Property>)chart.getProperties();
		for (Property property : properties) {
			chartView.setProperty(property.getName(), property.getValue());
		}
	}
}
