package org.omnetpp.scave.engineext;

import static org.omnetpp.scave.common.ScaveMarkers.MARKERTYPE_SCAVEPROBLEM;
import static org.omnetpp.scave.common.ScaveMarkers.addMarker;

import java.io.File;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.scave.builder.Activator;
import org.omnetpp.scave.engine.VectorFileIndexer;

/**
 * Augments org.omnetpp.scave.engine.IndexFile with methods
 * that accepts parameters of java types.
 *
 * @author tomi
 */
public class IndexFile extends org.omnetpp.scave.engine.IndexFile {
	
	/**
	 * Returns true, if <code>file</code> is an index file.
	 * The file need not exist.
	 */
	public static boolean isIndexFile(IFile file) {
		return isIndexFile(file.getLocation().toOSString());
	}
	
	/**
	 * Returns true, if <code>file</code> is an vector file.
	 * The file need not exist.
	 */
	public static boolean isVectorFile(IFile file) {
		return isVectorFile(file.getLocation().toOSString());
	}
	
	/**
	 * Returns true, if the index file of <code>file</code> exists
	 * and up-to-date. 
	 * @param file a vector or index file
	 * @return true if the index file is up-to-date
	 */
	public static boolean isIndexFileUpToDate(IFile file) {
		return isIndexFileUpToDate(file.getLocation().toOSString());
	}
	
	/**
	 * Returns true, if the index file of <code>file</code> exists
	 * and up-to-date. 
	 * @param file a vector or index file
	 * @return true if the index file is up-to-date
	 */
	public static boolean isIndexFileUpToDate(String file) {
		return org.omnetpp.scave.engine.IndexFile.isIndexFileUpToDate(file);
	}
	
	/**
	 * Returns the index file belongs to the specified vector file.
	 * 
	 * @param vectorFile the vector file
	 * @return the index file
	 */
	public static IFile getIndexFileFor(IFile vectorFile) {
		Assert.isLegal(isVectorFile(vectorFile));
		IContainer container = vectorFile.getParent();
		String path = getIndexFileName(vectorFile.getLocation().toOSString());
		return getWorkspaceFileForOsPath(container, path); 
	}
	
	/**
	 * Returns the index file belongs to the specified vector file.
	 * 
	 * @param vectorFile the vector file
	 * @return the index file
	 */
	public static File getIndexFileFor(File vectorFile) {
		Assert.isLegal(isVectorFile(vectorFile.getAbsolutePath()));
		return new File(getIndexFileName(vectorFile.getAbsolutePath()));
	}
	
	/**
	 * Returns the vector file belongs to the specified index file.
	 * 
	 * @param indexFile the index file
	 * @return the vector file
	 */
	public static IFile getVectorFileFor(IFile indexFile) {
		Assert.isLegal(isIndexFile(indexFile));
		IContainer container = indexFile.getParent();
		String path = getVectorFileName(indexFile.getLocation().toOSString());
		return getWorkspaceFileForOsPath(container, path);
	}

	/**
	 * Returns the workspace file for the specified absolute path.
	 * The file need not exists, but the path must be under the
	 * given parent container, otherwise null is returned.
	 * 
	 * @param container the the target container of the resource specified by the path 
	 * @param osPath    the absolute OS path
	 * @return the workspace file or null
	 */
	private static IFile getWorkspaceFileForOsPath(IContainer parent, String osPath) {
		IPath location = new Path(osPath);
		IPath parentLocation = parent.getLocation();
		if (parentLocation.isPrefixOf(location)) {
			IPath relativePath = location.removeFirstSegments(parentLocation.segmentCount());
			return parent.getFile(relativePath); 
		}
		else
			return null; // XXX assert?
	}

	/**
	 * Perform indexing the given vector file, and add error/warning
	 * markers to the file if there's any problem. 
	 */
	public static void performIndexing(IFile vectorFile, IProgressMonitor monitor) {
		try {
			vectorFile.deleteMarkers(MARKERTYPE_SCAVEPROBLEM, true, IResource.DEPTH_ZERO);

			VectorFileIndexer indexer = new VectorFileIndexer();
			String osFileName = vectorFile.getLocation().toFile().getAbsolutePath();

			System.out.println("started indexing " + vectorFile);
			long startTime = System.currentTimeMillis();
			indexer.generateIndex(osFileName, monitor);
			System.out.println("finished indexing " + vectorFile + ", " + (System.currentTimeMillis()-startTime) + "ms");
		}
		catch (ResultFileFormatException e) {
			addMarker(vectorFile, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Wrong file: "+e.getMessage(), e.getLineNo());
			Activator.logError("Vector file format error: " + e.getMessage(), e);
		}
		catch (Throwable e) {
			addMarker(vectorFile, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_WARNING, "Indexing failed: "+e.getMessage(), -1);
			Activator.logError("Cannot create index file for: "+vectorFile.toString(), e);
		}
	}
}
