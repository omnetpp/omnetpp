/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.launch;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;

/**
 * A job that can execute several simulations in serial or concurrent manner
 *
 * @author rhornig
 */
public class BatchedSimulationLauncherJob extends Job implements IJobChangeListener {
    protected ILaunchConfiguration configuration;
    protected ILaunch launch;
    protected int runs[];
    protected int runIndex = 0;
    protected int maxParalelJobs;
    protected int finishedJobs = 0;
    protected IProgressMonitor groupMonitor = Job.getJobManager().createProgressGroup();

    protected Job finishJob = new Job ("Finishing batch") {
        @Override
        protected IStatus run(IProgressMonitor pm) {
            SubMonitor monitor = SubMonitor.convert(pm, 100);
            monitor.subTask("Refreshing workspace");
            try {
                // refresh the workspace so all generated files will be displayed
                ResourcesPlugin.getWorkspace().getRoot().refreshLocal(IResource.DEPTH_INFINITE, monitor);
            }
            catch (CoreException e) {}
            return Status.OK_STATUS;
        }

        @Override
        public boolean belongsTo(Object family) {
            return family == launch || SimulationLauncherJob.SIMULATION_JOB_FAMILY.equals(family);
        }
    };

    /**
     * @param configuration The configuration to be started
     * @param launch The launch object
     * @param runs An integer array specifying which runs to execute
     * @param parallelism How many simulations should be able to run concurrently
     */
    public BatchedSimulationLauncherJob(ILaunchConfiguration configuration, ILaunch launch, int[] runs, int parallelism) {
        super("Running "+runs.length+" simulations in batch mode.");
        this.configuration = configuration;
        this.launch = launch;
        this.runs = runs;
        this.maxParalelJobs = parallelism;
        setSystem(true);
    }

    @Override
    protected IStatus run(IProgressMonitor mon) {
        groupMonitor.beginTask("Simulating "+launch.getLaunchConfiguration().getName(), runs.length);
        runIndex = finishedJobs = 0;

        try {
            while (finishedJobs < runs.length && canSchedule()) {
                scheduleJobs();
                Job.getJobManager().join(launch, null);
            }

            // refresh the workspace finally to show the result files
            finishJob.setProgressGroup(groupMonitor, 0);
            finishJob.schedule();
            finishJob.join();
        } catch (OperationCanceledException e) {
            return Status.CANCEL_STATUS;
        } catch (InterruptedException e) {
            return Status.CANCEL_STATUS;
        } finally {
            groupMonitor.done();
        }
        return Status.OK_STATUS;
    }

    /**
     * Schedules as many jobs for execution as possible
     */
    protected synchronized void scheduleJobs() {
        while (canSchedule()) {
            Job job = new SimulationLauncherJob(configuration, launch, runs[runIndex++], true, -1);
            job.setProgressGroup(groupMonitor, 1); // assign it to the group monitor so it will be displayed under a single progress view item
            job.addJobChangeListener(this);
            job.setPriority(Job.LONG);
            job.schedule();
        }
    }

    /**
     * Returns whether we can schedule this job (the number scheduled and
     * unfinished jobs are < maxParalellJobs)
     */
    protected synchronized boolean canSchedule() {
        return runIndex - finishedJobs < maxParalelJobs && runIndex < runs.length;
    }

    // job state change listeners to keep track of the number of finished jobs
    public synchronized void done(IJobChangeEvent event) {
        // if the a job is done, we no longer listen to its state changes
        event.getJob().removeJobChangeListener(this);

        if (event.getResult() == Status.CANCEL_STATUS)
            finishedJobs = runs.length;  // signal that we have finished. skip the rest of the jobs and jump after the last run
        else {
            finishedJobs++;
            scheduleJobs(); // schedule some more jobs (if there is any left)
        }
    }

    public void aboutToRun(IJobChangeEvent event) {
    }

    public void running(IJobChangeEvent event) {
    }

    public void awake(IJobChangeEvent event) {
    }

    public void scheduled(IJobChangeEvent event) {
    }

    public void sleeping(IJobChangeEvent event) {
    }
}