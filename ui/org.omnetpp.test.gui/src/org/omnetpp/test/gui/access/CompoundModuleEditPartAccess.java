package org.omnetpp.test.gui.access;

public class CompoundModuleEditPartAccess extends EditPartAccess
{
	public void createSubModuleWithPalette(String type, String name, int x, int y) {
		getGraphicalNedEditorAccess().getFlyoutPaletteAccess().clickButtonWithLabel(type);
		//click(Access.LEFT_MOUSE_BUTTON, x, y);
	}

	public void createConnectionWithPalette(String string, String string2) {
	}

	public GraphicalNedEditorAccess getGraphicalNedEditorAccess() {
		return null;
	}
}
