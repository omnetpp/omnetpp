package org.omnetpp.ned.resources.builder;

import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.ned.resources.NEDResources;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Parses NED files in the background.
 *
 * Configured in plugin.xml to run if project has org.omnetpp.nednature nature set in .project.
 * (Wizard has to set it on new OMNeT++ projects)
 * 
 * XXX What if a NED file gets deleted? do we handle that correctly?
 */
public class NEDBuilder extends IncrementalProjectBuilder {

	public static final String BUILDER_ID = "org.omnetpp.ned.builder.nedbuilder";

	class NEDDeltaVisitor implements IResourceDeltaVisitor {
		/*
		 * (non-Javadoc)
		 * 
		 * @see org.eclipse.core.resources.IResourceDeltaVisitor#visit(org.eclipse.core.resources.IResourceDelta)
		 */
		public boolean visit(IResourceDelta delta) throws CoreException {
			IResource resource = delta.getResource();
			switch (delta.getKind()) {
			case IResourceDelta.ADDED:
				// handle added resource
				handleResourceChange(resource);
				break;
			case IResourceDelta.REMOVED:
				// handle removed resource
				handleResourceDeletion(resource);
				break;
			case IResourceDelta.CHANGED:
				// handle changed resource
				handleResourceChange(resource);
				break;
			}
			//return true to continue visiting children.
			return true;
		}
	}

	class NEDResourceVisitor implements IResourceVisitor {
		public boolean visit(IResource resource) {
			handleResourceChange(resource);
			//return true to continue visiting children.
			return true;
		}
	}


	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.internal.events.InternalBuilder#build(int,
	 *      java.util.Map, org.eclipse.core.runtime.IProgressMonitor)
	 */
	protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
//		if (kind == FULL_BUILD) {
//			fullBuild(monitor);
//		} else {
//			IResourceDelta delta = getDelta(getProject());
//			if (delta == null) {
//				fullBuild(monitor);
//			} else {
//				incrementalBuild(delta, monitor);
//			}
//		}
//		
//		// rebuild tables
//		NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
//		nedResources.rehashIfNeeded();
		return null;
	}

	protected void handleResourceChange(IResource resource) {
		NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
		if (nedResources.isNEDFile(resource)) {
			nedResources.readNEDFile((IFile) resource);
		}
	}

	protected void handleResourceDeletion(IResource resource) {
		NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
		if (nedResources.isNEDFile(resource)) {
			nedResources.forgetNEDFile((IFile) resource);
		}
	}
	
	protected void fullBuild(final IProgressMonitor monitor) throws CoreException {
		try {
			getProject().accept(new NEDResourceVisitor());
		} catch (CoreException e) {
		}
	}

	protected void incrementalBuild(IResourceDelta delta, IProgressMonitor monitor) throws CoreException {
		// the visitor does the work.
		delta.accept(new NEDDeltaVisitor());
	}

	/**
	 * The sole purpose of this method is to be called on startup
	 * from NEDResourcesPlugin.start(). To be removed on the long term.  
	 */
	public static void runFullBuild() {
//		try {
//			IResource wsroot = ResourcesPlugin.getWorkspace().getRoot();
//			wsroot.accept(new IResourceVisitor() {
//				public boolean visit(IResource resource) {
//					NEDResources nedResources = NEDResourcesPlugin.getNEDResources();
//					if (nedResources.isNEDFile(resource))
//						nedResources.readNEDFile((IFile) resource);
//					return true;
//				}
//			});
//		} catch (CoreException e) {
//		}
//        NEDResourcesPlugin.getNEDResources().rehashIfNeeded();
	}
}
