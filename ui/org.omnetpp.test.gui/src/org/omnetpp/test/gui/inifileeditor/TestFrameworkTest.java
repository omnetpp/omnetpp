package org.omnetpp.test.gui.inifileeditor;

import org.omnetpp.test.gui.access.WorkspaceAccess;

public class TestFrameworkTest {
	
	public void testFileNotExists_1() {
		WorkspaceAccess.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}

	public void testFileExists_SHOULDFAIL_1() {
		WorkspaceAccess.assertFileNotExists("/no-such-project/some-nonsense-filename");
	}
}
