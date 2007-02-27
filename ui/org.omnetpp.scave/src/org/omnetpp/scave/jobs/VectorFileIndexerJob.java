package org.omnetpp.scave.jobs;

import static org.omnetpp.scave.engineext.IndexFile.getVectorFile;
import static org.omnetpp.scave.engineext.IndexFile.isIndexFile;
import static org.omnetpp.scave.engineext.IndexFile.isIndexFileUpToDate;
import static org.omnetpp.scave.engineext.IndexFile.isVectorFile;

import java.io.File;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.VectorFileIndexer;

/**
 * This job generates index files for vector files in the workspace.
 * When active it is scheduled automatically on workspace changes.
 *
 * @author tomi
 */
public class VectorFileIndexerJob extends WorkspaceJob {

	private static VectorFileIndexerJob instance;

	private IResourceChangeListener listener;
	private VectorFileIndexer indexer = new VectorFileIndexer();
	private Queue<File> filesToBeIndexed = new ConcurrentLinkedQueue<File>();

	private VectorFileIndexerJob(String name) {
		super(name);
	}

	public VectorFileIndexerJob(String name, IFile[] filesToBeIndexed) {
		super(name);
		for (IFile file : filesToBeIndexed)
			enqueueFile(file);
	}

	/**
	 * Turns on automatic indexing.
	 */
	public static void activate() {
		if (instance == null) {
			getInstance().initialize();
		}
	}

	/**
	 * Turns off automatic indexing.
	 */
	public static void deactivate() {
		if (instance != null) {
			instance.dispose();
			instance = null;
		}
	}

	private static VectorFileIndexerJob getInstance() {
		if (instance == null) {
			instance = new VectorFileIndexerJob("Indexing vector files");
		}
		return instance;
	}

	private void initialize() {
		collectFilesToBeIndexed();
		schedule();

		listener = new IResourceChangeListener() {
			public void resourceChanged(IResourceChangeEvent event) {
				updateFilesToBeIndexed(event);
				schedule();
			}
		};
		ResourcesPlugin.getWorkspace().addResourceChangeListener(listener,
				IResourceChangeEvent.POST_BUILD | IResourceChangeEvent.POST_CHANGE);
	}

	private void dispose() {
		if (listener != null)
			ResourcesPlugin.getWorkspace().removeResourceChangeListener(listener);
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

				File file;
				while ((file=filesToBeIndexed.poll()) != null) {
					if (monitor.isCanceled())
						return Status.CANCEL_STATUS;

					monitor.subTask("Indexing "+file.getName());
					try {
						if (file.exists() && !isIndexFileUpToDate(file))
							indexer.generateIndex(file.getAbsolutePath()); //TODO add ERROR and WARNING markers if possible
					}
					catch (Exception e) {
						IStatus error =  new Status(Status.ERROR, ScavePlugin.PLUGIN_ID, 0,
											String.format("Indexing of '%s' failed: %s",
													file.getAbsolutePath(), e.getLocalizedMessage()),
											e);
						ScavePlugin.getDefault().getLog().log(error);
						return error;
					}
					monitor.worked(1);
				}
			}
			finally {
				monitor.done();
			}
		}
		return Status.OK_STATUS;
	}

	private void updateFilesToBeIndexed(IResourceChangeEvent event) {
		try {
			int type = event.getType();
			if (type == IResourceChangeEvent.POST_BUILD || type == IResourceChangeEvent.POST_CHANGE) {
				IResourceDelta delta = event.getDelta();
				delta.accept(new IResourceDeltaVisitor() {
					public boolean visit(IResourceDelta delta) {
						if (delta.getResource() instanceof IFile) {
							IFile file = (IFile)delta.getResource();
							switch (delta.getKind()) {
							case IResourceDelta.ADDED:
							case IResourceDelta.CHANGED:
								if (toBeIndexed(file))
									enqueueFile(file);
								break;
							case IResourceDelta.REMOVED:
								if (isIndexFile(file))
									enqueueFile(getVectorFile(file));
							}
						}
						return true;
					}

				});
			}
		} catch (CoreException e) { // not reached
			e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
		}
	}

	private void collectFilesToBeIndexed() {
		try {
			ResourcesPlugin.getWorkspace().getRoot().accept(new IResourceVisitor() {
				public boolean visit(IResource resource) {
					if (resource instanceof IFile) {
						IFile file = (IFile)resource;
						if (toBeIndexed(file))
								enqueueFile(file);
						return false;
					}
					else
						return true;
				}
			});
		} catch (CoreException e) {
			e.printStackTrace();  //FIXME log it or something. Just "print" is not OK!
		}
	}

	private void enqueueFile(IFile file) {
		filesToBeIndexed.offer(file.getLocation().toFile());
	}

	private boolean toBeIndexed(IFile file) {
		if (isVectorFile(file)) {
			return !isIndexFileUpToDate(file);
		}
		else
			return false;
	}
}
