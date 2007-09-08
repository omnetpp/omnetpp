package org.omnetpp.launch;

import java.text.DateFormat;
import java.text.MessageFormat;
import java.util.Date;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
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
import org.eclipse.debug.internal.ui.views.launch.LaunchView;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;

import org.omnetpp.common.util.StringUtils;

/**
 * A job to launch a single simulation process in the background
 *
 * @author rhornig
 */
public class SimulationLauncherJob extends Job {
    ILaunchConfiguration configuration;
    ILaunch launch;
    Integer runNo;
    boolean waitForFinish;

    public SimulationLauncherJob(ILaunchConfiguration configuration, ILaunch launch, Integer runNo, boolean waitForFinish) {
        super("Running simulation");
        this.configuration = configuration;
        this.launch = launch;
        this.runNo = runNo;
        this.waitForFinish = waitForFinish;
    }

    /* (non-Javadoc)
     * @see org.eclipse.core.runtime.jobs.Job#belongsTo(java.lang.Object)
     * We are using the launch object as a group family, so we can cancel all process associated with
     * the launch.
     */
    @Override
    public boolean belongsTo(Object family) {
        return family == launch;
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        SubMonitor smon;
        try {
        	smon = SubMonitor.convert(monitor, "", 1);
        	launchSimulation(configuration, launch, smon.newChild(1), runNo);
        } 
        catch (CoreException e) {
            return Status.CANCEL_STATUS;
        } finally {
            monitor.done();
        }
        return Status.OK_STATUS;
    }

    /**
     * Launches a single instance of the simulation process
     */
    private void launchSimulation(ILaunchConfiguration configuration, ILaunch launch, final SubMonitor monitor, final Integer runNo)
            throws CoreException {
        // check for cancellation
        if (monitor.isCanceled())
            return;

        monitor.subTask("run #"+runNo+" - Initializing...");
        monitor.setWorkRemaining(100);

        Process process = LaunchPlugin.startSimulationProcess(configuration, " -r "+runNo);
        IProcess iprocess = DebugPlugin.newProcess(launch, process, renderProcessLabel(runNo));
        iprocess.setAttribute(IProcess.ATTR_CMDLINE, StringUtils.join(LaunchPlugin.createCommandLine(configuration, " -r "+runNo)," "));
        // if we don't want to wait for finishing the process, just exit
        if (!waitForFinish)
            return;

        // setup a stream monitor on the process output, so we can track the progress
        iprocess.getStreamsProxy().getOutputStreamMonitor().addListener(new IStreamListener () {
            int prevPct = 0;
            public void streamAppended(String text, IStreamMonitor ismon) {
                    int pct = LaunchPlugin.getProgressInPercent(text);
                    if (pct >= 0) {
                        monitor.worked(pct - prevPct);
                        prevPct = pct;
                    }

                if (LaunchPlugin.isWaitingForUserInput(text))
                    monitor.subTask("run #"+runNo+" - Waiting for user input... (Switch to console)");
                else
                    monitor.subTask("run #"+runNo+" - Executing ("+prevPct+"%)");
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
