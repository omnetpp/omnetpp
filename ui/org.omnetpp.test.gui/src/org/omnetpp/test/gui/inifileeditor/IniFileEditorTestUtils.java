package org.omnetpp.test.gui.inifileeditor;

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

	public static void bringUpNewWizardFor(String projectName) {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		
		// Select the project in the "Project Explorer" view, and choose "New|Other..." from its context menu
		TreeAccess projectTreeAccess = workbenchAccess.findViewPartByPartName("Project Explorer", true).findTree();
		TreeItemAccess treeItem = projectTreeAccess.findTreeItemByContent(projectName);
		treeItem.chooseFromContextMenu("New|Other.*");
	}

	public static void createNewIniFileByWizard(String projectName, String fileName, String networkName) { //FIXME assume parent is already selected in the Navigator ? 
		bringUpNewWizardFor(projectName);

		// Open the "OMNEST/OMNeT++" category in the "New" dialog, and double-click "Ini file" in it
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		ShellAccess shellAccess = Access.findShellByTitle("New");
		TreeAccess treeAccess = shellAccess.findTree();
		TreeItemAccess treeItemAccess = treeAccess.findTreeItemByContent(".*OMNEST.*");
		treeItemAccess.click();
		workbenchAccess.pressKey(SWT.ARROW_RIGHT); // open tree node
		treeAccess.findTreeItemByContent("Ini.*").doubleClick(); 

		// Enter the file name in the dialog, and click "Finish"
		ShellAccess shellAccess2 = Access.findShellByTitle("New Ini File");
		shellAccess2.findTextNearLabel("File name.*").typeIn(fileName);
		shellAccess2.findButtonWithLabel("Finish").activateWithMouseClick();
		
		// make sure the file really exists
		WorkspaceAccess.assertFileExists(projectName+"/"+fileName);
	}

}
