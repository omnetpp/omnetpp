package org.omnetpp.scave.engineext;

import java.io.File;
import java.util.HashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.IWorkspaceRunnable;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.scave.builder.Activator;
import org.omnetpp.scave.engineext.ResultFileFormatException;
import org.omnetpp.scave.engine.VectorFileIndexer;

/**
 * Augments org.omnetpp.scave.engine.IndexFile with methods
 * that accepts parameters of java types.
 *
 * @author tomi
 */
public class IndexFile extends org.omnetpp.scave.engine.IndexFile {
	
	private static final String MARKERTYPE_SCAVEPROBLEM = "org.omnetpp.scave.builder.scaveproblem";

	/**
	 * Returns true, if <code>file</code> is an index file.
	 * The file need not exist.
	 */
	public static boolean isIndexFile(IFile file) {
		return isIndexFile(file.getLocation().toOSString());
	}
	
	/**
	 * Returns true, if <code>file</code> is an index file.
	 * The file need not exist.
	 */
	public static boolean isIndexFile(File file) {
		return isIndexFile(file.getAbsolutePath());
	}
	
	/**
	 * Returns true, if <code>file</code> is an vector file.
	 * The file need not exist.
	 */
	public static boolean isVectorFile(IFile file) {
		return isVectorFile(file.getLocation().toOSString());
	}
	
	/**
	 * Returns true, if <code>file</code> is an vector file.
	 * The file need not exist.
	 */
	public static boolean isVectorFile(File file) {
		return isVectorFile(file.getAbsolutePath());
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
	public static boolean isIndexFileUpToDate(File file) {
		return isIndexFileUpToDate(file.getAbsolutePath());
	}
	
	/**
	 * Returns the index file belongs to the specified vector file.
	 * 
	 * @param vectorFile the vector file
	 * @return the index file
	 */
	public static IFile getIndexFileFor(IFile vectorFile) {
		Assert.isLegal(isVectorFile(vectorFile));
		String path = getIndexFileName(vectorFile.getLocation().toOSString());
		return getWorkspaceFileForOsPath(path); 
	}
	
	/**
	 * Returns the index file belongs to the specified vector file.
	 * 
	 * @param vectorFile the vector file
	 * @return the index file
	 */
	public static File getIndexFileFor(File vectorFile) {
		Assert.isLegal(isVectorFile(vectorFile));
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
		String path = getVectorFileName(indexFile.getLocation().toOSString());
		return getWorkspaceFileForOsPath(path);
	}

	/**
	 * Returns the vector file belongs to the specified index file.
	 * 
	 * @param indexFile the index file
	 * @return the vector file
	 */
	public static File getVectorFileFor(File indexFile) {
		Assert.isLegal(isIndexFile(indexFile));
		return new File(getVectorFileName(indexFile.getAbsolutePath()));
	}
	
	/**
	 * Returns the workspace relative full path of the file,
	 * specified by an OS absolute path. The file need not exists,
	 * but if the path is not under the workspace root, then <code>null</code>
	 * is returned.
	 * 
	 * @param path the OS path
	 * @return the full path in the workspace or null
	 */
	private static IPath getWorkspacePathForOsPath(String path) {
		IPath location = new Path(path);
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IPath workspacePath = root.getLocation();
		if (workspacePath.isPrefixOf(location)) {
			return location.removeFirstSegments(workspacePath.segmentCount());
		}
		else
			return null;
	}
	
	/**
	 * Returns the workspace file for the specified absolute path.
	 * The file need not exists, but the path must be under the
	 * workspace, otherwise null is returned.
	 * 
	 * @param path the absolute OS path
	 * @return the workspace file or null
	 */
	private static IFile getWorkspaceFileForOsPath(String path) {
		IPath fullPath = getWorkspacePathForOsPath(path);
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		return fullPath != null ? root.getFile(fullPath) : null; 
	}

	/**
	 * Perform indexing the given vector file, and add error/warning
	 * markers to the file if there's any problem. 
	 */
	public static void performIndexing(IFile vectorFile) {
		try {
			vectorFile.deleteMarkers(MARKERTYPE_SCAVEPROBLEM, true, IResource.DEPTH_ZERO);

			VectorFileIndexer indexer = new VectorFileIndexer();
			String osFileName = vectorFile.getLocation().toFile().getAbsolutePath();

			System.out.println("started indexing " + vectorFile);
			long startTime = System.currentTimeMillis();
			indexer.generateIndex(osFileName);
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
	
	/**
	 * Utility function to add markers to a file.
	 */
	@SuppressWarnings("unchecked")
	private static void addMarker(final IFile file, final String type, int severity, String message, int line) {

        // taken from MarkerUtilities see. Eclipse FAQ 304
        final HashMap map = new HashMap();
        map.put(IMarker.MESSAGE, message);
        map.put(IMarker.SEVERITY, severity);
        if (line > 0)
        	map.put(IMarker.LINE_NUMBER, line);

        IWorkspaceRunnable r = new IWorkspaceRunnable() {
            public void run(IProgressMonitor monitor) throws CoreException {
                IMarker marker = file.createMarker(type);
                marker.setAttributes(map);
            }
        };

        try {
			file.getWorkspace().run(r, null, 0, null);
			System.out.println("marker added: "+type+" on "+file+" line "+line+": "+message);
		} catch (CoreException e) {
			Activator.logError("cannot add marker", e);
		}
	}
}
