package org.omnetpp.scave.jobs;

import static org.omnetpp.scave.engineext.IndexFile.isIndexFileUpToDate;
import static org.omnetpp.scave.engineext.IndexFile.isVectorFile;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.omnetpp.scave.engineext.IndexFile;

/**
 * This job generates index files for vector files in the workspace.
 *
 * @author tomi
 */
public class VectorFileIndexerJob extends WorkspaceJob {
	
	private List<IFile> filesToBeIndexed;
	private Object lock;

	public VectorFileIndexerJob(String name, IFile[] filesToBeIndexed, Object lock) {
		super(name);
		setRule(ResourcesPlugin.getWorkspace().getRuleFactory().buildRule());
		
		this.filesToBeIndexed = new ArrayList<IFile>();
		this.lock = lock;
		for (IFile file : filesToBeIndexed)
			if (toBeIndexed(file))
				this.filesToBeIndexed.add(file);
	}

	/**
	 * Generate indeces for files in the queue.
	 */
	@Override
	public IStatus runInWorkspace(IProgressMonitor monitor)
			throws CoreException {

		if (!filesToBeIndexed.isEmpty()) {
			try {
				monitor.beginTask(getName(), filesToBeIndexed.size());

				for (IFile file : filesToBeIndexed) {
					if (monitor.isCanceled())
						return Status.CANCEL_STATUS;

					monitor.subTask("Indexing "+file.getName());
					synchronized (lock) {
						if (file.exists() && !isIndexFileUpToDate(file)) {
							IProgressMonitor subMonitor = new SubProgressMonitor(monitor, 1);
							IndexFile.performIndexing(file, subMonitor);
							if (subMonitor.isCanceled())
								return Status.CANCEL_STATUS;
						}
					}
				}
			}
			finally {
				monitor.done();
			}
		}
		return Status.OK_STATUS;
	}

	private boolean toBeIndexed(IFile file) {
		if (isVectorFile(file)) {
			synchronized (lock) {
				return !isIndexFileUpToDate(file);
			}
		}
		else
			return false;
	}
}
