package org.omnetpp.common.eventlog;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Display;

/**
 * Manages potentially long running event log operations.
 */
public class EventLogProgressManager {
	private ProgressMonitorDialog progressDialog;

	private IProgressMonitor progressMonitor;
	
	public IProgressMonitor getProgressMonitor() {
		return progressMonitor;
	}

	public boolean isCanceled() {
        return progressMonitor.isCanceled();
	}

	public boolean isInRunWithProgressMonitor() {
		return progressDialog != null;
	}

	public void runWithProgressMonitor(final Runnable runnable) throws InvocationTargetException, InterruptedException {
		try {
			progressDialog = new ProgressMonitorDialog(Display.getCurrent().getActiveShell());
			progressDialog.setOpenOnRun(false);
			progressDialog.run(false, true, new IRunnableWithProgress() {
				public void run(IProgressMonitor progressMonitor)
						throws InvocationTargetException, InterruptedException
				{
					try {
						EventLogProgressManager.this.progressMonitor = progressMonitor;
						progressMonitor.beginTask("A long running event log operation is in progress. Please wait or press Cancel to abort.", IProgressMonitor.UNKNOWN);
						runnable.run();
						progressMonitor.done();
					}
					finally {
						EventLogProgressManager.this.progressMonitor = null;
					}
				}
			});
		}
		finally {
			progressDialog = null;
		}
	}

	public void showProgressDialog() {
		if (!progressDialog.getShell().isVisible())
			progressDialog.open();
	}
}
