package org.omnetpp.launch;


import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;


/**
 * Can launch a single or multiple simulation process. Understands OMNeT++ specific launch attributes. 
 * see IOmnetppLaunchConstants.
 *
 * @author rhornig
 */
public class SimulationRunConfigurationDelegate extends LaunchConfigurationDelegate {

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor)
            throws CoreException {
		// check if program name is not provided in this case we should use opp_run as the executable
    	configuration = OmnetppLaunchUtils.convertLaunchConfig(configuration, mode);

    	if (monitor == null) {
            monitor = new NullProgressMonitor();
        }
        monitor.beginTask("Launching Simulation", 1);

        int runs[] = OmnetppLaunchUtils.parseRuns(configuration.getAttribute(IOmnetppLaunchConstants.OPP_RUNNUMBER, ""),
                                                OmnetppLaunchUtils.getMaxNumberOfRuns(configuration));
        Assert.isTrue(runs != null && runs.length > 0);

        // show the debug view if option is checked
        if (configuration.getAttribute(IOmnetppLaunchConstants.OPP_SHOWDEBUGVIEW, false)) {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                    try {
                        workbenchPage.showView(IDebugUIConstants.ID_DEBUG_VIEW, null, IWorkbenchPage.VIEW_VISIBLE);
                    } catch (PartInitException e) {
                        LaunchPlugin.logError("Cannot initialize the Debug View", e);
                    }
                }
            });
        }

        int numProcesses = configuration.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        boolean reportProgress = StringUtils.contains(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, ""), "-u Cmdenv");
        // start a single or batched launch job
        Job job;
        if (runs.length == 1 )
            job = new SimulationLauncherJob(configuration, launch, runs[0], reportProgress);
        else
            job = new BatchedSimulationLauncherJob(configuration, launch, runs, numProcesses);

        job.schedule();
        monitor.done();
    }

}
