package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;


public class GraphicalNedEditorAccess
	extends EditorPartAccess
{
	public GraphicalNedEditorAccess(GraphicalNedEditor editorPart) {
		super(editorPart);
	}
	
	public GraphicalNedEditor getGraphicalNedEditor() {
		return (GraphicalNedEditor)workbenchPart;
	}
	
	public FlyoutPaletteCompositeAccess getFlyoutPaletteComposite() {
		return new FlyoutPaletteCompositeAccess((FlyoutPaletteComposite)ReflectionUtils.getFieldValue(getGraphicalNedEditor(), "splitter"));
	}

	@NotInUIThread
	public SimpleModuleEditPartAccess createSimpleModuleWithPalette(String name) {
		clickPaletteItem(".*Simple.*module.*");
		clickBackground();
		renameModule("Unnamed.*", name);

		// TODO:
		return null;
	}

	@InUIThread
	private void renameModule(final String oldName, String newName) {
		FigureCanvas figureCanvas = getGraphicalNedEditor().getFigureCanvas();
		IFigure rootFigure = figureCanvas.getLightweightSystem().getRootFigure();
		IFigure labelFigure = findDescendantFigure(rootFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(oldName);
			}
		});

		new FigureAccess(labelFigure).click(LEFT_MOUSE_BUTTON, figureCanvas);
		ControlAccess canvasAccess = new ControlAccess(figureCanvas);
		canvasAccess.pressKey(SWT.F6);
		canvasAccess.typeIn(newName);
		canvasAccess.pressKey(SWT.KEYPAD_CR);
	}

	@InUIThread
	public void clickBackground() {
		getComposite().click(LEFT_MOUSE_BUTTON, getCompositeInternal().toDisplay(1, 1));
	}

	@InUIThread
	public void clickPaletteItem(String label) {
		getFlyoutPaletteComposite().clickButtonWithLabel(label);
	}

	public CompoundModuleEditPartAccess createCompoundModuleWithPalette(String name) {
		return null;
	}
}
