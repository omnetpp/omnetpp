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
	
	public void testMainMenu() {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.chooseFromMainMenu("Window|Show View|Problems");
		Access.sleep(2);
		workbenchAccess.chooseFromMainMenu("Window|Show View|Tasks");
		Access.sleep(2);
	}

}
