package org.omnetpp.test.gui.nededitor;

import org.omnetpp.test.gui.access.CompoundModuleEditPartAccess;
import org.omnetpp.test.gui.access.GraphicalNedEditorAccess;
import org.omnetpp.test.gui.access.NedEditorAccess;

import com.simulcraft.test.gui.access.TextEditorAccess;
import com.simulcraft.test.gui.util.WorkspaceUtils;

public class CreateComplexModelWithGraphicalEditorTest
	extends NedFileTestCase
{
	public void testCreateSimpleModel() throws Throwable {
		WorkspaceUtils.createFileWithContent(filePath, "");
		NedEditorAccess nedEditor = findNedEditor();
		GraphicalNedEditorAccess graphicalNedEditor = nedEditor.ensureActiveGraphicalEditor();
		graphicalNedEditor.createSimpleModuleWithPalette("TestNode");
		TextEditorAccess textualEditor = nedEditor.activateTextEditor();
		textualEditor.moveCursorAfter("simple TestNode.*\\n\\{");
		textualEditor.pressEnter();
		textualEditor.typeIn("gates:");
		textualEditor.pressEnter();
		textualEditor.typeIn("inout g;");
		textualEditor.pressEnter();
		nedEditor.activateGraphicalEditor();
		CompoundModuleEditPartAccess compoundModuleEditPart = graphicalNedEditor.createCompoundModuleWithPalette("TestNetwork");
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node1", 200, 200);
		compoundModuleEditPart.createSubModuleWithPalette("TestNode", "node2", 100, 100);
		compoundModuleEditPart.createConnectionWithPalette("node1", "node2", ".*g.*");
		nedEditor.saveWithHotKey();
	}
}
