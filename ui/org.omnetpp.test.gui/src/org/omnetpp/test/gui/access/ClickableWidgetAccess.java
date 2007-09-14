package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;

public abstract class ClickableWidgetAccess
    extends WidgetAccess
{
	public ClickableWidgetAccess(Widget widget) {
		super(widget);
	}

	protected abstract Point getAbsolutePointToClick();

	protected abstract Menu getContextMenu();

	@InUIThread
	public void click(int button) {
		clickAbsolute(button, getAbsolutePointToClick());
	}

	@InUIThread
	public void click() {
		click(Access.LEFT_MOUSE_BUTTON);
	}

	@InUIThread
	public void rightClick() {
		click(Access.RIGHT_MOUSE_BUTTON);
	}

	@InUIThread
	public void doubleClick() {
		doubleClickAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick());
	}

	@InUIThread
	public MenuAccess activateContextMenuWithMouseClick() {
		rightClick();
		return new MenuAccess(getContextMenu());
	}

	@NotInUIThread
	public void chooseFromContextMenu(String labelPath) {
		MenuAccess menuAccess = activateContextMenuWithMouseClick();
		for (String label : labelPath.split("\\|"))
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}

}
