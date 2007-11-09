package org.omnetpp.scave.model2;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;

public class ComputedResultFileLocator {
	
	private static final ComputedResultFileLocator instance = new ComputedResultFileLocator();
	
	protected ComputedResultFileLocator() { } 
	
	public static ComputedResultFileLocator instance() {
		return instance;
	}
	
	public String getFileNameFor(Dataset dataset, DatasetItem target) {
		URI uri = dataset.eResource().getURI();
		if (uri != null) {
			ResourceSet resourceSet = dataset.eResource().getResourceSet(); 
			if (resourceSet != null)
				uri = resourceSet.getURIConverter().normalize(uri);
			if (uri.isFile()) {
				IPath path = new Path(uri.toFileString());
				String filePath = path.removeLastSegments(1).append("TODO.vec").toOSString();
				return filePath;
			}
			else if (uri.isPlatform()) {
				IPath path = new Path(uri.toPlatformString(true));
				IPath workspace = ResourcesPlugin.getWorkspace().getRoot().getLocation();
				String filePath = workspace.append(path.removeLastSegments(1)).append("TODO.vec").toOSString();
				return filePath;
			}
		}
		return null;
	}
	
	public String getFileNameFor(ResultItem resultItem) {
		Assert.isLegal(resultItem.getComputed());
		 // XXX should locate its computation node
		String filename = resultItem.getFileRun().getFile().getFileSystemFilePath();
		filename = new Path(filename).removeLastSegments(1).append("TODO.vec").toOSString();
		return filename;
	}
}
