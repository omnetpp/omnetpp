package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Assert;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.test.gui.GUITestCase;
import org.omnetpp.test.gui.access.EditorPartAccess;
import org.omnetpp.test.gui.access.MenuAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.TreeAccess;
import org.omnetpp.test.gui.access.TreeItemAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;

public class IniFileEditorTest2 extends GUITestCase {
	protected String projectName = "test-project";
	protected String fileName = "omnetpp.ini";
	
	protected void ensureProjectFileDeleted(String projectName, String fileName) throws CoreException {
		IFile file = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName).getFile(fileName);
		if (file.exists())
			file.delete(true, null);
	}

	protected void createNewIniFile() throws CoreException {
		// Test setup: close all editors, delete the inifile left over from previous runs 
		workbenchAccess.closeAllEditorPartsWithHotKey();
		ensureProjectFileDeleted(projectName, fileName);

		// Select the project in the "Project Explorer" view
		TreeAccess projectTreeAccess = workbenchAccess.findViewPartByPartName("Project Explorer", true).findTree();
		TreeItemAccess treeItem = projectTreeAccess.findTreeItemByContent(projectName);

		// Select "New|Other..." from context menu
		MenuAccess menuAccess = treeItem.activateContextMenuWithMouseClick();
		MenuAccess submenuAccess = menuAccess.activateMenuItemWithMouse("New");
		submenuAccess.activateMenuItemWithMouse("Other.*");

		// Open the "OMNEST/OMNeT++" category in the "New" dialog, and double-click "Ini file" in it
		ShellAccess shellAccess = workbenchAccess.findShellByTitle("New");
		TreeAccess treeAccess = new TreeAccess((Tree)shellAccess.findDescendantControl(shellAccess.getShell(), Tree.class));
		TreeItemAccess treeItemAccess = treeAccess.findTreeItemByContent(".*OMNEST.*");
		treeItemAccess.click();
		workbenchAccess.pressKey(SWT.ARROW_RIGHT); // open tree node
		treeAccess.findTreeItemByContent("Ini.*").doubleClick(); 

		// Enter the file name in the dialog, and click "Finish"
		ShellAccess shellAccess2 = workbenchAccess.findShellByTitle("New Ini File");
		shellAccess2.findTextNearLabel("File name.*").typeIn(fileName);
		shellAccess2.findButtonWithLabel("Finish").activateWithMouseClick();
		
		// make sure the file really exists
		WorkspaceAccess.assertFileExists(projectName+"/"+fileName);
		WorkspaceAccess.assertFileNotExists(projectName+"/"+"some-nonsense-filename");
	}

	public void testCreateIniFile() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		createNewIniFile();
	}

	public void testWizardResult() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		createNewIniFile();
		
		// Find the inifile editor, and switch to its text page
		EditorPartAccess editorAccess = workbenchAccess.findEditorByTitle(fileName);
		editorAccess.activatePageInMultiPageEditorByLabel("Text");
		//Access.dumpWidgetHierarchy(editorAccess.getRootControl());

		// Find the text editor in it, and verify it has the right content
		StyledTextAccess textAccess = new StyledTextAccess((StyledText) editorAccess.findDescendantControl(editorAccess.getRootControl(), StyledText.class));
		String editorContent = textAccess.getText();
		Assert.assertTrue(editorContent.equals("[General]\npreload-ned-files = *.ned\nnetwork = "));
	}
	
	public void testWrongNetwork() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		createNewIniFile();

		// Find the inifile editor and switch it to text mode
		workbenchAccess.findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");

		// Wizard has created the file with an empty "network=" line; type "Undefined" there as network name
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.END);
		workbenchAccess.typeIn(" Undefined");
		workbenchAccess.saveCurrentEditorPartWithHotKey();

		// The "Problems" view must display a "No such NED network" error
		ViewPartAccess problemsViewAccess = workbenchAccess.findViewPartByPartName("Problems", true);
		problemsViewAccess.activateWithMouseClick();
		problemsViewAccess.findTree().findTreeItemByContent(".*No such NED network.*");
	}
	
}
