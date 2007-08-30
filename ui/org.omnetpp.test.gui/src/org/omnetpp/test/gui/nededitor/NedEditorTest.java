package org.omnetpp.test.gui.nededitor;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
import org.omnetpp.test.gui.access.ShellAccess;
import org.omnetpp.test.gui.access.StyledTextAccess;
import org.omnetpp.test.gui.access.TextEditorAccess;
import org.omnetpp.test.gui.access.WorkbenchWindowAccess;
import org.omnetpp.test.gui.core.GUITestCase;
import org.omnetpp.test.gui.util.WorkbenchUtils;
import org.omnetpp.test.gui.util.WorkspaceUtils;

public class NedEditorTest extends GUITestCase
{
	protected String projectName = "test-project";

	protected String fileName = "test.ned";

	protected void prepareForTest() throws CoreException {
		// Test setup: close all editors, delete the file left over from previous runs
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		workbenchWindow.assertIsActiveShell();
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}

	public void _testCreateNedFile() throws Throwable {
		prepareForTest();
		createNewNEDFileByWizard(projectName, fileName);
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess mutliPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		mutliPageEditorPart.activatePage("Text");
	}
	
	public void testCreateNedModel() throws Throwable {
		prepareForTest();
		createNewNEDFileByWizard(projectName, fileName);
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.activatePage("Graphical");
		graphicalNedEditor.createSimpleModuleWithPalette("TestNode");
		TextEditorAccess textualEditor = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
		textualEditor.moveCursorAfter("simple TestNode.*\\{");
		textualEditor.typeIn("\n");
		textualEditor.typeIn("gates:\n");
		textualEditor.typeIn("inout g;\n");
		CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.createCompoundModuleWithPalette("TestNetwork");
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node1", 100, 100);
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node2", 200, 200);
		compoundModuleEditPart.createConnectionWithPalette("node1", "node2");
	}

	//TODO incomplete code...
	public void _testInheritanceErrors() throws Throwable {
		prepareForTest();
		createNewNEDFileByWizard(projectName, fileName);

		// plain inheritance
		checkErrorInSource("simple A {}", null); //OK
		checkErrorInSource("simple A extends Unknown {}", "no such type: Unknown");
		checkErrorInSource("simple A like Unknown {}", "no such type: Unknown");
		checkErrorInSource("simple A {}\nsimple B extends A {}", null); //OK
		checkErrorInSource("simple A {}\nsimple B like A {}", "A is not an interface");
		// cycles
		checkErrorInSource("simple A extends A {}", "cycle");
		checkErrorInSource("simple A extends B {}\nsimple B extends A {}", "cycle");
		checkErrorInSource("simple A extends B {}\nsimple B extends C {}\nsimple C extends A {}", "cycle");
		//TODO: same thing with "module" and "channel" instead of "simple"
	}
	
	private void createNewNEDFileByWizard(String parentFolder, String fileName) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		WorkbenchUtils.choosePerspectiveFromDialog(".*OMN.*"); // so that we have "New|NED file" in the menu
		workbenchWindow.chooseFromMainMenu("File|New.*|Network Description.*");
		ShellAccess shell = Access.findShellByTitle("New NED File");
		shell.findTextAfterLabel(".*parent folder.*").clickAndType(parentFolder);
		shell.findTextAfterLabel("File name.*").clickAndType(fileName);
		shell.findButtonWithLabel("Finish").activateWithMouseClick();
		WorkspaceUtils.assertFileExists(parentFolder + "/" + fileName); // make sure file got created
	}

	private void checkErrorInSource(String nedSource, String errorText) {
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		TextEditorAccess textualEditor = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
		StyledTextAccess styledText = textualEditor.findStyledText();
		styledText.pressKey('A', SWT.CTRL); // "Select all"
		styledText.typeIn(nedSource);
		WorkbenchUtils.assertErrorMessageInProblemsView(errorText);
		multiPageEditorPart.activatePage("Graphics");
		//TODO: do something in the graphical editor: check error markers are there, etc
	}
}
