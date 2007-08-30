package org.omnetpp.test.gui.access;

import org.omnetpp.ned.editor.graph.GraphicalNedEditor;


public class GraphicalNedEditorAccess
	extends EditorPartAccess
{
	public GraphicalNedEditorAccess(GraphicalNedEditor editorPart) {
		super(editorPart);
	}
	
	public GraphicalNedEditor getGraphicalNedEditor() {
		return (GraphicalNedEditor)workbenchPart;
	}
	
	public FlyoutPaletteAccess getFlyoutPaletteAccess() {
		return null;
	}
	
	public SimpleModuleEditPartAccess createSimpleModuleWithPalette(String name) {
		return null;
	}

	public CompoundModuleEditPartAccess createCompoundModuleWithPalette(String name) {
		return null;
	}
}
