package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;

public class TestFrameworkTest {
	
	public void testFileNotExists_1() {
		WorkspaceAccess.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}

	public void testFileExists_SHOULDFAIL_1() {
		WorkspaceAccess.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}
	
	public void testMainMenu_1() {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.assertIsActiveShell();
		workbenchAccess.chooseFromMainMenu("Window|Show View|Problems");
		Access.sleep(2);
		workbenchAccess.chooseFromMainMenu("Window|Show View|Tasks");
		Access.sleep(2);
	}

	public void testMainMenu_SHOULDFAIL_2() {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.assertIsActiveShell();
		workbenchAccess.chooseFromMainMenu("Window|Blabla-menu-item"); // should fail
	}

	public void testMainMenu_SHOULDFAIL_3() {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.assertIsActiveShell();
		workbenchAccess.chooseFromMainMenu("File|Revert"); // should fail because it's **disabled**
	}

	public void testIsActiveShell_SHOULDFAIL_1() {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.assertIsActiveShell();
		workbenchAccess.chooseFromMainMenu("File|Open File.*"); // bring up "File Open" dialog
		workbenchAccess.assertIsActiveShell();  // now this should fail because dialog is open
	}
	
}
