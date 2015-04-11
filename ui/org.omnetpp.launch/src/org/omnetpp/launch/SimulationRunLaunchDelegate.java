/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;


import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.LaunchConfigurationDelegate;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.simulation.AbstractSimulationProcess;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;

/**
 * Can launch a single or multiple simulation process. Understands OMNeT++ specific launch attributes.
 * see IOmnetppLaunchConstants.
 *
 * @author rhornig, andras
 */
public class SimulationRunLaunchDelegate extends LaunchConfigurationDelegate {
    public static final String PREF_SWITCH_TO_SIMULATE_PERSPECTIVE = "org.omnetpp.launch.SwitchToSimulatePerspective";  //TODO add a way to clear this preference!

    public static class JobSimulationProcess extends AbstractSimulationProcess {
        private Job job;
        private IJobChangeListener jobChangeListener;

        public JobSimulationProcess(Job job) {
            this.job = job;
            Job.getJobManager().addJobChangeListener(jobChangeListener = new JobChangeAdapter() {
                @Override
                public void done(IJobChangeEvent event) {
                    if (event.getJob() == JobSimulationProcess.this.job) {
                        Job.getJobManager().removeJobChangeListener(jobChangeListener);
                        fireSimulationProcessTerminated();
                    }
                }
            });
        }

        public boolean canCancel() {
            return !isTerminated();
        }

        public void cancel() {
            job.cancel();
        }

        @Override
        public boolean isSuspended() {
            return false;
        }

        @Override
        public boolean isTerminated() {
            return job.getState() == Job.NONE;
        }
    };

    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {

        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode);
        OmnetppLaunchUtils.replaceConfigurationInLaunch(launch, configuration);

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

        final int portNumber = configuration.getAttribute(IOmnetppLaunchConstants.OPP_HTTP_PORT, -1);
        int numProcesses = configuration.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
        boolean reportProgress = StringUtils.contains(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, ""), "-u Cmdenv");

        // start a single or batched launch job
        Job job;
        if (runs.length == 1)
            job = new SimulationLauncherJob(configuration, launch, runs[0], reportProgress, portNumber);
        else
            job = new BatchedSimulationLauncherJob(configuration, launch, runs, numProcesses);

        job.schedule();
    }

    @Override
    protected IProject[] getProjectsForProblemSearch(ILaunchConfiguration configuration, String mode) throws CoreException {
        // NOTE: we need to do this twice: here and in launch() which is kind of superfluous
        //       but it is unclear whether those two incoming configurations are the same or not
        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode);
        String projectName = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");

        if (StringUtils.isEmpty(projectName))
            return ProjectUtils.getOpenProjects();
        else {
            IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
            return ProjectUtils.getAllReferencedProjects(project, false, true);
        }
    }
}
