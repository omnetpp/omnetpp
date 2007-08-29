package org.omnetpp.test.gui.access;

import java.io.ByteArrayInputStream;

import junit.framework.Assert;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;

public class WorkspaceAccess {

	@InUIThread
	public static IProject assertProjectExists(String name) {
		IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		Assert.assertTrue("project does not exist", project.exists());
		return project;
	}

	@InUIThread
	public static IProject assertProjectNotExists(String name) {
		IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(name);
		Assert.assertTrue("project still exists", !project.exists());
		return project;
	}

	@InUIThread
	public static IFolder assertFolderExists(String path) {
		IFolder folder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(path));
		Assert.assertTrue("folder does not exist", folder.exists());
		return folder;
	}

	@InUIThread
	public static IFolder assertFolderNotExists(String path) {
		IFolder folder = ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(path));
		Assert.assertTrue("folder still exists", !folder.exists());
		return folder;
	}

	@InUIThread
	public static IFile assertFileExists(String path) {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
		Assert.assertTrue("file does not exist", file.exists());
		return file;
	}

	@InUIThread
	public static IFile assertFileNotExists(String path) {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
		Assert.assertTrue("file still exists", !file.exists());
		return file;
	}

	@InUIThread
	public static void assertFileExistsWithContent(String path, String content) throws Exception {
		IFile file = assertFileExists(path);
		String actualContent = FileUtils.readTextFile(file.getContents());
		Assert.assertTrue("file content differs from expected", actualContent.equals(content));
	}
	
	@NotInUIThread
	public static void createFileWithContent(String path, String content) throws Exception {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
		if (file.exists()) file.delete(true, null);
		file.create(new ByteArrayInputStream(content.getBytes()), true, null);
		WorkspaceAccess.assertFileExistsWithContent(path, content); // wait until background job finishes
	}

}
