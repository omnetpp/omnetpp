package org.omnetpp.test.gui.inifileeditor;

import junit.framework.Assert;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.GUITestCase;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.EditorPartAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.ViewPartAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;

public class IniFileEditorTest2 extends GUITestCase {
	protected String projectName = "test-project";
	protected String fileName = "omnetpp.ini";

	protected void prepareForTest() throws CoreException {
		// Test setup: close all editors, delete the inifile left over from previous runs 
		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		workbenchAccess.closeAllEditorPartsWithHotKey();
		IniFileEditorTestUtils.ensureProjectFileDeleted(projectName, fileName);
	}
	
	public void testCreateIniFile() throws Throwable {
		prepareForTest();
		IniFileEditorTestUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");
		
		//Access.getWorkbenchWindowAccess().findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");
		//Access.sleep(5);
		
		//workbenchAccess.chooseFromMainMenu("Window|Show View|Problems");
		//Access.sleep(2);
		//workbenchAccess.chooseFromMainMenu("Window|Show View|Tasks");
		//Access.sleep(2);
	}

	public void testWizardResult() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		prepareForTest();
		IniFileEditorTestUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");

		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		
		// Find the inifile editor, and switch to its text page
		EditorPartAccess editorAccess = workbenchAccess.findEditorByTitle(fileName);
		editorAccess.activatePageInMultiPageEditorByLabel("Text");
		//Access.dumpWidgetHierarchy(editorAccess.getRootControl());

		// Find the text editor in it, and verify it has the right content
		StyledTextAccess textAccess = editorAccess.findTextEditor();
		String editorContent = textAccess.getText();
		System.out.println("Editor contents: >>>" + editorContent + "<<<");
		Assert.assertTrue(editorContent.equals("[General]\npreload-ned-files = *.ned\nnetwork = \n"));
	}
	
	public void testWrongNetwork() throws Throwable {
		//WorkbenchAccess.startTracingEvents();
		prepareForTest();
		IniFileEditorTestUtils.createNewIniFileByWizard2(projectName, fileName, "some-network");

		WorkbenchWindowAccess workbenchAccess = Access.getWorkbenchWindowAccess();
		
		// Find the inifile editor and switch it to text mode
		workbenchAccess.findEditorByTitle(fileName).activatePageInMultiPageEditorByLabel("Text");

		// Wizard has created the file with an empty "network=" line; type "Undefined" there as network name
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.ARROW_DOWN);
		workbenchAccess.pressKey(SWT.END);
		workbenchAccess.typeKeySequence(" Undefined");
		workbenchAccess.saveCurrentEditorPartWithHotKey();

		// The "Problems" view must display a "No such NED network" error
		ViewPartAccess problemsViewAccess = workbenchAccess.findViewPartByPartName("Problems", true);
		problemsViewAccess.activateWithMouseClick();
		problemsViewAccess.findTree().findTreeItemByContent(".*No such NED network.*");
	}
	
}
