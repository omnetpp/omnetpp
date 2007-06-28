package org.omnetpp.launch;

import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.IStreamListener;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.debug.core.model.IStreamMonitor;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;
import org.eclipse.debug.internal.ui.views.launch.LaunchView;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;

import org.omnetpp.common.util.StringUtils;

public class StandaloneOmnetppSimulationLaunchConfiguration extends LaunchConfigurationDelegate {

    private static class BatchedSimulationLauncherJob extends Job {
        ILaunchConfiguration configuration;
        String mode;
        ILaunch launch;
        IProgressMonitor monitor;
        Integer runs[];

        public BatchedSimulationLauncherJob(String jobName, ILaunchConfiguration configuration, String mode, ILaunch launch, Integer[] runs) {
            super(jobName);
            this.configuration = configuration;
            this.mode = mode;
            this.launch = launch;
            this.runs = runs;
        }

        @Override
        protected IStatus run(IProgressMonitor monitor) {
            SubMonitor smon;
            try {
                if (runs.length == 1) {
                    // execution without run specified
                    smon = SubMonitor.convert(monitor, "Running Simulation", 1);
                    launchSimulation(configuration, mode, launch, smon.newChild(1), runs[0]);
                } else {
                    smon = SubMonitor.convert(monitor, "Running Simulation Batch ("+runs.length+" runs)", runs.length);
                    for(Integer runNo : runs) {  // execute all runs
                        launchSimulation(configuration, mode, launch, smon.newChild(1), runNo);
                        if (monitor.isCanceled())
                            return Status.CANCEL_STATUS;
                    }
                }
            } catch (CoreException e) {
                return Status.CANCEL_STATUS;
            } finally {
                monitor.done();
            }
            return Status.OK_STATUS;
        }

        /**
         * Launches a single instance of the simulation process
         */
        private void launchSimulation(ILaunchConfiguration configuration, String mode, ILaunch launch, final SubMonitor monitor, final Integer runNo)
                throws CoreException {
            // check for cancellation
            if (monitor.isCanceled())
                return;

            monitor.subTask("run #"+runNo);
            monitor.setWorkRemaining(100);

            Process process = LaunchPlugin.startSimulationProcess(configuration, " -r "+runNo);
            IProcess iprocess = DebugPlugin.newProcess(launch, process, renderProcessLabel(runNo));
            // setup a stream monitor on the process output, so we can track the progress
            iprocess.getStreamsProxy().getOutputStreamMonitor().addListener(new IStreamListener () {
                int prevPct = 0;
                public void streamAppended(String text, IStreamMonitor ismon) {
                    String pctStr = StringUtils.substringAfterLast(StringUtils.substringBeforeLast(text, "% completed")," ");
                    if (StringUtils.isNumeric(pctStr)) {
                        try {
                            int pct = Integer.parseInt(pctStr);
                            monitor.worked(pct - prevPct);
                            prevPct = pct;
                        } catch (NumberFormatException e) {}
                    }

                    String runIdentifier = "";
                    if (runNo != null )
                        runIdentifier = "run #"+runNo;

                    if (text.contains("Enter parameter"))
                        monitor.subTask(runIdentifier + " - Waiting for user input... (Switch to console)");
                    else
                        monitor.subTask(runIdentifier+" - Executing");
                }
            });

            refreshDebugView();
            // poll the state of the monitor and terminate the process if cancel was requested
            while (!iprocess.isTerminated()) {
                synchronized (iprocess) {
                    try {
                        iprocess.wait(1000);
                        // check the monitor state on every 1000ms
                        if (monitor.isCanceled())
                            iprocess.terminate();
                    } catch (InterruptedException e) {
                        iprocess.terminate();
                    }
                }
            }
            refreshDebugView();
        }

        /**
         * @param runNo
         * @return The process label to display with the run number.
         * @throws CoreException
         */
        private String renderProcessLabel(Integer runNo) throws CoreException {
            String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
            String expandedProg = VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(progAttr);
            String format = "{0} ({1} - run #"+runNo +")";
            String timestamp = DateFormat.getInstance().format(new Date(System.currentTimeMillis()));
            return MessageFormat.format(format, new Object[] {expandedProg, timestamp});
        }

        /**
         * Forces a refresh of the debug view - for some reason adding/removing processes to the Launch
         * does not correctly refreshes the tree
         */
        private void refreshDebugView() {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                    IViewPart wp = workbenchPage.findView(IDebugUIConstants.ID_DEBUG_VIEW);
                    if (wp instanceof LaunchView)
                        ((LaunchView)wp).getViewer().refresh();
                }
            });
        }

    }

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
        if (monitor == null) {
            monitor = new NullProgressMonitor();
        }
        monitor.beginTask("Launching Simulation", 1);

        Integer runs[] = LaunchPlugin.parseRuns(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_RUN, ""),
                                                LaunchPlugin.getMaxNumberOfRuns(configuration));
        if (runs == null)
            throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID,"Invalid run numbers specified"));

        BatchedSimulationLauncherJob job = new BatchedSimulationLauncherJob("Executing simulation job", configuration, mode, launch, runs);
        job.schedule();
        monitor.done();
    }

}
