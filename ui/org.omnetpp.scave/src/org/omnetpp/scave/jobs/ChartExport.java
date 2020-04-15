package org.omnetpp.scave.jobs;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

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
import org.eclipse.ui.console.TextConsole;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartProvider;
import org.omnetpp.scave.editors.ResultsProvider;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.pychart.PythonCallerThread.ExceptionHandler;
import org.omnetpp.scave.pychart.PythonOutputMonitoringThread.IOutputListener;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;

/**
 * Runs chart scripts in background jobs, with the purpose of exporting plot graphics, data, etc.
 *
 * @author andras
 */
public class ChartExport {

    private static final String CONSOLE_MARKER_ATTRIBUTE_KEY = "FOR_CHART_EXPORT";

    private static class Context {
        public Context(Map<String, String> extraProperties, File chartsDir, ResultFileManager manager, boolean stopOnError, int numConcurrentProcesses) {
            this.extraProperties = extraProperties;
            this.chartsDir = chartsDir;
            this.manager = manager;
            this.stopOnError = stopOnError;
            this.numConcurrentProcesses = numConcurrentProcesses;
        }
        final Map<String, String> extraProperties;
        final File chartsDir;
        final ResultFileManager manager;
        final boolean stopOnError;
        final int numConcurrentProcesses;
    }

    protected static class ChartExportJob extends Job {
        private Chart chart;
        private Context context;

        public ChartExportJob(Chart chart, Context context) {
            super("Exporting chart '" + chart.getName() + "'");
            this.chart = chart;
            this.context = context;
        }

        @Override
        protected IStatus run(final IProgressMonitor monitor) {
            try {
                ResultFileManager.runWithReadLock(context.manager, () -> {
                    PythonProcessPool processPool = new PythonProcessPool(1);
                    processPool.setShouldSetOmnetppMplBackend(false);
                    runChartScript(chart, processPool, context, monitor);
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

    protected static class BatchChartExportJob extends Job {
        private List<Chart> charts;
        private Context context;

        public BatchChartExportJob(List<Chart> charts, Context context) {
            super("Exporting " + charts.size() + " charts");
            this.charts = charts;
            this.context = context;
        }

        @Override
        protected IStatus run(IProgressMonitor monitor) {
            JobGroup jobGroup = new JobGroup("Exporting charts", context.numConcurrentProcesses, charts.size()) {
                @Override
                protected boolean shouldCancel(IStatus lastCompletedJobResult, int numberOfFailedJobs, int numberOfCanceledJobs) {
                    return numberOfCanceledJobs > 0 || (context.stopOnError && numberOfFailedJobs > 0);
                }
            };
            for (Chart chart : charts) {
                Job job1 = new ChartExportJob(chart, context);
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

    protected static void startExportJob(Chart chart, Context context) {
        chart = (Chart)chart.dup(); // since job runs in another thread, and we don't want locking
        Job job = new ChartExportJob(chart, context);
        job.setPriority(Job.BUILD);
        job.setSystem(false);
        job.setUser(true);
        job.schedule();
    }

    protected static void startBatchExportJob(List<Chart> charts, Context context) {
        charts = CollectionUtils.getDeepCopyOf(charts); // since job runs in another thread, and we don't want locking
        Job job = new BatchChartExportJob(charts, context);
        job.setPriority(Job.BUILD);
        job.setSystem(false);
        job.setUser(true);
        job.schedule();
    }

    public static Map<String, String> makeExtraPropertiesForImageExport(IContainer targetFolder, String format, String dpi) {
        Map<String, String> map = new HashMap<>();
        map.put("export_image", "true");
        map.put("image_export_folder", targetFolder.getLocation().toOSString());
        map.put("image_export_format", format);
        map.put("image_export_dpi", dpi);
        return map;
    }

    public static Map<String, String> makeExtraPropertiesForDataExport(IContainer targetFolder) {
        Map<String, String> map = new HashMap<>();
        map.put("export_data", "true");
        map.put("data_export_folder", targetFolder.getLocation().toOSString());
        return map;
    }

    public static void exportCharts(List<Chart> charts, Map<String, String> extraProperties, File chartsDir, ResultFileManager manager, boolean stopOnError, int numConcurrentProcesses) {
        clearPreviousConsoles();
        Context context = new Context(extraProperties, chartsDir, manager, stopOnError, numConcurrentProcesses);
        if (charts.size() == 1)
            startExportJob(charts.get(0), context);
        else if (charts.size() >= 2)
            startBatchExportJob(charts, context);
        //TODO folder.refreshLocal(IResource.DEPTH_INFINITE, monitor); // because we're creating the file behind Eclipse's back
    }

    public static void exportChart(Chart chart, Map<String, String> extraProperties, File chartsDir, ResultFileManager manager) {
        clearPreviousConsoles();
        Context context = new Context(extraProperties, chartsDir, manager, false, 1);
        startExportJob(chart, context);
        //TODO folder.refreshLocal(IResource.DEPTH_INFINITE, monitor); // because we're creating the file behind Eclipse's back
    }

    private static void clearPreviousConsoles() {
        IConsoleManager consoleManager = ConsolePlugin.getDefault().getConsoleManager();
        for (IConsole console : consoleManager.getConsoles()) {
            if (console instanceof TextConsole) {
                TextConsole textConsole = (TextConsole)console;
                Object marker = textConsole.getAttribute(CONSOLE_MARKER_ATTRIBUTE_KEY);
                if (marker != null) {
                    consoleManager.removeConsoles(new IConsole[] { textConsole });
                }
            }
        }
    }

    private static void runChartScript(Chart chart, PythonProcessPool processPool, Context context, IProgressMonitor monitor) {
        IOConsole console = new IOConsole("'" + chart.getName() + "' - chart export", null);
        console.setAttribute(CONSOLE_MARKER_ATTRIBUTE_KEY, "true");
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

        proc.getEntryPoint().setResultsProvider(new ResultsProvider(context.manager, proc.getInterruptedFlag()));
        proc.getEntryPoint().setChartProvider(new ChartProvider(chart, context.extraProperties));

        final Thread waitingThread = Thread.currentThread();

        final boolean[] executionDone = new boolean[] { false };
        Runnable runAfterDone = () -> {
            executionDone[0] = true;
            waitingThread.interrupt();
        };

        ExceptionHandler runAfterError = (p, e) -> {
            try {
                errorStream.write(StringUtils.defaultIfEmpty(e.getMessage(), e.getClass().getSimpleName())); // TODO tweakStackTrace
            } catch (IOException e1) {
                ScavePlugin.logError(e);
            }
            executionDone[0] = true;
            waitingThread.interrupt();
        };

        proc.pythonCallerThread.asyncExec(() -> {
            // Ensure the chart script can load source files and Python modules from the anf file's directory
            proc.getEntryPoint().execute("import os; os.chdir(r\"\"\"" + context.chartsDir.getAbsolutePath() + "\"\"\"); del os;");
            proc.getEntryPoint().execute("import site; site.addsitedir(r\"\"\"" + context.chartsDir.getAbsolutePath() + "\"\"\"); del site;");
            proc.getEntryPoint().execute(chart.getScript());
        }, runAfterDone, runAfterError);

        while (!executionDone[0]) {
            try {
                if (monitor.isCanceled())
                    break;
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                // check again for completion on next iteration
            }
        }

        proc.kill();
        processPool.dispose();
    }

}
