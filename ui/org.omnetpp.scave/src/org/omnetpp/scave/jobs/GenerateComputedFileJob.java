package org.omnetpp.scave.jobs;

import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model2.DataflowNetworkBuilder;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.model2.ComputedResultFileUpdater.CompletionCallback;

/**
 * Job for  updating a computed vector file asynchronously.
 *
 * @author tomi
 */
public class GenerateComputedFileJob extends WorkspaceJob
{
	private static final boolean debug = true;
	
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
		long startTime = 0;
		if (debug)
			startTime = System.currentTimeMillis();
		
		try {
			if (monitor != null)
				monitor.beginTask("Generate computed file", 100);
			
			Dataset dataset = ScaveModelUtil.findEnclosingDataset(operation);
			if (dataset == null)
				return ScavePlugin.getErrorStatus(0, "Operation removed from the dataset.", null);
			
			if (monitor != null)
				monitor.subTask("Build dataflow network");

			DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
			DataflowManager network = builder.build(dataset, operation, operation.getComputedFile());
			if (debug) network.dump();
			
			IProgressMonitor subMonitor = null;
			if (monitor != null) {
				monitor.worked(10);
				monitor.subTask("Execute dataflow network");
				subMonitor = new SubProgressMonitor(monitor, 90);
			}
			
			try {
				network.execute(subMonitor);
			}
			finally {
				network.delete();
				network = null;
			}
			
			if (monitor != null && monitor.isCanceled())
				return Status.CANCEL_STATUS;
			
			operation.setComputationHash(DatasetManager.getComputationHash(operation, manager));
			return Status.OK_STATUS;
		} catch (Exception e) {
			return ScavePlugin.getErrorStatus(e);
		}
		finally {
			if (monitor != null)
				monitor.done();
			if (debug) {
				long endTime = System.currentTimeMillis();
				System.out.format("Generated computed file in %s ms%n", endTime - startTime);
			}
		}
	}
}
