package org.omnetpp.launch;


import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;


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
		if (StringUtils.isEmpty(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, ""))) {
			ILaunchConfigurationWorkingCopy cfg = configuration.copy("opp_run temporary configuration");
			String oppRunName = OmnetppMainPlugin.getOmnetppBinDir()+"/opp_run"; 
			if(Platform.getOS().equals(Platform.OS_WIN32))
				oppRunName += ".exe";
				
			cfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_NAME, oppRunName);
			// use the first librarie's project as main project
			cfg.setAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, cfg.getAttribute(IOmnetppLaunchConstants.ATTR_SHARED_LIB_PROJECT_NAME, ""));
			configuration = cfg;
		}

    	if (monitor == null) {
            monitor = new NullProgressMonitor();
        }
        monitor.beginTask("Launching Simulation", 1);

        Integer runs[] = LaunchPlugin.parseRuns(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_RUNNUMBER, ""),
                                                LaunchPlugin.getMaxNumberOfRuns(configuration));
        if (runs == null)
            throw new CoreException(new Status(IStatus.ERROR, LaunchPlugin.PLUGIN_ID, "Invalid run numbers specified"));

        // show the debug view if option is checked
        if (configuration.getAttribute(IOmnetppLaunchConstants.ATTR_SHOWDEBUGVIEW, false)) {
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

        int numProcesses = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_NUM_CONCURRENT_PROCESSES, 1);
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
