/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;

import java.io.IOException;
import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.DebugException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.IStreamListener;
import org.eclipse.debug.core.model.IProcess;
import org.eclipse.debug.core.model.IStreamMonitor;
import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.internal.ui.preferences.IDebugPreferenceConstants;
import org.eclipse.debug.internal.ui.views.console.ProcessConsole;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.console.IOConsoleOutputStream;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * A job to launch a single simulation process in the background
 *
 * @author rhornig
 */
public class SimulationLauncherJob extends Job {
    public final static String SIMULATION_JOB_FAMILY = "simulation";
    ILaunchConfiguration configuration;
    ILaunch launch;
    Integer runNo;
    String taskName;
    boolean reportProgress;
    int port;

    public SimulationLauncherJob(ILaunchConfiguration configuration, ILaunch launch, Integer runNo, boolean reportProgress, int port) {
        super("Simulating "+configuration.getName());
        this.configuration = configuration;
        this.launch = launch;
        this.runNo = runNo;
        this.taskName = "Run #"+runNo;
        this.reportProgress = reportProgress;
        this.port = port;
    }

    /* (non-Javadoc)
     * @see org.eclipse.core.runtime.jobs.Job#belongsTo(java.lang.Object)
     * We are using the launch object as a group family, so we can cancel all processes associated with
     * the launch.
     */
    @Override
    public boolean belongsTo(Object family) {
        return family == launch || SIMULATION_JOB_FAMILY.equals(family);
    }

    /**
     * Launches a single instance of the simulation process
     * @param monitor the progress monitor to use for reporting progress to the user. It is the caller's
     * responsibility to call done() on the given monitor. Accepts null, indicating that no progress should be
     * reported and that the operation cannot be cancelled.
     */
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        final SubMonitor subMonitor = SubMonitor.convert(monitor, taskName, 100);
        subMonitor.subTask("Initializing...");

        try {

            String additionalArgs = " -r "+runNo;
        if (port != -1)
            additionalArgs += " -p "+port;

            // calculate the command-line for display purposes (dump to the console before start)
            String[] cmdLineArgs = OmnetppLaunchUtils.createCommandLine(configuration, additionalArgs);
            IPath workingDir = OmnetppLaunchUtils.getWorkingDirectoryPath(configuration);
            String commandLine = OmnetppLaunchUtils.makeRelativePathTo(new Path(cmdLineArgs[0]), workingDir).toString();
            // if opp_run is used, do not display any path info
            if (commandLine.endsWith("/opp_run"))
                commandLine = "opp_run";
            if (commandLine.endsWith("/opp_run_release"))
                commandLine = "opp_run_release";
            for (int i=1; i<cmdLineArgs.length; ++i)
                commandLine += " " + cmdLineArgs[i];

            // launch the process
            Process process = OmnetppLaunchUtils.startSimulationProcess(configuration, cmdLineArgs, false);
            IProcess iprocess = DebugPlugin.newProcess(launch, process, renderProcessLabel(runNo));
            printToConsole(iprocess, "Starting...\n\n$ cd "+workingDir+"\n$ "+commandLine+"\n\n", false);

            // command line will be visible in the debug view's property dialog
            iprocess.setAttribute(IProcess.ATTR_CMDLINE, commandLine);

            // setup a stream monitor on the process output, so we can track the progress
            if (reportProgress && monitor != null)
                iprocess.getStreamsProxy().getOutputStreamMonitor().addListener(new IStreamListener () {
                    int prevPct = 0;
                    public void streamAppended(String text, IStreamMonitor ismon) {
                        int pct = OmnetppLaunchUtils.getProgressInPercent(text);
                        if (pct >= 0) {
                            subMonitor.worked(pct - prevPct);
                            prevPct = pct;
                        }

                        if (OmnetppLaunchUtils.isWaitingForUserInput(text))
                            subMonitor.subTask("Waiting for user input... (Switch to console)");
                        else
                            subMonitor.subTask("Executing ("+prevPct+"%)");
                    }
                });

            // poll the state of the monitor and terminate the process if cancel was requested
            while (!iprocess.isTerminated()) {
                synchronized (iprocess) {
                    try {
                        iprocess.wait(1000);
                        // check the monitor state on every 1000ms and terminate the process if requested
                        if (subMonitor.isCanceled())
                            iprocess.terminate();
                    } catch (InterruptedException e) {
                        iprocess.terminate();
                    }
                }
            }

            subMonitor.done();

            // do some error reporting if the process finished with error
            if (iprocess.isTerminated() && iprocess.getExitValue() != 0) {
                try {
                    String errorMsg = "\nSimulation terminated with exit code: " + iprocess.getExitValue() + "\n";
                    errorMsg += "Working directory: " + workingDir + "\n";
                    errorMsg += "Command line: " + commandLine + "\n";

                    // add the environment variables we are interested in
                    String environment[] = DebugPlugin.getDefault().getLaunchManager().getEnvironment(configuration);
                    errorMsg += "\nEnvironment variables:\n";
                    for (String env : environment) {
                        // on Windows, environment variable names are case insensitive, so we convert the name to upper case
                        if (Platform.getOS().equals(Platform.OS_WIN32))
                            env = StringUtils.substringBefore(env, "=").toUpperCase() + "=" + StringUtils.substringAfter(env, "=");
                        if (env.startsWith("PATH=") || env.startsWith("LD_LIBRARY_PATH=") || env.startsWith("DYLD_LIBRARY_PATH=")
                                || env.startsWith("OMNETPP_") || env.startsWith("NEDPATH="))
                            errorMsg += env  + "\n";
                    }

                    printToConsole(iprocess, errorMsg, true);
                    if (subMonitor.isCanceled() || iprocess.getExitValue() == 2) {
                        printToConsole(iprocess, "Cancelled by user request.", true);
                        subMonitor.subTask("Cancelled");
                        return Status.CANCEL_STATUS;
                    } else {
                        subMonitor.subTask("Failed");
                        return new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, iprocess.getExitValue(), taskName+": Finished with Error", null);
                    }
                }
                catch (DebugException e) {
                    LaunchPlugin.logError("Process is not yet terminated (should not happen)", e);
                }
            }
        }
        catch (CoreException e) {
            return e.getStatus();
        }

        subMonitor.subTask("Finished");
        return Status.OK_STATUS;
    }

    /**
     * Print something to the process's console output. Error message will be written in red
     * and the console will be brought to focus.
     */
    private void printToConsole(IProcess iprocess, String text, boolean isErrorMessage) {
        try {
            ProcessConsole console = (ProcessConsole)DebugUIPlugin.getDefault().getProcessConsoleManager().getConsole(iprocess);
            if (console != null) {
                final IOConsoleOutputStream stream = console.newOutputStream();
                if (isErrorMessage) {
                    stream.setActivateOnWrite(true);
                    // we have to set the color in the UI thread otherwise SWT will throw an error
                    Display.getDefault().syncExec(new Runnable() {
                        public void run() {
                            stream.setColor(DebugUITools.getPreferenceColor(IDebugPreferenceConstants.CONSOLE_SYS_ERR_COLOR));
                        }
                    });
                }
                stream.write(text);
                stream.close();
            }
        } catch (IOException e) {
            LaunchPlugin.logError("Unable to write to console", e);
        }
    }

    /**
     * @param runNo
     * @return The process label to display with the run number.
     * @throws CoreException
     */
    private String renderProcessLabel(Integer runNo) throws CoreException {
        String progAttr = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, "");
        String expandedProg = StringUtils.substituteVariables(progAttr);
        String format = "{0} ({1} - run #"+runNo +")";
        String timestamp = DateFormat.getInstance().format(new Date(System.currentTimeMillis()));
        return MessageFormat.format(format, new Object[] {expandedProg, timestamp});
    }

}
