package org.omnetpp.test.gui.nededitor;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;
import org.omnetpp.test.gui.core.GUITestCase;
import org.omnetpp.test.gui.util.WorkbenchUtils;

public class NedEditorTest extends GUITestCase
{
	protected String projectName = "test-project";

	protected String fileName = "test.ned";

	protected void prepareForTest() throws CoreException {
		// Test setup: close all editors, delete the file left over from previous runs
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.assertIsActiveShell();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkbenchUtils.ensureProjectFileDeleted(projectName, fileName);
	}

	public void testCreateNedFile() throws Throwable {
		prepareForTest();

		createNewNEDFileByWizard(projectName, fileName);
		
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.findEditorPartByTitle(fileName).activatePage("Text");
	}
	
	private void createNewNEDFileByWizard(String parentFolder, String fileName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		WorkbenchUtils.choosePerspectiveFromDialog(".*OMN.*"); // so that we have "New|NED file" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Network Description.*");
		ShellAccess shell = Access.findShellByTitle("New NED File");
		shell.findTextAfterLabel("File name.*").clickAndType(fileName);
		shell.findButtonWithLabel("Finish").activateWithMouseClick();
		WorkspaceAccess.assertFileExists(parentFolder + "/" + fileName); // make sure file got created
	}
}
