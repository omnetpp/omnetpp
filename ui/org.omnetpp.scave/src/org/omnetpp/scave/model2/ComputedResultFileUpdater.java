package org.omnetpp.scave.model2;

import java.io.File;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.jobs.GenerateComputedFileJob;
import org.omnetpp.scave.model.ProcessingOp;

/**
 * Updates a computed file asynchronously.
 *
 * @author tomi
 */
public class ComputedResultFileUpdater {
	
	public static interface CompletionCallback
	{
		void completed(CompletionEvent event);
	}
	
	public static class CompletionEvent
	{
		ProcessingOp operation;
		IStatus status;
		
		public CompletionEvent(IStatus status, ProcessingOp operation) {
			this.status = status;
			this.operation = operation;
		}

		public ProcessingOp getOperation() {
			return operation;
		}

		public IStatus getStatus() {
			return status;
		}
	}
	
	
	private static ComputedResultFileUpdater instance = new ComputedResultFileUpdater();
	
	private GenerateComputedFileJob scheduledJob;
	private GenerateComputedFileJob waitingJob;
	private IJobChangeListener jobListener = new JobChangeAdapter() {
		public void done(IJobChangeEvent event) {
			handleJobDone(event);
		}
	};
	
	public static ComputedResultFileUpdater instance() {
		return instance;
	}
	
	public synchronized boolean ensureComputedFile(ProcessingOp operation, ResultFileManager manager, CompletionCallback callback) {
		String fileName = operation.getComputedFile(); 
		if (!operation.isComputedFileUpToDate() || fileName == null || !new File(fileName).exists()) {
			updateComputedFile(operation, manager, callback);
			return false;
		}
		else {
			if (callback != null)
				callback.completed(new CompletionEvent(Status.OK_STATUS, operation));
			return true;
		}
	}
	
	public synchronized void updateComputedFile(ProcessingOp operation, ResultFileManager manager, CompletionCallback callback) {
		String fileName = ComputedResultFileLocator.instance().getComputedFile(operation);
		if (fileName == null) {
			if (callback != null) {
				IStatus error = ScavePlugin.getErrorStatus(0, "Cannot locate directory for computed file.", null);
				callback.completed(new CompletionEvent(error, operation));
			}
			return;
		}
		
		GenerateComputedFileJob job = new GenerateComputedFileJob(operation, manager, callback);
		if (scheduledJob == null) {
			job.schedule();
			job.addJobChangeListener(jobListener);
			scheduledJob = job;
		}
		else {
			scheduledJob.cancel();
			waitingJob = job;
		}
	}
	
	private synchronized void handleJobDone(IJobChangeEvent jobEvent) {
		Assert.isTrue(jobEvent.getJob() == scheduledJob);
		Assert.isNotNull(jobEvent.getResult());
		
		if (scheduledJob.getCallback() != null) {
			CompletionEvent event = new CompletionEvent(
											jobEvent.getResult(),
											scheduledJob.getOperation());
			scheduledJob.getCallback().completed(event);
		}
		
		scheduledJob = null;
		if (waitingJob != null) {
			scheduledJob = waitingJob;
			waitingJob = null;
			scheduledJob.addJobChangeListener(jobListener);
			scheduledJob.schedule();
		}
	}
}
