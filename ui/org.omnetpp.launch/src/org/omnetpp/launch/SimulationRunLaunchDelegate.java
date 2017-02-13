/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;


import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

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
import org.omnetpp.common.Debug;
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

        @Override
        public boolean canCancel() {
            return !isTerminated();
        }

        @Override
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

    @Override
    public void launch(ILaunchConfiguration configuration, String mode, ILaunch launch, IProgressMonitor monitor) throws CoreException {

        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode, true);
        OmnetppLaunchUtils.replaceConfigurationInLaunch(launch, configuration);

        monitor.beginTask("Launching Simulation", 1);

        final int portNumber = configuration.getAttribute(IOmnetppLaunchConstants.OPP_HTTP_PORT, -1);
        boolean reportProgress = StringUtils.contains(configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROGRAM_ARGUMENTS, ""), "-u Cmdenv");

        String runFilter = configuration.getAttribute(IOmnetppLaunchConstants.OPP_RUNFILTER, "");

        boolean useBatching = configuration.getAttribute(IOmnetppLaunchConstants.OPP_USE_BATCHING, false);

        if (!useBatching) {
            Job job = new SimulationLauncherJob(configuration, launch, runFilter, reportProgress, portNumber);
            job.schedule();
        }
        else {
            List<Integer> runNumbers;
            try {
                runNumbers = OmnetppLaunchUtils.queryRunNumbers(configuration, runFilter);
            } catch (InterruptedException e1) {
                return; // abandon job
            }

            int numConcurrentProcesses = configuration.getAttribute(IOmnetppLaunchConstants.OPP_NUM_CONCURRENT_PROCESSES, 1);
            int batchSize = configuration.getAttribute(IOmnetppLaunchConstants.OPP_BATCH_SIZE, 1);
            boolean stopOnError = configuration.getAttribute(IOmnetppLaunchConstants.OPP_STOP_BATCH_ON_ERROR, "false").equals("true"); //XXX similar ini setting won't take effect here

            List<List<Integer>> batches = splitIntoBatches(runNumbers, batchSize);
            Debug.println(batches.toString());

            String[] batchRunFilters = batches.stream().map(batch -> StringUtils.join(batch, ",")).collect(Collectors.toList()).toArray(new String[]{});
            Job job = new BatchedSimulationLauncherJob(configuration, launch, batchRunFilters, numConcurrentProcesses, stopOnError);
            job.schedule();

            /*
             * TODO Something like this could work instead of BatchedSimulationLauncherJob, only progress reporting
             * is broken in Eclipse 4.6, see https://bugs.eclipse.org/bugs/show_bug.cgi?id=505959
             *
            Job launcherJob = new Job("x") {
                //TODO obey stopOnError option!
                @Override
                protected IStatus run(IProgressMonitor monitor) {
                    JobGroup jobGroup = new JobGroup("Simulation batch", numConcurrentProcesses, batchRunFilters.length) {
                        @Override
                        protected boolean shouldCancel(IStatus lastCompletedJobResult, int numberOfFailedJobs, int numberOfCanceledJobs) {
                            return numberOfCanceledJobs > 0 || numberOfFailedJobs > 0;
                        }
                    };
                    for (String batchRunFilter : batchRunFilters) {
                        Job job1 = new SimulationLauncherJob(configuration, launch, batchRunFilter, reportProgress, portNumber);
                        job1.setJobGroup(jobGroup);
                        job1.setSystem(false);
                        job1.schedule();
                    }
                    try {
                        jobGroup.join(0, monitor);
                    } catch (OperationCanceledException | InterruptedException e) {
                        Debug.println("Cancelling group");
                        jobGroup.cancel();
                        return Status.CANCEL_STATUS;
                    }
                    return Status.OK_STATUS;
                }
            };
            launcherJob.setSystem(false);
            launcherJob.schedule();
            */
        }
    }

    protected List<List<Integer>> splitIntoBatches(List<Integer> runNumbers, int batchSize) {
        // note: this algorithm prefers creating equal-size batches to strictly sticking to batchSize, e.g. creates 3+3 instead of 4+2
        List<List<Integer>> result = new ArrayList<>();
        int numBatches = (runNumbers.size() + batchSize - 1) / batchSize;
        int cursor = 0; // for drawing from runNumbers[]
        for (int i = 0; i < numBatches; i++) {
            List<Integer> batch = new ArrayList<>();
            int runsInBatch = runNumbers.size() / numBatches;
            if (i < runNumbers.size() % numBatches)
                runsInBatch++;
            for (int j = 0; j < runsInBatch; j++)
                batch.add(runNumbers.get(cursor++));
            result.add(batch);
        }
        Assert.isTrue(cursor == runNumbers.size());
        return result;
    }

    @Override
    protected IProject[] getProjectsForProblemSearch(ILaunchConfiguration configuration, String mode) throws CoreException {
        // NOTE: we need to do this twice: here and in launch() which is kind of superfluous
        //       but it is unclear whether those two incoming configurations are the same or not
        configuration = OmnetppLaunchUtils.createUpdatedLaunchConfig(configuration, mode, true);
        String projectName = configuration.getAttribute(IOmnetppLaunchConstants.ATTR_PROJECT_NAME, "");

        if (StringUtils.isEmpty(projectName))
            return ProjectUtils.getOpenProjects();
        else {
            IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
            return ProjectUtils.getAllReferencedProjects(project, false, true);
        }
    }
}
