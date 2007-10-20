package com.simulcraft.test.gui.core;


import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.WorkbenchWindowAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class TestFrameworkTest extends GUITestCase
{
	public void testFileNotExists_1() {
		WorkspaceUtils.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}

	public void testFileExists_SHOULDFAIL_1() {
		WorkspaceUtils.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}

	public void testMainMenu_1() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
		workbenchWindow.chooseFromMainMenu("Window|Show View|Problems");
		Access.sleep(2);
		workbenchWindow.chooseFromMainMenu("Window|Show View|Tasks");
		Access.sleep(2);
	}

	public void testMainMenu_SHOULDFAIL_2() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
		workbenchWindow.chooseFromMainMenu("Window|Blabla-menu-item"); // should fail
	}

	public void testMainMenu_SHOULDFAIL_3() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
		workbenchWindow.chooseFromMainMenu("File|Revert"); // should fail because it's **disabled**
	}

	public void testIsActiveShell_SHOULDFAIL_1() {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindow();
		workbenchWindow.chooseFromMainMenu("File|Open File.*"); // bring up "File Open" dialog
		workbenchWindow.getShell().assertIsActive(); // now this should fail because dialog is open
	}
}
