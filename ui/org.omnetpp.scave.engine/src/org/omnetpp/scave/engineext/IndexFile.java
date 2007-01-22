package org.omnetpp.scave.engineext;

import java.io.File;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

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
	 * @param file the vector file
	 * @return the index file
	 */
	public static IFile getIndexFile(IFile file) {
		Assert.isLegal(isVectorFile(file));
		String path = getIndexFileName(file.getLocation().toOSString());
		return getWorkspaceFileForOsPath(path); 
	}
	
	/**
	 * Returns the index file belongs to the specified vector file.
	 * 
	 * @param file the vector file
	 * @return the index file
	 */
	public static File getIndexFile(File file) {
		Assert.isLegal(isVectorFile(file));
		return new File(getIndexFileName(file.getAbsolutePath()));
	}
	
	/**
	 * Returns the vector file belongs to the specified index file.
	 * 
	 * @param file the index file
	 * @return the vector file
	 */
	public static IFile getVectorFile(IFile file) {
		Assert.isLegal(isIndexFile(file));
		String path = getVectorFileName(file.getLocation().toOSString());
		return getWorkspaceFileForOsPath(path);
	}

	/**
	 * Returns the vector file belongs to the specified index file.
	 * 
	 * @param file the index file
	 * @return the vector file
	 */
	public static File getVectorFile(File file) {
		Assert.isLegal(isIndexFile(file));
		return new File(getVectorFileName(file.getAbsolutePath()));
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
}
