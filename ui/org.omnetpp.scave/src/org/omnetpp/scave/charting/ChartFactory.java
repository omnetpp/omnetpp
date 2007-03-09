package org.omnetpp.scave.charting;

import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
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

	public static void populateChart(ChartCanvas chartCanvas, Chart chart, Dataset dataset, ResultFileManager manager) {
		if (chart instanceof BarChart)
			populateScalarChart(chart, dataset, manager, (ScalarChart)chartCanvas);
		else if (chart instanceof LineChart)
			populateVectorChart(chart, dataset, manager, (VectorChart)chartCanvas);
		else if (chart instanceof HistogramChart)
			;//TODO
		else
			throw new RuntimeException("unknown chart type");
	}
	
	public static ScalarChart createScalarChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		ScalarChart scalarChart = new ScalarChart(parent, SWT.DOUBLE_BUFFERED);
		setChartProperties(chart, scalarChart);

		populateScalarChart(chart, dataset, manager, scalarChart);
		
		return scalarChart;
	}

	public static VectorChart createVectorChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		final VectorChart vectorChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		vectorChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		setChartProperties(chart, vectorChart);

		populateVectorChart(chart, dataset, manager, vectorChart);

		return vectorChart;
	}

	public static ChartCanvas createHistogramChart(Composite parent, Chart chart, Dataset dataset, ResultFileManager manager) {
		return null; //TODO
	}

	public static void populateScalarChart(final Chart chart, final Dataset dataset, final ResultFileManager manager, ScalarChart scalarChart) {
		// perform:
		// scalarChart.setDataset(DatasetManager.createScalarDataset(chart, dataset, manager, null));
		// but as a background job:
		//
		startDatasetEvaluationJob(scalarChart, new IDatasetCalculation() {
			public org.jfree.data.general.Dataset run(IProgressMonitor progressMonitor) {
				return DatasetManager.createScalarDataset(chart, dataset, manager, progressMonitor);
			}
		});
	}

	public static void populateVectorChart(final Chart chart, final Dataset dataset, final ResultFileManager manager, final VectorChart vectorChart) {
		// perform:
		// vectorChart.setDataset(DatasetManager.createVectorDataset(chart, dataset, manager));
		// but as a background job:
		//
		startDatasetEvaluationJob(vectorChart, new IDatasetCalculation() {
			public org.jfree.data.general.Dataset run(IProgressMonitor progressMonitor) {
				return DatasetManager.createVectorDataset(chart, dataset, manager, progressMonitor);
			}
		});
	}

	interface IDatasetCalculation {
		public org.jfree.data.general.Dataset run(IProgressMonitor progressMonitor);
	}
	
	protected static void startDatasetEvaluationJob(final ChartCanvas chartCanvas, final IDatasetCalculation calc) {
		//
		// FIXME Handle concurrency issues! Model must be locked against modification while 
		// the background job is working with the dataset. Build locking into CommandStack.execute()?
		// Making the model temporarily readonly could also be a solution, but I didn't find such 
		// method. (Andras)
		//
		chartCanvas.setStatusText("Please wait...");
		chartCanvas.setDataset(null);
		Job job = new Job("Evaluating dataset...") {
			protected IStatus run(IProgressMonitor monitor) {
				try {
					// calculate
					long startTime = System.currentTimeMillis();
					final org.jfree.data.general.Dataset data = calc.run(monitor);
					System.out.println("total dataset creation: "+(System.currentTimeMillis()-startTime)+" ms");

					if (monitor.isCanceled()) {
						setChartStatusText(chartCanvas, "Chart update cancelled.");
						return Status.CANCEL_STATUS;
					}
					
					// we're a non-UI thread, so we need to use display.asyncExec() to set the results.
					// Note that the chart page may have been close since, so we need to check isDisposed() too.
					Display.getDefault().asyncExec(new Runnable() {
						public void run() {
							if (!chartCanvas.isDisposed()) {
								chartCanvas.setStatusText(null);
								chartCanvas.setDataset(data);
							}
						}});
					return Status.OK_STATUS;
				} 
				catch (Throwable e) {
					setChartStatusText(chartCanvas, "An error occurred during dataset processing.");
					return new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, 0, "An error occurred during dataset processing.", e);
				}
			}

			private void setChartStatusText(final ChartCanvas chartCanvas, final String text) {
				// we're a non-UI thread, so we need to use display.asyncExec()...
				Display.getDefault().asyncExec(new Runnable() {
					public void run() {
						if (!chartCanvas.isDisposed())
							chartCanvas.setStatusText(text);
					}});
			}
		};
		job.setPriority(Job.INTERACTIVE); // high priority
		job.schedule();
	}

	private static void setChartProperties(Chart chart, ChartCanvas chartView) {
		List<Property> properties = (List<Property>)chart.getProperties();
		for (Property property : properties) {
			chartView.setProperty(property.getName(), property.getValue());
		}
	}
}
