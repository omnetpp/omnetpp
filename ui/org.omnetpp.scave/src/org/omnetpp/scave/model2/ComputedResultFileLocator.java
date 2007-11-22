package org.omnetpp.scave.model2;

import java.net.URISyntaxException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourceAttributes;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.omnetpp.scave.model.ProcessingOp;

public class ComputedResultFileLocator {
	
	private static final ComputedResultFileLocator instance = new ComputedResultFileLocator();
	
	protected ComputedResultFileLocator() {	} 
	
	public static ComputedResultFileLocator instance() {
		return instance;
	}
	
	public String getFileNameFor(ProcessingOp operation) {
		URI uri = getResourceURI(operation);
		if (uri != null) {
			String fileName = String.format("%08x.vec", operation.hashCode());
			if (uri.isFile()) {
				return getFileNameFor(uri, fileName).toOSString();
			}
			else if (uri.isPlatform()) {
				IPath path = new Path(uri.toPlatformString(true));
				return getFileNameFor(path, fileName).toOSString();
			}
		}
		return null;
	}
	
	private URI getResourceURI(ProcessingOp operation) {
		URI uri = operation.eResource().getURI();
		if (uri != null) {
			ResourceSet resourceSet = operation.eResource().getResourceSet(); 
			if (resourceSet != null)
				uri = resourceSet.getURIConverter().normalize(uri);
		}
		return uri;
	}
	
	private IPath getFileNameFor(URI uri, String fileName) {
		try {
			IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
			IFile[] files = root.findFilesForLocationURI(new java.net.URI(uri.toString()));
	
			if (files.length > 0) {
				IFile file = files[0];
				IFolder folder = ensureComputedFolder(file.getProject());
				if (folder != null) {
					return folder.getLocation().append(fileName);
				}
			}
		}
		catch (URISyntaxException e) {
		}
		
		IPath filePath = new Path(uri.toFileString());
		return filePath.removeLastSegments(1).append(fileName);
	}
	
	private IPath getFileNameFor(IPath workspacePath, String fileName) {
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IFile file = root.getFile(workspacePath);

		IFolder folder = ensureComputedFolder(file.getProject());
		if (folder != null) {
			return folder.getLocation().append(fileName);
		}
		
		return file.getLocation().removeLastSegments(1).append(fileName);
	}
	
	private IFolder ensureComputedFolder(IProject project) {
		IFolder folder = project.getFolder(".computed");
		if (!folder.exists()) {
			try {
				folder.create(IResource.DERIVED, true, null);
				ResourceAttributes attributes = new ResourceAttributes();
				attributes.setHidden(true);
				folder.setResourceAttributes(attributes);
			}
			catch (CoreException e) {
				return null;
			}
		}
		return folder;
	}
}
