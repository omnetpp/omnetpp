package com.simulcraft.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public abstract class ClickableWidgetAccess
    extends WidgetAccess
{
	public ClickableWidgetAccess(Widget widget) {
		super(widget);
	}

	protected abstract Point getAbsolutePointToClick();
	
	protected abstract Point toAbsolute(Point point);

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
        MenuAccess menu = new MenuAccess(getContextMenu());
        menu.assertVisible();
        return menu;
	}

	@InUIThread
    public MenuAccess activateContextMenuWithMouseClick(Point point) {
        clickAbsolute(Access.RIGHT_MOUSE_BUTTON, toAbsolute(point));
        MenuAccess menu = new MenuAccess(getContextMenu());
        menu.assertVisible();
        return menu;
    }

	@NotInUIThread
	public void chooseFromContextMenu(String labelPath) {
		MenuAccess menuAccess = activateContextMenuWithMouseClick();
		for (String label : labelPath.split("\\|"))
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}
	
    @InUIThread
	public void dragToAbsolute(int x, int y) {
	    dragMouseAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick(), new Point(x,y));    
	}

    @InUIThread
	public void dragTo(ClickableWidgetAccess target) {
	    dragMouseAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick(), target.getAbsolutePointToClick());    
	}

}
