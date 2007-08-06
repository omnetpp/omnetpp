package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;

public class WorkspaceAccess {
	
	public static IProject assertProjectExists(String name) {
		IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		Assert.assertTrue(project.exists());
		return project;
	}

	public static IProject assertProjectNotExists(String name) {
		IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		Assert.assertTrue(!project.exists());
		return project;
	}

	public static IFolder assertFolderExists(String path) {
		IFolder folder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(path));
		Assert.assertTrue(folder.exists());
		return folder;
	}

	public static IFolder assertFolderNotExists(String path) {
		IFolder folder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(path));
		Assert.assertTrue(!folder.exists());
		return folder;
	}

	public static IFile assertFileExists(String path) {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
		Assert.assertTrue(file.exists());
		return file;
	}

	public static IFile assertFileNotExists(String path) {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
		Assert.assertTrue(!file.exists());
		return file;
	}
}
