package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.EditPartAccess;
import com.simulcraft.test.gui.access.FigureAccess;
import com.simulcraft.test.gui.access.FlyoutPaletteCompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;

public class CompoundModuleEditPartAccess extends EditPartAccess
{
	public CompoundModuleEditPartAccess(CompoundModuleEditPart compoundModuleeditPart) {
		super(compoundModuleeditPart);
	}
	
	public CompoundModuleEditPart getCompoundModuleEditPart() {
		return (CompoundModuleEditPart)editPart;
	}

	@UIStep
	public FlyoutPaletteCompositeAccess getFlyoutPaletteComposite() {
		Composite composite = editPart.getRoot().getViewer().getControl().getParent().getParent();
		return new FlyoutPaletteCompositeAccess((FlyoutPaletteComposite)findDescendantControl(composite, FlyoutPaletteComposite.class));
	}

	@InBackgroundThread
	public void createSubModuleWithPalette(String type, String name, int x, int y) {
		getFlyoutPaletteComposite().clickButtonFigureWithLabel(type);
		clickBackground(x, y);
		if (name != null)
		    typeInNewName(name);
	}

	@UIStep
	public void clickBackground(int x, int y) {
		FigureAccess figureAccess = new FigureAccess(getFigure());
		figureAccess.clickAbsolute(LEFT_MOUSE_BUTTON, figureAccess.toDisplay(x, y));
	}

	@InBackgroundThread
	public void renameSubmodule(String oldName, String newName) {
		clickSubmoduleFigureWithName(oldName);
		typeInNewName(newName);
	}

	@InBackgroundThread
	public void typeInNewName(String name) {
        pressKey(SWT.F6);
		Canvas canvas = new FigureAccess(getFigure()).getCanvas();
		Text text = (Text)Access.findDescendantControl(canvas, Text.class);
		TextAccess textAccess = new TextAccess(text);
		textAccess.typeIn(name);
		textAccess.pressEnter();
	}

	@UIStep
	public void clickSubmoduleFigureWithName(final String name) {
		findSubmoduleFigureByName(name).click(LEFT_MOUSE_BUTTON);
	}

    @UIStep
    public FigureAccess findSubmoduleFigureByName(final String name) {
        IFigure compoundModuleFigure = getFigure();
		return new FigureAccess(findDescendantFigure(compoundModuleFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(name);
			}
		}));
    }

    @InBackgroundThread
    public void createConnectionWithPalette(String name1, String name2, String connectionOptionLabel) {
        createConnectionWithPalette("Connection", name1, name2, connectionOptionLabel);
    }

	@InBackgroundThread
    public void createConnectionWithPalette(String channel, String name1, String name2, String connectionOptionLabel) {
	    FlyoutPaletteCompositeAccess flyoutPaletteComposite = getFlyoutPaletteComposite();
        flyoutPaletteComposite.ensureButtonFigureWithLabelSelected(channel);
        clickSubmoduleFigureWithName(name1);
        clickSubmoduleFigureWithName(name2);
        clickConnectionOption(connectionOptionLabel);
	}

	@UIStep
	protected void clickConnectionOption(String label) {
		new MenuAccess(getDisplay().getActiveShell().getMenu()).activateMenuItemWithMouse(label);
	}

	protected IFigure getFigure() {
		IFigure compoundModuleFigure = (IFigure)ReflectionUtils.invokeMethod(editPart, "getCompoundModuleFigure");
		return compoundModuleFigure;
	}
}
