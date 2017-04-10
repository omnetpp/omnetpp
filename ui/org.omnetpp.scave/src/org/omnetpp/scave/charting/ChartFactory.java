/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IDataset;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model2.DatasetManager;

/**
 * Factory for scalar and vector charts.
 */
public class ChartFactory {


    public static IChartView createChart(Composite parent, Chart chart, ResultFileManager manager) {
        return createChart(parent, chart, manager, null);
    }

    public static IChartView createChart(Composite parent, Chart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        if (chart instanceof BarChart)
            return createScalarChart(parent, (BarChart)chart, manager, datasetJobListener);
        if (chart instanceof LineChart)
            return createVectorChart(parent, (LineChart)chart, manager, datasetJobListener);
        if (chart instanceof HistogramChart)
            return createHistogramChart(parent, (HistogramChart)chart, manager, datasetJobListener);
        if (chart instanceof ScatterChart)
            return createScatterChart(parent, (ScatterChart)chart, manager, datasetJobListener);
        throw new RuntimeException("unknown chart type");
    }

    public static void populateChart(IChartView chartView, Chart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        if (chart instanceof BarChart)
            populateScalarChart((BarChart)chart, manager, (ScalarChart)chartView, datasetJobListener);
        else if (chart instanceof LineChart)
            populateVectorChart((LineChart)chart, manager, (VectorChart)chartView, datasetJobListener);
        else if (chart instanceof HistogramChart)
            populateHistogramChart((HistogramChart)chart, manager, (HistogramChartCanvas)chartView, datasetJobListener);
        else if (chart instanceof ScatterChart)
            populateScatterChart((ScatterChart)chart, manager, (VectorChart)chartView, datasetJobListener);
        else
            throw new RuntimeException("unknown chart type");
    }

    public static ScalarChart createScalarChart(Composite parent, BarChart chart, ResultFileManager manager) {
        return createScalarChart(parent, chart, manager, null);
    }

    public static ScalarChart createScalarChart(Composite parent, BarChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        ScalarChart scalarChart = new ScalarChart(parent, SWT.DOUBLE_BUFFERED);
        setChartProperties(chart, scalarChart);

        populateScalarChart(chart, manager, scalarChart, datasetJobListener);

        return scalarChart;
    }

    public static VectorChart createVectorChart(Composite parent, LineChart chart, ResultFileManager manager) {
        return createVectorChart(parent, chart, manager, null);
    }

    public static VectorChart createVectorChart(Composite parent, LineChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final VectorChart vectorChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
        vectorChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, vectorChart);

        populateVectorChart(chart, manager, vectorChart, datasetJobListener);

        return vectorChart;
    }

    public static HistogramChartCanvas createHistogramChart(Composite parent, HistogramChart chart, ResultFileManager manager) {
        return createHistogramChart(parent, chart, manager, null);
    }

    public static HistogramChartCanvas createHistogramChart(Composite parent, HistogramChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final HistogramChartCanvas histogramChart = new HistogramChartCanvas(parent, SWT.DOUBLE_BUFFERED);
        histogramChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, histogramChart);
        populateHistogramChart(chart, manager, histogramChart, datasetJobListener);
        return histogramChart;
    }

    public static VectorChart createScatterChart(Composite parent, ScatterChart chart, ResultFileManager manager) {
        return createScatterChart(parent, chart, manager, null);
    }

    public static VectorChart createScatterChart(Composite parent, ScatterChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final VectorChart scatterChart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
        scatterChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, scatterChart);
        populateScatterChart(chart, manager, scatterChart, datasetJobListener);
        return scatterChart;
    }

    public static void populateScalarChart(final BarChart chart, final ResultFileManager manager, ScalarChart scalarChart, IJobChangeListener datasetJobListener) {
        // perform:
        // scalarChart.setDataset(DatasetManager.createScalarDataset(chart, manager, null));
        // but as a background job:
        //
        startDatasetEvaluationJob(scalarChart, new IDatasetCalculation() {
            @Override
            public IDataset run(final IProgressMonitor progressMonitor) {
                return ResultFileManager.callWithReadLock(manager, new Callable<IDataset>() {
                    @Override
                    public IDataset call() throws Exception {
                        return DatasetManager.createScalarDataset(chart, manager, progressMonitor);
                    }
                });
            }
        }, datasetJobListener);
    }

    public static void populateVectorChart(final LineChart chart, final ResultFileManager manager, final VectorChart vectorChart, IJobChangeListener datasetJobListener) {
        // perform:
        // vectorChart.setDataset(DatasetManager.createVectorDataset(chart, manager));
        // but as a background job:
        //
        startDatasetEvaluationJob(vectorChart, new IDatasetCalculation() {
            @Override
            public IDataset run(final IProgressMonitor progressMonitor) {
                return ResultFileManager.callWithReadLock(manager, new Callable<IDataset>() {
                    @Override
                    public IDataset call() {
                        return DatasetManager.createVectorDataset(chart, manager, progressMonitor);
                    }
                });
            }
        }, datasetJobListener);
    }

    public static void populateHistogramChart(final HistogramChart chart, final ResultFileManager manager, final HistogramChartCanvas histogramChart, IJobChangeListener datasetJobListener) {
        // perform:
        // vectorChart.setDataset(DatasetManager.createVectorDataset(chart, manager));
        // but as a background job:
        //
        startDatasetEvaluationJob(histogramChart, new IDatasetCalculation() {
            @Override
            public IDataset run(final IProgressMonitor progressMonitor) {
                return ResultFileManager.callWithReadLock(manager, new Callable<IDataset>() {
                    @Override
                    public IDataset call() {
                        return DatasetManager.createHistogramDataset(chart, manager, progressMonitor);
                    }
                });
            }
        }, datasetJobListener);
    }

    public static void populateScatterChart(final ScatterChart chart, final ResultFileManager manager, final VectorChart scatterChart, IJobChangeListener datasetJobListener) {
        // perform:
        // scatterChart.setDataset(DatasetManager.createScatterPlotDataset(chart, manager));
        // but as a background job:
        //
        startDatasetEvaluationJob(scatterChart, new IDatasetCalculation() {
            @Override
            public IDataset run(final IProgressMonitor progressMonitor) {
                return ResultFileManager.callWithReadLock(manager, new Callable<IDataset>() {
                    @Override
                    public IDataset call() {
                        return DatasetManager.createScatterPlotDataset(chart, manager, progressMonitor);
                    }
                });
            }
        }, datasetJobListener);
    }

    interface IDatasetCalculation {
        public IDataset run(IProgressMonitor progressMonitor);
    }

    protected static void startDatasetEvaluationJob(final IChartView chartView, final IDatasetCalculation calc, IJobChangeListener datasetJobListener) {
        //
        // FIXME Handle concurrency issues! Model must be locked against modification while
        // the background job is working with the dataset. Build locking into CommandStack.execute()?
        // Making the model temporarily readonly could also be a solution, but I didn't find such
        // method. (Andras)
        //
        chartView.setStatusText("Please wait...");
        //chartCanvas.setDataset(null);
        final Job job = new Job("Evaluating dataset...") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    // calculate
                    long startTime = System.currentTimeMillis();
                    final IDataset data = calc.run(monitor);
                    Debug.println("total dataset creation: "+(System.currentTimeMillis()-startTime)+" ms");

                    if (monitor.isCanceled()) {
                        setChartStatusText(chartView, "Chart update cancelled.");
                        return Status.CANCEL_STATUS;
                    }

                    // we're a non-UI thread, so we need to use display.asyncExec() to set the results.
                    // Note that the chart page may have been close since, so we need to check isDisposed() too.
                    Display.getDefault().asyncExec(new Runnable() {
                        @Override
                        public void run() {
                            if (!chartView.getCanvas().isDisposed()) {
                                chartView.setStatusText(null);
                                chartView.setDataset(data);
                            }
                        }});
                    return Status.OK_STATUS;
                }
                catch (Throwable e) {
                    setChartStatusText(chartView, "An error occurred during dataset processing.");
                    return new Status(IStatus.ERROR, ScavePlugin.PLUGIN_ID, 0, "An error occurred during dataset processing.", e);
                }
            }

            private void setChartStatusText(final IChartView chartCanvas, final String text) {
                // we're a non-UI thread, so we need to use display.asyncExec()...
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        if (!chartCanvas.getCanvas().isDisposed())
                            chartCanvas.setStatusText(text);
                    }});
            }
        };

        chartView.getCanvas().addDisposeListener(new DisposeListener() {
            @Override
            public void widgetDisposed(DisposeEvent e) {
                job.cancel();
            }
        });

        job.setPriority(Job.INTERACTIVE); // high priority
        if (datasetJobListener != null)
            job.addJobChangeListener(datasetJobListener);
        job.schedule();
    }

    private static void setChartProperties(Chart chart, IChartView chartView) {
        List<Property> properties = chart.getProperties();
        for (Property property : properties) {
            chartView.setProperty(property.getName(), property.getValue());
        }
    }
}
