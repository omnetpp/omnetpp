package org.omnetpp.test.gui.scave;

import org.omnetpp.test.gui.access.InputsPageAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;

import com.simulcraft.test.gui.access.TreeAccess;
import com.simulcraft.test.gui.core.GUITestCase;

public class InputsPageTest extends GUITestCase {

	public void testInputFilesTree() {
		ScaveEditorAccess editor = ScaveEditorUtils.openAnalysisFile("test-project", "test-1.scave");
		InputsPageAccess inputsPage = editor.ensureInputsPageActive();
		TreeAccess filesView = inputsPage.getFilesViewTree();
		
	}
	
}
