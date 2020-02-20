package org.omnetpp.scave.jobs;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobGroup;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleManager;
import org.eclipse.ui.console.IOConsole;
import org.eclipse.ui.console.IOConsoleOutputStream;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartProvider;
import org.omnetpp.scave.editors.ResultsProvider;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;


public class ChartExport {

    private static class ChartExportJob extends Job {
        private Chart chart;
        private Map<String, String> extraProperties;
        private ResultFileManager manager;

        public ChartExportJob(Chart chart, Map<String, String> extraProperties, ResultFileManager manager) {
            super("Exporting '" + chart.getName() + "'");
            this.chart = chart;
            this.extraProperties = extraProperties;
            this.manager = manager;
        }

        @Override
        protected IStatus run(final IProgressMonitor monitor) {
            try {
                ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                    @Override
                    public Object call() throws Exception {
                        PythonProcessPool processPool = new PythonProcessPool(1);
                        processPool.setShouldSetOmnetppMplBackend(false);
                        runChartScript(chart, extraProperties, processPool, manager, monitor);
                        return null;
                    }
                });
                return Status.OK_STATUS;
            }
            catch (RuntimeException e) {
                return ScavePlugin.getErrorStatus(0, "Failed to export chart", e);
            }
            finally {
                monitor.done();
            }
        }
    }

    private static class BatchChartExportJob extends Job {
        private List<Chart> charts;
        private Map<String, String> extraProperties;
        private ResultFileManager manager;
        private int numConcurrentProcesses = 2; //TODO as param
        private boolean stopOnError = false;

        public BatchChartExportJob(List<Chart> charts, Map<String, String> extraProperties, ResultFileManager manager) {
            super("Exporting " + charts.size());
            this.charts = charts;
            this.extraProperties = extraProperties;
            this.manager = manager;
        }

        @Override
        protected IStatus run(IProgressMonitor monitor) {
            JobGroup jobGroup = new JobGroup("Simulation batch", numConcurrentProcesses, charts.size()) {
                @Override
                protected boolean shouldCancel(IStatus lastCompletedJobResult, int numberOfFailedJobs, int numberOfCanceledJobs) {
                    return numberOfCanceledJobs > 0 || (stopOnError && numberOfFailedJobs > 0);
                }
            };
            for (Chart chart : charts) {
                Job job1 = new ChartExportJob(chart, extraProperties, manager);
                job1.setJobGroup(jobGroup);
                job1.setPriority(Job.BUILD);
                job1.setSystem(false);
                job1.schedule();
            }
            try {
                jobGroup.join(0, monitor);
            }
            catch (OperationCanceledException | InterruptedException e) {
                Debug.println("Cancelling group");
                jobGroup.cancel();
                return Status.CANCEL_STATUS;
            }
            return Status.OK_STATUS;
        }

    };

    protected static void startExportJob(Chart chart, Map<String, String> extraProperties, ResultFileManager manager) {
        chart = (Chart)chart.dup(); // since job runs in another thread, and we don't want locking
        Job job = new ChartExportJob(chart, extraProperties, manager);
        job.setPriority(Job.BUILD);
        job.setSystem(false);
        job.setUser(true);
        job.schedule();
    }

    protected static void startBatchExportJob(List<Chart> charts, Map<String, String> extraProperties, ResultFileManager manager) {
        charts = CollectionUtils.getDeepCopyOf(charts); // since job runs in another thread, and we don't want locking
        Job job = new BatchChartExportJob(charts, extraProperties, manager);
        job.setPriority(Job.BUILD);
        job.setSystem(false);
        job.setUser(true);
        job.schedule();
    }

    public static void exportChartImage(Chart chart, IContainer folder, String format, ResultFileManager manager) {
        Map<String, String> extraProperties = new HashMap<>();
        extraProperties.put("export_image", "true");
        extraProperties.put("image_export_folder", folder.getLocation().toOSString());
        extraProperties.put("image_export_format", format);
        startExportJob(chart, extraProperties, manager);
        //TODO folder.refreshLocal(IResource.DEPTH_INFINITE, monitor); // because we're creating the file behind Eclipse's back
    }

    public static void exportChartImages(List<Chart> charts, IContainer folder, String format, ResultFileManager manager) {
        Map<String, String> extraProperties = new HashMap<>();
        extraProperties.put("export_image", "true");
        extraProperties.put("image_export_folder", folder.getLocation().toOSString());
        extraProperties.put("image_export_format", format);
        startBatchExportJob(charts, extraProperties, manager);
        //TODO folder.refreshLocal(IResource.DEPTH_INFINITE, monitor); // because we're creating the file behind Eclipse's back
    }

    private static void runChartScript(Chart chart, Map<String,String> extraProperties, PythonProcessPool processPool, ResultFileManager rfm, IProgressMonitor monitor) {

        IOConsole console = new IOConsole("'" + chart.getName() + "' - chart script output", null);
        IConsoleManager consoleManager = ConsolePlugin.getDefault().getConsoleManager();
        consoleManager.addConsoles(new IConsole[] { console });
        IOConsoleOutputStream outputStream = console.newOutputStream();
        IOConsoleOutputStream errorStream = console.newOutputStream();
        errorStream.setColor(new Color(Display.getCurrent(), 220, 10, 10));

        PythonProcess proc = processPool.getProcess();

        IOutputListener outputListener = (output, err) -> {
            try {
                if (err)
                    errorStream.write(output);
                else
                    outputStream.write(output);
            }
            catch (IOException e) {
                ScavePlugin.logError(e);
            }
        };

        proc.outputMonitoringThread.addOutputListener(outputListener);
        proc.errorMonitoringThread.addOutputListener(outputListener);

        proc.getEntryPoint().setResultsProvider(new ResultsProvider(rfm, proc.getInterruptedFlag()));
        proc.getEntryPoint().setChartProvider(new ChartProvider(chart, extraProperties));

        final boolean[] executionDone = new boolean[] { false };
        Runnable runAfterDone = () -> {
            executionDone[0] = true;
        };

        ExceptionHandler runAfterError = (p, e) -> {
            try {
                errorStream.write(e.getMessage()); // TODO tweakStackTrace
            } catch (IOException e1) {
                ScavePlugin.logError(e);
            }
            executionDone[0] = true;
        };

        proc.pythonCallerThread.asyncExec(() -> {
            //proc.getEntryPoint().execute("import os; os.chdir(r\"\"\"" + workingDir.getAbsolutePath() + "\"\"\"); del os;");
            //proc.getEntryPoint().execute("import site; site.addsitedir(r\"\"\"" + workingDir.getAbsolutePath() + "\"\"\"); del site;");
            proc.getEntryPoint().execute(chart.getScript());
        }, runAfterDone, runAfterError);

        while (!executionDone[0]) {
            try {
                Thread.sleep(2000);
            } catch (InterruptedException e1) {
                // ignore
            }
            if (monitor.isCanceled())
                break;
        }

        proc.kill();
        processPool.dispose();

    }


}
