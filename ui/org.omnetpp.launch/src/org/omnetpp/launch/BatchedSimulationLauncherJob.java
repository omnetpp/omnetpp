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
 * A job that can execute several simulation batches in a serial or concurrent manner
 *
 * @author rhornig
 */
public class BatchedSimulationLauncherJob extends Job implements IJobChangeListener {
    protected ILaunchConfiguration configuration;
    protected ILaunch launch;
    protected String[] batchRunFilters;  // one for each batch
    protected int batchIndex = 0;
    protected int maxParallelJobs;
    protected boolean stopOnFirstError;
    protected int finishedJobs = 0;
    protected boolean errorOccurred = false; // one simulation run had an error
    protected boolean cancelled = false; // some simulation job cancelled
    protected IProgressMonitor monitor;
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
     * Create a batched simulation launcher job.
     * @param configuration The configuration to be started
     * @param launch The launch object
     * @param batchRunFilters Run filters for the individual batches
     * @param maxParallelJobs How many batches should able to run concurrently
     */
    public BatchedSimulationLauncherJob(ILaunchConfiguration configuration, ILaunch launch, String[] batchRunFilters, int maxParallelJobs, boolean stopOnFirstError) {
        super("Running " + batchRunFilters.length + " simulation batches.");
        this.configuration = configuration;
        this.launch = launch;
        this.batchRunFilters = batchRunFilters;
        this.maxParallelJobs = maxParallelJobs;
        this.stopOnFirstError = stopOnFirstError;
        setSystem(false);
    }

    @Override
    protected IStatus run(IProgressMonitor mon) {
        groupMonitor.beginTask("Simulating " + launch.getLaunchConfiguration().getName(), batchRunFilters.length);
        batchIndex = finishedJobs = 0;
        this.monitor = mon;
        try {
            while (finishedJobs < batchRunFilters.length && canSchedule() && !mon.isCanceled() && (!stopOnFirstError || !errorOccurred)) {
                scheduleJobs();
                Job.getJobManager().join(launch, null);
            }

            //TODO We should report summary statistics! Number of successful / cancelled / erroneous runs, etc.
            
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
        while (canSchedule() && !monitor.isCanceled() && !cancelled && (!stopOnFirstError || !errorOccurred)) {
            Job job = new SimulationLauncherJob(configuration, launch, batchRunFilters[batchIndex++], true, -1);
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
        return batchIndex - finishedJobs < maxParallelJobs && batchIndex < batchRunFilters.length;
    }

    // job state change listeners to keep track of the number of finished jobs
    public synchronized void done(IJobChangeEvent event) {
        // if the a job is done, we no longer listen to its state changes
        event.getJob().removeJobChangeListener(this);

        if (event.getResult() == Status.CANCEL_STATUS)
            cancelled = true;
        else {
            if (event.getResult().getSeverity() == IStatus.ERROR)
                errorOccurred = true;
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