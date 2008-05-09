package org.omnetpp.test.gui.scave;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkbenchUtils;
import com.simulcraft.test.gui.util.WorkspaceUtils;


public class OpenFileTest
    extends ScaveFileTestCase
{
	public void testNewScaveFile() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
		WorkbenchUtils.ensurePerspectiveActivated(".*Simul.*"); // so that we have "New|Inifile" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Analysis.*");
		fillNewScaveFileWizard(projectName, fileName); // fill in wizard
		WorkspaceUtils.assertFileExists(projectName + "/" + fileName); // make sure file got created
	}
	
	private static void fillNewScaveFileWizard(String parentFolder, String fileName) {
		// fill in the fields in the dialog, then click "Finish"
		ShellAccess shell = Access.findShellWithTitle("New Analysis.*");
		//Access.dumpWidgetHierarchy(shell.getShell());
		if (parentFolder != null)
			shell.findTextAfterLabel(".*parent folder.*").clickAndTypeOver(parentFolder);
		if (fileName != null)
			shell.findTextAfterLabel("File name.*").clickAndTypeOver(fileName);
		shell.findButtonWithLabel("Finish").selectWithMouseClick();
	}
}
