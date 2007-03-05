package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.jfree.data.category.CategoryDataset;
import org.omnetpp.scave.ScavePlugin;
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
		ScalarChart scalarChart = new ScalarChart(parent, SWT.DOUBLE_BUFFERED);
		// set chart data
		CategoryDataset categoryDataset = DatasetManager.createScalarDataset(chart, dataset, manager);
		scalarChart.setDataset(categoryDataset);
		// set chart properties
		setChartProperties(chart, scalarChart);
		return scalarChart;
	}

	private static VectorChart createVectorChart(Composite parent, final Chart chart, final Dataset dataset, final ResultFileManager resultFileManager) {
		final VectorChart vectorChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		vectorChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		vectorChart.setStatusText("Please wait...");
		setChartProperties(chart, vectorChart);

		vectorChart.setBackground(ColorConstants.white);  //XXX why here?
		
		// FIXME Handle concurrency issues! Model must be locked against modification while 
		// the background job is working with the dataset. Build locking into CommandStack.execute()?
		// Making the model temporarily readonly could also be a solution, but didn't find such method.
		//
		Job job = new Job("Evaluating dataset...") {
			protected IStatus run(IProgressMonitor monitor) {
				try {
					long startTime = System.currentTimeMillis();
					//XXX update progress monitor
					final OutputVectorDataset data = DatasetManager.createVectorDataset(chart, dataset, resultFileManager);
					System.out.println("total dataset creation: "+(System.currentTimeMillis()-startTime)+" ms");
					Assert.isTrue(false);
					Display.getDefault().asyncExec(new Runnable() {
						public void run() {
							vectorChart.setDataset(data);
						}});
					return Status.OK_STATUS;
				} 
				catch (Throwable e) {
					Display.getDefault().asyncExec(new Runnable() {
						public void run() {
							vectorChart.setStatusText("An error occurred during dataset processing.");
						}});
					return new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, 0, "An error occurred during dataset processing.", e);
				}
			}
		};
		job.setPriority(Job.SHORT);
		job.schedule();

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
