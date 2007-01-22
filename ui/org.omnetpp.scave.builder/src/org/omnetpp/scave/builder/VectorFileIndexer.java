package org.omnetpp.scave.builder;

import static org.omnetpp.scave.engineext.IndexFile.getVectorFile;
import static org.omnetpp.scave.engineext.IndexFile.isIndexFile;
import static org.omnetpp.scave.engineext.IndexFile.isIndexFileUpToDate;
import static org.omnetpp.scave.engineext.IndexFile.isVectorFile;

import java.io.File;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;

public class VectorFileIndexer extends IncrementalProjectBuilder {

	public static final String BUILDER_ID = "org.omnetpp.scave.builder.vectorfileindexer";
	
	private org.omnetpp.scave.engine.VectorFileIndexer indexer = 
				new org.omnetpp.scave.engine.VectorFileIndexer();
	
	private Queue<IFile> filesToBeIndexed = new ConcurrentLinkedQueue<IFile>();
	
	@Override
	protected IProject[] build(int kind, Map args, IProgressMonitor monitor)
			throws CoreException {
	    if (kind == IncrementalProjectBuilder.FULL_BUILD) {
	         fullBuild(monitor);
	      } else {
	         IResourceDelta delta = getDelta(getProject());
	         if (delta == null) {
	            fullBuild(monitor);
	         } else {
	            incrementalBuild(delta, monitor);
	         }
	      }
	      return null;
	}
	
	protected void fullBuild(IProgressMonitor monitor) throws CoreException {
		// collect files to be indexed
		getProject().accept(new IResourceVisitor() {
			public boolean visit(IResource resource) throws CoreException {
				if (resource instanceof IFile) {
					IFile file = (IFile)resource;
					if (toBeIndexed(file))
						filesToBeIndexed.offer(file);
					return false;
				}
				else
					return true;
			}
		});
		// generate index files
		doBuild(monitor);
	}

	protected void incrementalBuild(IResourceDelta delta, IProgressMonitor monitor)
		throws CoreException {
		// add files to be indexed to the queue
		delta.accept(new IResourceDeltaVisitor() {
			public boolean visit(IResourceDelta delta) {
				if (delta.getResource() instanceof IFile) {
					IFile file = (IFile)delta.getResource();
					switch (delta.getKind()) {
					case IResourceDelta.ADDED:
					case IResourceDelta.CHANGED:
						if (toBeIndexed(file))
							filesToBeIndexed.offer(file);
						break;
					case IResourceDelta.REMOVED:
						if (isIndexFile(file))
							filesToBeIndexed.offer(getVectorFile(file));
					}
				}
				return true;
			}
		});
		// generate index files
		doBuild(monitor);
	}
	
	protected void doBuild(IProgressMonitor monitor) {
		IFile file;
		try {
			monitor.beginTask("Indexing vector files", filesToBeIndexed.size());

			while ((file=filesToBeIndexed.peek()) != null) {
				if (monitor.isCanceled())
					throw new OperationCanceledException();
				if (isInterrupted())
					break;
				
				file = filesToBeIndexed.poll();
				monitor.subTask("Indexing "+file.getName());
				try {
					File path = file.getLocation().toFile();
					if (path.exists())
						indexer.generateIndex(path.getAbsolutePath());
				}
				catch (Exception e) {
					Activator.logError(e); // TODO: retry?
				}
				monitor.worked(1);
			}
		}
		finally {
			monitor.done();
		}
	}
	
	protected boolean toBeIndexed(IFile file) {
		if (isVectorFile(file)) {
			return !isIndexFileUpToDate(file);
		}
		else
			return false;
	}
	
	protected void generateIndex(IFile vectorFile) {
		indexer.generateIndex(vectorFile.getLocation().toFile().getAbsolutePath());
	}
}
