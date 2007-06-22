package org.omnetpp.launch;

import java.io.File;
import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.VariablesPlugin;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;

// TODO add correct progress monitoring
public class StandaloneOmnetppSimulationLaunchConfiguration extends LaunchConfigurationDelegate {

    class BatchedSimulationLauncherJob extends Job {
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
        public BatchedSimulationLauncherJob(String jobName, ILaunchConfiguration configuration, String mode, ILaunch launch) {
            this(jobName, configuration, mode, launch, null);
        }

        @Override
        protected IStatus run(IProgressMonitor monitor) {
            try {
                if (runs == null)
                    // execution without run specified
                    launchSimulation(configuration, mode, launch, monitor, null);
                else
                    for(Integer runNo : runs)  // execute all runs
                        launchSimulation(configuration, mode, launch, monitor, runNo);

            } catch (CoreException e) {
                return Status.CANCEL_STATUS;
            }
            return Status.OK_STATUS;
        }

        /**
         * Launches a single instance of the simulation process
         */
        private void launchSimulation(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor, Integer runNo)
                throws CoreException {
            if (monitor == null) {
                monitor = new NullProgressMonitor();
            }
            String name = "Running Simulation" + (runNo == null ? "" : " (run: "+runNo+")");
            monitor.beginTask(name, 100);
            // check for cancellation
            if (monitor.isCanceled())
                return;

            try {
                String wdAttr = LaunchPlugin.getWorkingDirectoryPath(configuration).toString();
                String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
                String argAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, "");
                IStringVariableManager varman = VariablesPlugin.getDefault().getStringVariableManager();
                String expandedWd = varman.performStringSubstitution(wdAttr);
                String expandedProg = varman.performStringSubstitution(progAttr);
                String expandedArg = varman.performStringSubstitution(argAttr);
                if (runNo != null) expandedArg += " -r "+runNo;
                IFile executableFile = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(expandedProg));
                String cmdLine = executableFile.getRawLocation().makeAbsolute().toString() + " " + expandedArg;
                Process process = DebugPlugin.exec(DebugPlugin.parseArguments(cmdLine), new File(expandedWd));
                IProcess iprocess = DebugPlugin.newProcess(launch, process, renderProcessLabel(expandedProg));
                monitor.worked(1);

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
            } finally {
                monitor.done();
            }
        }

        private String renderProcessLabel(String commandLine) {
            String format = "{0} ({1})";
            String timestamp = DateFormat.getInstance().format(new Date(System.currentTimeMillis()));
            return MessageFormat.format(format, new Object[]{commandLine, timestamp});
        }

    }

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
        if (monitor == null) {
            monitor = new NullProgressMonitor();
        }
        monitor.beginTask("Launching Simulation", 1);
//        BatchedSimulationLauncherJob job = new BatchedSimulationLauncherJob("Executing simulation job",configuration, mode, launch, new Integer[] {1 , 2});
        // TODO parse out the run numbers into tn integer string
        BatchedSimulationLauncherJob job = new BatchedSimulationLauncherJob("Executing simulation job",configuration, mode, launch);
        job.schedule();
        monitor.done();
    }

}
