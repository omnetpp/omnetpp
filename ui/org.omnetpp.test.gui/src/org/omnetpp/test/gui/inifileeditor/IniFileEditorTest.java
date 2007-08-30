package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Assert;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.TextEditorAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.core.GUITestCase;
import org.omnetpp.test.gui.util.WorkbenchUtils;
import org.omnetpp.test.gui.util.WorkspaceUtils;

public class IniFileEditorTest extends GUITestCase
{
	protected String projectName = "test-project";

	protected String fileName = "omnetpp.ini";

	protected void prepareForTest() throws CoreException {
		// Test setup: close all editors, delete the file left over from previous runs
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.assertIsActiveShell();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}

	public void testCreateIniFile() throws Throwable {
		prepareForTest();
		IniFileEditorUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");

		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.findMultiPageEditorPartByTitle(fileName).activatePage("Text");
//		Access.sleep(2);
//		workbenchWindow.chooseFromMainMenu("Window|Show View|Problems");
//		Access.sleep(2);
//		workbenchWindow.chooseFromMainMenu("Window|Show View|Tasks");
//		Access.sleep(2);
	}

	public void testWizardResult() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		prepareForTest();
		IniFileEditorUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");

		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();

		// Find the inifile editor, and switch to its text page
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		multiPageEditorPart.activatePage("Text");
		//Access.dumpWidgetHierarchy(editor.getRootControl());

		// Find the text editor in it, and verify it has the right content
		StyledTextAccess styledText = multiPageEditorPart.findStyledText();
		String editorContent = styledText.getText();
		System.out.println("Editor contents: >>>" + editorContent + "<<<");
		Assert.assertTrue(editorContent.equals("[General]\npreload-ned-files = *.ned\nnetwork = some-network\n"));
	}

	public void testWrongNetwork() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		prepareForTest();
		IniFileEditorUtils.createNewIniFileByWizard2(projectName, fileName, null);

		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		TextEditorAccess textualEditor = (TextEditorAccess)workbenchWindow.findMultiPageEditorPartByTitle(fileName).activatePage("Text");

		// Wizard has created the file with an empty "network=" line; type "Undefined" there as network name
		textualEditor.findStyledText().moveCursorAfter("network.*=");
		
		StyledTextAccess styledText = workbenchWindow.getActiveEditorPart().findStyledText();
		styledText.typeIn(" Undefined");
		workbenchWindow.getActiveEditorPart().saveWithHotKey();

		WorkbenchUtils.assertErrorMessageInProblemsView(".*No such NED network.*");
	}
}
