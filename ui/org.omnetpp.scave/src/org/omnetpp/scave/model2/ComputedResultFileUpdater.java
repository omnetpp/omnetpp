package org.omnetpp.scave.model2;

import java.io.File;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
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
		String fileName;
		IStatus status;
		
		public CompletionEvent(IStatus status, ProcessingOp operation, String fileName) {
			this.status = status;
			this.operation = operation;
			this.fileName = fileName;
		}

		public ProcessingOp getOperation() {
			return operation;
		}

		public String getFileName() {
			return fileName;
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
		if (fileName == null || !new File(fileName).exists()) {
			updateComputedFile(operation, manager, callback);
			return false;
		}
		else {
			return true;
		}
	}
	
	public synchronized void updateComputedFile(ProcessingOp operation, ResultFileManager manager, CompletionCallback callback) {
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
											scheduledJob.getOperation(),
											scheduledJob.getFileName());
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
