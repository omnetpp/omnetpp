package org.omnetpp.test.gui.inifileeditor;

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
import org.omnetpp.test.gui.access.WorkspaceAccess;

public class IniFileEditorTestUtils {

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

	public static void createNewIniFileByWizard1(String projectName, String fileName, String networkName) {
		// Select the project in the "Project Explorer" view, and choose "New|Other..." from its context menu
		findInProjectExplorerView(projectName).chooseFromContextMenu("New|Other.*");

		// Open the "OMNEST/OMNeT++" category in the "New" dialog, and double-click "Ini file" in it
		ShellAccess shell = Access.findShellByTitle("New");
		TreeAccess tree = shell.findTree();
		TreeItemAccess treeItem = tree.findTreeItemByContent(".*OMNEST.*");
		treeItem.click();
		shell.pressKey(SWT.ARROW_RIGHT); // open tree node
		tree.findTreeItemByContent("Ini.*").doubleClick();

		fillNewInifileWizard(null, fileName, networkName);

		// make sure the file really exists
		WorkspaceAccess.assertFileExists(projectName+"/"+fileName);
	}

	public static void createNewIniFileByWizard2(String parentFolder, String fileName, String networkName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		choosePerspectiveFromDialog(".*OMN.*"); // so that we have "New|Inifile" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Ini.*");
		fillNewInifileWizard(parentFolder, fileName, networkName); // fill in wizard
		WorkspaceAccess.assertFileExists(parentFolder+"/"+fileName); // make sure file got created
	}

	public static void fillNewInifileWizard(String parentFolder, String fileName, String networkName) {
		// fill in the fields in the dialog, then click "Finish"
		ShellAccess shell = Access.findShellByTitle("New Ini File");
		//Access.dumpWidgetHierarchy(shell.getShell());
		if (parentFolder != null)
			shell.findTextAfterLabel(".*parent folder.*").clickAndType(parentFolder);
		if (fileName != null)
			shell.findTextAfterLabel("File name.*").clickAndType(fileName);
		if (networkName != null)
			shell.findComboAfterLabel("NED Network:").clickAndType(networkName);
		shell.findButtonWithLabel("Finish").activateWithMouseClick();
	}
}
