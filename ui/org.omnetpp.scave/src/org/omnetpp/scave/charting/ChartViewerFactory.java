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
import org.omnetpp.scave.script.ScriptEngine;

/**
 * Factory for scalar and vector chart viewers.
 */
public class ChartViewerFactory {


    public static IChartView createChartViewer(Composite parent, Chart chart, ResultFileManager manager) {
        return createChartViewer(parent, chart, manager, null);
    }

    public static IChartView createChartViewer(Composite parent, Chart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        if (chart instanceof BarChart)
            return createScalarChartViewer(parent, (BarChart)chart, manager, datasetJobListener);
        if (chart instanceof LineChart)
            return createVectorChartViewer(parent, (LineChart)chart, manager, datasetJobListener);
        if (chart instanceof HistogramChart)
            return createHistogramChartViewer(parent, (HistogramChart)chart, manager, datasetJobListener);
        if (chart instanceof ScatterChart)
            return createScatterChartViewer(parent, (ScatterChart)chart, manager, datasetJobListener);
        throw new RuntimeException("unknown chart type");
    }

    public static ScalarChartViewer createScalarChartViewer(Composite parent, BarChart chart, ResultFileManager manager) {
        return createScalarChartViewer(parent, chart, manager, null);
    }

    public static ScalarChartViewer createScalarChartViewer(Composite parent, BarChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        ScalarChartViewer scalarChart = new ScalarChartViewer(parent, SWT.DOUBLE_BUFFERED);
        setChartProperties(chart, scalarChart);

        //populateChart(scalarChart, chart, manager, datasetJobListener);

        return scalarChart;
    }

    public static VectorChartViewer createVectorChartViewer(Composite parent, LineChart chart, ResultFileManager manager) {
        return createVectorChartViewer(parent, chart, manager, null);
    }

    public static VectorChartViewer createVectorChartViewer(Composite parent, LineChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final VectorChartViewer vectorChart = new VectorChartViewer(parent, SWT.DOUBLE_BUFFERED);
        vectorChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, vectorChart);

        //populateChart(vectorChart, chart, manager, datasetJobListener);

        return vectorChart;
    }

    public static HistogramChartViewer createHistogramChartViewer(Composite parent, HistogramChart chart, ResultFileManager manager) {
        return createHistogramChartViewer(parent, chart, manager, null);
    }

    public static HistogramChartViewer createHistogramChartViewer(Composite parent, HistogramChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final HistogramChartViewer histogramChart = new HistogramChartViewer(parent, SWT.DOUBLE_BUFFERED);
        histogramChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, histogramChart);
        //populateChart(histogramChart, chart, manager, datasetJobListener);
        return histogramChart;
    }

    public static VectorChartViewer createScatterChartViewer(Composite parent, ScatterChart chart, ResultFileManager manager) {
        return createScatterChartViewer(parent, chart, manager, null);
    }

    public static VectorChartViewer createScatterChartViewer(Composite parent, ScatterChart chart, ResultFileManager manager, IJobChangeListener datasetJobListener) {
        final VectorChartViewer scatterChart = new VectorChartViewer(parent, SWT.DOUBLE_BUFFERED);
        scatterChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        setChartProperties(chart, scatterChart);
        //populateChart(scatterChart, chart, manager, datasetJobListener);
        return scatterChart;
    }


    /*
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
    */

    private static void setChartProperties(Chart chart, IChartView chartView) {
        List<Property> properties = chart.getProperties();
        for (Property property : properties) {
            chartView.setProperty(property.getName(), property.getValue());
        }
    }
}
