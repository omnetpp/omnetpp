package org.omnetpp.test.gui.nededitor;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.test.gui.access.Access;
import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.MultiPageEditorPartAccess;
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
		workbenchWindow.closeAllEditorPartsWithHotKey();
		WorkspaceUtils.ensureProjectFileDeleted(projectName, fileName);
	}

	public void _testCreateNedFile() throws Throwable {
		prepareForTest();
		NedEditorUtils.createNewNedFileByWizard(projectName, fileName);
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess mutliPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		mutliPageEditorPart.activatePage("Text");
	}
	
	public void _testCreateNedModel() throws Throwable {
		prepareForTest();
		NedEditorUtils.createNewNedFileByWizard(projectName, fileName);
		WorkbenchWindowAccess workbenchWindow = Access.getWorkbenchWindowAccess();
		MultiPageEditorPartAccess multiPageEditorPart = workbenchWindow.findMultiPageEditorPartByTitle(fileName);
		GraphicalNedEditorAccess graphicalNedEditor = (GraphicalNedEditorAccess)multiPageEditorPart.activatePage("Graphical");
		graphicalNedEditor.createSimpleModuleWithPalette("TestNode");
		TextEditorAccess textualEditor = (TextEditorAccess)multiPageEditorPart.activatePage("Text");
		textualEditor.moveCursorAfter("simple TestNode.*\\n\\{");
		textualEditor.pressEnter();
		textualEditor.typeIn("gates:");
		textualEditor.pressEnter();
		textualEditor.typeIn("inout g;");
		textualEditor.pressEnter();
		multiPageEditorPart.activatePage("Graphical");
		CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.createCompoundModuleWithPalette("TestNetwork");
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node1", 200, 200);
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node2", 100, 100);
		compoundModuleEditPart.createConnectionWithPalette("node1", "node2", ".*g.*");
		multiPageEditorPart.saveWithHotKey();
	}

	//TODO incomplete code...
	public void testInheritanceErrors() throws Throwable {
		prepareForTest();
		NedEditorUtils.createNewNedFileByWizard(projectName, fileName);

		// plain inheritance
		assertNoErrorInNedSource("simple A {}");
		assertNoErrorInNedSource("simple A {}\nsimple B extends A {}");
		assertErrorInNedSource("simple A extends Unknown {}", ".*no such.*Unknown.*");
		assertErrorInNedSource("simple A like Unknown {}", ".*no such.*Unknown.*");
		assertErrorInNedSource("simple A {}\nsimple B like A {}", ".*A is not an interface.*");

		// cycles
		assertErrorInNedSource("simple A extends A {}", ".*cycle.*");
		assertErrorInNedSource("simple A extends B {}\nsimple B extends A {}", ".*cycle.*");
		assertErrorInNedSource("simple A extends B {}\nsimple B extends C {}\nsimple C extends A {}", ".*cycle.*");
		
		//TODO: same thing with "module" and "channel" instead of "simple"
	}

	private void assertNoErrorInNedSource(String nedSource) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
	}

	private void assertErrorInNedSource(String nedSource, String errorText) {
		NedEditorUtils.typeIntoTextualNedEditor(fileName, nedSource);
		WorkbenchUtils.assertErrorMessageInProblemsView(errorText);
		//TODO: do something in the graphical editor: check error markers are there, etc
	}
}
