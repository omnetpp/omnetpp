package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Assert;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;

public class IniFileEditorTestUtils {
	
	public static void ensureProjectFileDeleted(String projectName, String fileName) throws CoreException {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
		if (file.exists())
			file.delete(true, null);
	}

	public static void choosePerspectiveFromDialog(String perspectiveLabel) { 
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.chooseFromMainMenu("Window|Open Perspective|Other.*");
		Access.findShellByTitle("Open Perspective").findTable().findTableItemByContent(perspectiveLabel).doubleClick();
	}

	public static TreeItemAccess findInProjectExplorerView(String resourceName) {
		Assert.assertTrue(!resourceName.contains("/"));
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		TreeAccess projectTreeAccess = workbenchAccess.findViewPartByPartName("Project Explorer", true).findTree();
		return projectTreeAccess.findTreeItemByContent(resourceName);
	}

	public static void createNewIniFileByWizard1(String projectName, String fileName, String networkName) { 
		// Select the project in the "Project Explorer" view, and choose "New|Other..." from its context menu
		findInProjectExplorerView(projectName).chooseFromContextMenu("New|Other.*");

		// Open the "OMNEST/OMNeT++" category in the "New" dialog, and double-click "Ini file" in it
		ShellAccess shellAccess = Access.findShellByTitle("New");
		TreeAccess treeAccess = shellAccess.findTree();
		TreeItemAccess treeItemAccess = treeAccess.findTreeItemByContent(".*OMNEST.*");
		treeItemAccess.click();
		shellAccess.pressKey(SWT.ARROW_RIGHT); // open tree node
		treeAccess.findTreeItemByContent("Ini.*").doubleClick(); 

		fillNewInifileWizard(null, fileName, networkName);
		
		// make sure the file really exists
		WorkspaceAccess.assertFileExists(projectName+"/"+fileName);
	}

	public static void createNewIniFileByWizard2(String parentFolder, String fileName, String networkName) { 
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		choosePerspectiveFromDialog(".*OMN.*"); // so that we have "New|Inifile" in the menu
		workbenchAccess.chooseFromMainMenu("File|New.*|Ini.*");
		fillNewInifileWizard(parentFolder, fileName, networkName); // fill in wizard
		WorkspaceAccess.assertFileExists(parentFolder+"/"+fileName); // make sure file got created
	}

	public static void fillNewInifileWizard(String parentFolder, String fileName, String networkName) {
		// Enter the file name in the dialog, and click "Finish"
		ShellAccess shellAccess = Access.findShellByTitle("New Ini File");
		Access.dumpWidgetHierarchy(shellAccess.getShell());
		if (parentFolder != null)
			shellAccess.findTextAfterLabel(".*parent folder.*").clickAndType(parentFolder);
		if (fileName != null)
			shellAccess.findTextAfterLabel("File name.*").clickAndType(fileName);
		if (networkName != null)
			shellAccess.findTextAfterLabel("NED Network:").clickAndType(networkName);
		shellAccess.findButtonWithLabel("Finish").activateWithMouseClick();
	}
}
