package org.omnetpp.test.gui.access;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
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
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class CompoundModuleEditPartAccess extends EditPartAccess
{
	public CompoundModuleEditPartAccess(CompoundModuleEditPart compoundModuleeditPart) {
		super(compoundModuleeditPart);
	}
	
	public CompoundModuleEditPart getCompoundModuleEditPart() {
		return (CompoundModuleEditPart)editPart;
	}

	@InUIThread
	public FlyoutPaletteCompositeAccess getFlyoutPaletteComposite() {
		Composite composite = editPart.getRoot().getViewer().getControl().getParent().getParent();
		return new FlyoutPaletteCompositeAccess((FlyoutPaletteComposite)findDescendantControl(composite, FlyoutPaletteComposite.class));
	}

	@NotInUIThread
	public void createSubModuleWithPalette(String type, String name, int x, int y) {
		getFlyoutPaletteComposite().clickButtonFigureWithLabel(type);
		clickBackground(x, y);
		String namePattern = Character.toLowerCase(type.charAt(0)) + type.substring(1) + ".*";
		renameSubmodule(namePattern, name);
	}

	@InUIThread
	public void clickBackground(int x, int y) {
		FigureAccess figureAccess = new FigureAccess(getFigure());
		figureAccess.clickAbsolute(LEFT_MOUSE_BUTTON, figureAccess.toDisplay(x, y));
	}

	@NotInUIThread
	public void renameSubmodule(String oldName, String newName) {
		clickSubmoduleFigureWithName(oldName);
		typeInNewName(newName);
	}

	@InUIThread
	public void typeInNewName(String name) {
		Canvas canvas = new FigureAccess(getFigure()).getCanvas();
		Text text = (Text)Access.findDescendantControl(canvas, Text.class);
		TextAccess textAccess = new TextAccess(text);
		textAccess.pressKey(SWT.F6);
		textAccess.typeIn(name);
		textAccess.pressEnter();
	}

	@InUIThread
	public void clickSubmoduleFigureWithName(final String name) {
		findSubmoduleFigureByName(name).click(LEFT_MOUSE_BUTTON);
	}

    @InUIThread
    public FigureAccess findSubmoduleFigureByName(final String name) {
        IFigure compoundModuleFigure = getFigure();
		return new FigureAccess(findDescendantFigure(compoundModuleFigure, new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Label && ((Label)object).getText().matches(name);
			}
		}));
    }

    @NotInUIThread
    public void createConnectionWithPalette(String name1, String name2, String connectionOptionLabel) {
        createConnectionWithPalette("Connection", name1, name2, connectionOptionLabel);
    }

	@NotInUIThread
    public void createConnectionWithPalette(String channel, String name1, String name2, String connectionOptionLabel) {
	    FlyoutPaletteCompositeAccess flyoutPaletteComposite = getFlyoutPaletteComposite();
        flyoutPaletteComposite.clickButtonFigureWithLabel(channel);
        clickSubmoduleFigureWithName(name1);
        clickSubmoduleFigureWithName(name2);
        clickConnectionOption(connectionOptionLabel);
	}

	@InUIThread
    public void fuck(FigureAccess figureAccess, String channel) {
	    xxx(figureAccess.getCanvas());
	    MenuAccess menuAccess = new MenuAccess(getDisplay().getActiveShell().getMenu());
	    Access.dumpMenu(menuAccess.getMenu());
        menuAccess.activateMenuItemWithMouse(channel);
    }
	
	private void xxx(Control control) {
        System.out.println("************");
        while (control != null) {
            System.out.println(control);
            if (control.getMenu() != null)
                Access.dumpMenu(control.getMenu());
            control = control.getParent();
        }
    }

	@InUIThread
	protected void clickConnectionOption(String label) {
		new MenuAccess(getDisplay().getActiveShell().getMenu()).activateMenuItemWithMouse(label);
	}

	protected IFigure getFigure() {
		IFigure compoundModuleFigure = (IFigure)ReflectionUtils.invokeMethod(editPart, "getCompoundModuleFigure");
		return compoundModuleFigure;
	}
}
