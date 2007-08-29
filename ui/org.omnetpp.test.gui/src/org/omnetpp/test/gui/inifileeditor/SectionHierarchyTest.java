package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.EditorPartAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;
import org.omnetpp.test.gui.core.GUITestCase;

public class SectionHierarchyTest extends GUITestCase {
	protected String projectName = "test-project";
	protected String fileName = "tmp.ini";
	protected String filePath = projectName + "/" + fileName;

	@Override
	protected void setUp() throws Exception {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.assertIsActiveShell();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceAccess.createFileWithContent(filePath, "");
		IniFileEditorTestUtils.findInProjectExplorerView(filePath).reveal().doubleClick();
		workbenchWindow.findEditorPartByTitle(fileName); //TODO .assertClass(InifileEditor.class)
	}
	
	public void testWrongNetwork() throws Throwable {
		WorkbenchWindowAccess workbenchWindowAccess = Access.getWorkbenchWindowAccess();
		EditorPartAccess editor = workbenchWindowAccess.getActiveEditorPart();
		editor.activatePage("Text");
		StyledTextAccess styledText = editor.findStyledText();
		styledText.assertHasFocus();
		styledText.pressKey('A', SWT.CTRL); // select all
		styledText.typeIn("Hello");

		// The "Problems" view must display a "No such NED network" error
		ViewPartAccess problemsViewAccess = workbenchWindowAccess.findViewPartByTitle("Problems", true);
		problemsViewAccess.activateWithMouseClick();
		problemsViewAccess.findTree().findTreeItemByContent("Line must be in the form.*");
	}
	
}
