package org.omnetpp.test.gui.access;

public class CompoundModuleEditPartAccess extends EditPartAccess
{
	public GraphicalNedEditorAccess getGraphicalNedEditorAccess() {
		return null;
	}

	public void createSubModuleWithPalette(String type, String name, int x, int y) {
		getGraphicalNedEditorAccess().getFlyoutPaletteComposite().clickButtonWithLabel(type);
		//click(Access.LEFT_MOUSE_BUTTON, x, y);
	}

	public void createConnectionWithPalette(String string, String string2) {
	}
}
