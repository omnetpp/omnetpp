package org.omnetpp.test.gui.inifileeditor;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.GUITestCase;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.EditorPartAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.access.WorkspaceAccess;

public class SectionHierarchyTest extends GUITestCase {
	protected String projectName = "test-project";
	protected String fileName = "tmp.ini";
	protected String filePath = projectName + "/" + fileName;

	@Override
	protected void setUp() throws Exception {
		WorkbenchWindowAccess workbenchWindowAccess = Access.getWorkbenchWindowAccess();
		workbenchWindowAccess.assertIsActiveShell();
		workbenchWindowAccess.closeAllEditorPartsWithHotKey();
		WorkspaceAccess.createFileWithContent(filePath, "");
		IniFileEditorTestUtils.findInProjectExplorerView(fileName).doubleClick();
		workbenchWindowAccess.findEditorPartByTitle(fileName); //TODO .assertClass(InifileEditor.class)
	}
	
	public void testCreateIniFile() throws Throwable {
		WorkbenchWindowAccess workbenchWindowAccess = Access.getWorkbenchWindowAccess();
		EditorPartAccess editorPart = workbenchWindowAccess.findEditorPartByTitle(fileName);
		editorPart.activatePageInMultiPageEditorByLabel("Text");
		StyledTextAccess textEditor = editorPart.findStyledText();
		textEditor.assertHasFocus();
		//textEditor.ty
//		Access.sleep(2);
//		workbenchAccess.chooseFromMainMenu("Window|Show View|Problems");
//		Access.sleep(2);
//		workbenchAccess.chooseFromMainMenu("Window|Show View|Tasks");
//		Access.sleep(2);
	}

	public void testWrongNetwork() throws Throwable {
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();

		// Find the inifile editor and switch it to text mode
		workbenchAccess.findEditorPartByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");

		// Wizard has created the file with an empty "network=" line; type "Undefined" there as network name
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.END);
		workbenchAccess.typeKeySequence(" Undefined");
		workbenchAccess.getActiveEditorPart().saveWithHotKey();

		// The "Problems" view must display a "No such NED network" error
		ViewPartAccess problemsViewAccess = workbenchAccess.findViewPartByTitle("Problems", true);
		problemsViewAccess.activateWithMouseClick();
		problemsViewAccess.findTree().findTreeItemByContent(".*No such NED network.*");
	}
	
}
