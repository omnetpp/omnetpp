package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.test.gui.InUIThread;

public abstract class ClickableWidgetAccess<T extends Widget> extends WidgetAccess<T>
{
	public ClickableWidgetAccess(T widget) {
		super(widget);
	}

	@InUIThread
	public MenuAccess activateContextMenuWithMouseClick() {
		rightClick();
		return new MenuAccess(getContextMenu());
	}

//	//FIXME doesn't work -- looks like there right-click and activate-with-mouse must be separate steps
//	@InUIThread
//	public MenuAccess activateContextMenuItemWithMouse(String label) {
//		// Note: we must click the menu immediately here, because by the time
//		// the next Display.syncExec() runs the menu will be long gone!
//		// See the runPopups() private method of Display
//		rightClick();
//		return new MenuAccess(getContextMenu()).activateMenuItemWithMouse(label);
//	}

	protected abstract Point getPointToClick();

	protected abstract Menu getContextMenu();

	@InUIThread
	public void click(int button) {
		click(button, getPointToClick());
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
		doubleClick(LEFT_MOUSE_BUTTON, getPointToClick());
	}
}
