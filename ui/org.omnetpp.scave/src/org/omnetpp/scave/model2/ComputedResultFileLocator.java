package org.omnetpp.scave.model2;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourceAttributes;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.omnetpp.scave.engineext.IndexFile;
import org.omnetpp.scave.model.ProcessingOp;

/**
 * Implementation of the computed file naming policy.
 *
 * @author tomi
 */
public class ComputedResultFileLocator {
	
	private static final ComputedResultFileLocator instance = new ComputedResultFileLocator();
	
	protected ComputedResultFileLocator() {	} 
	
	public static ComputedResultFileLocator instance() {
		return instance;
	}
	
	public String getComputedFile(ProcessingOp operation) {
		if (operation.getComputedFile() != null)
			return operation.getComputedFile();
		
		URI uri = getResourceURI(operation);
		if (uri != null) {
			IPath dirPath = getComputedFileDirectory(uri);
			if (dirPath != null) {
				try {
					File file = File.createTempFile("computed", ".vec", dirPath.toFile());
					operation.setComputedFile(file.getAbsolutePath());
					operation.setComputedFileUpToDate(false);
					file.deleteOnExit();
					File indexFile = IndexFile.getIndexFileFor(file);
					indexFile.deleteOnExit();
					return file.getAbsolutePath();
				} catch (IOException e) {}
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
	
	private IPath getComputedFileDirectory(URI uri) {
		if (uri.isFile()) {
			try {
				IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
				IFile[] files = root.findFilesForLocationURI(new java.net.URI(uri.toString()));
				if (files.length > 0)
					return getComputedFileDirectory(files[0]);
			}
			catch (URISyntaxException e) {
			}
			
			// Hmm, file not found in the workspace
			// save the computed file in the same directory as the analysis file.
			IPath filePath = new Path(uri.toFileString());
			return filePath.removeLastSegments(1);
		}
		else if (uri.isPlatform()) {
			IPath workspacePath = new Path(uri.toPlatformString(true));
			IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
			IFile file = root.getFile(workspacePath);
			return getComputedFileDirectory(file);
		}
		else
			return null;
	}
	
	private IPath getComputedFileDirectory(IFile file) {
		IFolder folder = file.getProject().getFolder(".computed");
		if (!folder.exists()) {
			try {
				folder.create(IResource.DERIVED, true, null);
				ResourceAttributes attributes = new ResourceAttributes();
				attributes.setHidden(true);
				folder.setResourceAttributes(attributes);
			}
			catch (CoreException e) {
			}
		}
		
		IPath dirPath = null;
		if (checkFolder(folder))
			dirPath = folder.getLocation();
		else {
			IPath path = file.getLocation().removeLastSegments(1);
			if (checkFolder(path))
				dirPath = path;
		}
		return dirPath;
	}
	
	private boolean checkFolder(IFolder folder) {
		return folder.exists() && checkFolder(folder.getLocation());
	}
	
	private boolean checkFolder(IPath folder) {
		File file = folder.toFile();
		return file.exists() && file.canWrite();
	}
}
