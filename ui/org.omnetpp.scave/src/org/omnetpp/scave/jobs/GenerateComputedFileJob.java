package org.omnetpp.scave.jobs;

import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model2.DataflowNetworkBuilder;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.model2.ComputedResultFileUpdater.CompletionCallback;

/**
 * Job for  updating a computed vector file asynchronously.
 *
 * @author tomi
 */
public class GenerateComputedFileJob extends WorkspaceJob
{
	private static final boolean debug = false;
	
	ProcessingOp operation;
	ResultFileManager manager;
	CompletionCallback callback;
	
	
	public GenerateComputedFileJob(ProcessingOp operation, ResultFileManager manager, CompletionCallback callback) {
		super("Generate computed file");
		Assert.isNotNull(operation.getComputedFile());
		this.operation = operation;
		this.manager = manager;
		this.callback = callback;
	}
	
	public ProcessingOp getOperation() {
		return operation;
	}
	
	public CompletionCallback getCallback() {
		return callback;
	}

	@Override
	public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
		try {
			if (monitor != null)
				monitor.beginTask("Generate computed file", IProgressMonitor.UNKNOWN);
			
			Dataset dataset = ScaveModelUtil.findEnclosingDataset(operation);
			if (dataset == null)
				return ScavePlugin.getErrorStatus(0, "Operation removed from the dataset.", null);
			
			if (monitor != null)
				monitor.subTask("Build dataflow network");

			DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
			DataflowManager network = builder.build(dataset, operation, operation.getComputedFile());
			if (debug)
				network.dump();
			
			if (monitor != null)
				monitor.subTask("Execute dataflow network");
			network.execute(monitor);
			
			if (monitor != null && monitor.isCanceled())
				return Status.CANCEL_STATUS;
			
			operation.setComputedFileUpToDate(true);
			return Status.OK_STATUS;
		} catch (Exception e) {
			return ScavePlugin.getErrorStatus(e);
		}
		finally {
			if (monitor != null)
				monitor.done();
		}
	}
}
