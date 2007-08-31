package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.NedTypeEditPart;
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
	public NedTypeEditPart createSimpleModuleWithPalette(String name) {
		clickPaletteItem(".*Simple.*module.*");
		clickBackground();
		renameModule("Unnamed.*", name);

		// TODO:
		return null;
	}

	@NotInUIThread
	public void renameModule(String oldName, String newName) {
		clickModuleFigureWithName(oldName);
		typeInNewName(newName);
	}

	@InUIThread
	private void typeInNewName(String name) {
		FigureCanvas figureCanvas = getGraphicalNedEditor().getFigureCanvas();
		Text text = (Text)Access.findDescendantControl(figureCanvas, Text.class);
		TextAccess textAccess = new TextAccess(text);
		textAccess.pressKey(SWT.F6);
		textAccess.typeIn(name);
		textAccess.pressEnter();
	}

	@InUIThread
	public void clickModuleFigureWithName(final String name) {
		FigureCanvas figureCanvas = getGraphicalNedEditor().getFigureCanvas();
		IFigure rootFigure = figureCanvas.getLightweightSystem().getRootFigure();
		IFigure labelFigure = findDescendantFigure(rootFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(name);
			}
		});

		new FigureAccess(labelFigure).click(LEFT_MOUSE_BUTTON);
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
		clickPaletteItem(".*Compound.*Module.*");
		clickBackground();
		
		GraphicalViewer graphicalViewer = (GraphicalViewer)ReflectionUtils.invokeMethod(getGraphicalNedEditor(), "getGraphicalViewer");
		return new CompoundModuleEditPartAccess((CompoundModuleEditPart)findDescendantEditPart(graphicalViewer.getRootEditPart(), CompoundModuleEditPart.class));
	}
}
