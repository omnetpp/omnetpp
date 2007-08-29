package org.omnetpp.test.gui.util;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;

public class WorkbenchUtils
{
	public static void ensureProjectFileDeleted(String projectName, String fileName) throws CoreException {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
		if (file.exists())
			file.delete(true, null);
	}

	public static void choosePerspectiveFromDialog(String perspectiveLabel) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("Window|Open Perspective|Other.*");
		ShellAccess dialog = Access.findShellByTitle("Open Perspective");
		dialog.findTable().findTableItemByContent(perspectiveLabel).reveal().doubleClick();
	}

	public static ViewPartAccess chooseViewFromDialog(String viewCategory, String viewLabel) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.chooseFromMainMenu("Window|Show View|Other.*");
		TreeAccess tree = Access.findShellByTitle("Show View").findTree();
		tree.findTreeItemByContent(viewCategory).reveal().click();
		tree.pressKey(SWT.ARROW_RIGHT); // open category node
		tree.findTreeItemByContent(viewLabel).reveal().doubleClick();
		//return workbenchWindow.findViewPartByTitle(viewLabel, true);
		return (ViewPartAccess) workbenchWindow.getActivePart();
	}

	public static TreeItemAccess findInProjectExplorerView(String path) {
		//ViewPartAccess projectExplorerView = workbenchWindow.findViewPartByTitle("Project Explorer", true);
		ViewPartAccess projectExplorerView = chooseViewFromDialog("General", "Project Explorer");
		TreeAccess tree = projectExplorerView.findTree();
		tree.assertHasFocus();

		// Note that findTreeItemByContent() does deep search, so this code won't work
		// if any segment is not fully unique in the tree!

		// open project and folder tree nodes
		for (String pathSegment : new Path(path).removeLastSegments(1).segments()) {
			tree.findTreeItemByContent(pathSegment).reveal().click();
			tree.pressKey(SWT.ARROW_RIGHT);
		}
		return tree.findTreeItemByContent(new Path(path).lastSegment()).reveal();
	}
}
