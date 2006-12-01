package org.omnetpp.scave2.jobs;

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
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.content.IContentDescription;
import org.eclipse.core.runtime.content.IContentType;
import org.omnetpp.scave.engine.VectorFileIndexer;
import org.omnetpp.scave2.ContentTypes;

/**
 * This job generates index files for vector files in the workspace.
 * It is sheduled on workspace changes.
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
	
	/**
	 * Turns on automatic indexing.
	 */
	public static void activate() {
		if (instance == null) {
			instance = new VectorFileIndexerJob("Indexing vector files");
			instance.initialize();
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
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(instance.listener);
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
						if (file.exists())
							indexer.generateIndex(file.getAbsolutePath());
					}
					catch (Exception e) {
						e.printStackTrace(); // TODO: retry?
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
									filesToBeIndexed.offer(file.getLocation().toFile());
								break;
							case IResourceDelta.REMOVED:
								if (hasContentType(file, ContentTypes.INDEX))
									filesToBeIndexed.offer(getVectorFile(file).getLocation().toFile());
							}
						}
						return true;
					}
					
				});
			}
		} catch (CoreException e) { // not reached
			e.printStackTrace();
		}
	}
	
	private void collectFilesToBeIndexed() {
		try {
			ResourcesPlugin.getWorkspace().getRoot().accept(new IResourceVisitor() {
				public boolean visit(IResource resource) {
					if (resource instanceof IFile) {
						IFile file = (IFile)resource;
						if (toBeIndexed(file))
								filesToBeIndexed.offer(file.getLocation().toFile());
						return false;
					}
					else
						return true;
				}
			});
		} catch (CoreException e) {
			e.printStackTrace();
		}
	}
	
	private boolean toBeIndexed(IFile file) {
		if (hasContentType(file, ContentTypes.VECTOR)) {
			File osFile = file.getLocation().toFile();
			File osIndexFile = getIndexFile(file).getLocation().toFile(); // might not be added to the workspace, use java.io.File methods
			return !osIndexFile.exists() || osFile.lastModified() > osIndexFile.lastModified();
		}
		else
			return false;
	}
	
	private static boolean hasContentType(IFile file, String contentType) {
		try
		{
			IContentDescription description = file.getContentDescription();
			if (description != null) {
				return contentType.equals(description.getContentType().getId());
			}
		} catch (CoreException e) {
			e.printStackTrace();
		}
		return false;
	}
	
	private static IFile getIndexFile(IFile vectorFile) {
		Assert.isLegal(hasContentType(vectorFile, ContentTypes.VECTOR));
		IPath indexFilePath = vectorFile.getFullPath().removeFileExtension().addFileExtension("vci");
		IFile indexFile = ResourcesPlugin.getWorkspace().getRoot().getFile(indexFilePath);
		return  indexFile;
	}

	private static IFile getVectorFile(IFile indexFile) {
		Assert.isLegal(hasContentType(indexFile, ContentTypes.INDEX));
		IPath vectorFilePath = indexFile.getFullPath().removeFileExtension().addFileExtension("vec");
		IFile vectorFile = ResourcesPlugin.getWorkspace().getRoot().getFile(vectorFilePath);
		return  vectorFile;
	}
}
