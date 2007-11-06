package com.simulcraft.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Widget;

import com.simulcraft.test.gui.core.InAnyThread;
import com.simulcraft.test.gui.core.InBackgroundThread;
import com.simulcraft.test.gui.core.UIStep;

public abstract class ClickableWidgetAccess
    extends WidgetAccess
{
	public ClickableWidgetAccess(Widget widget) {
		super(widget);
	}

	protected abstract Point getAbsolutePointToClick();
	
	protected abstract Point toAbsolute(Point point);

	protected abstract Menu getContextMenu();

	@UIStep
	public void click(int button) {
		clickAbsolute(button, getAbsolutePointToClick());
	}

	@UIStep
	public void click() {
		click(Access.LEFT_MOUSE_BUTTON);
	}

	@UIStep
	public void rightClick() {
		click(Access.RIGHT_MOUSE_BUTTON);
	}

	@UIStep
	public void doubleClick() {
		doubleClickAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick());
	}

	@UIStep
	public MenuAccess activateContextMenuWithMouseClick() {
        rightClick();
        MenuAccess menu = new MenuAccess(getContextMenu());
        menu.assertVisible();
        return menu;
	}

	@UIStep
    public MenuAccess activateContextMenuWithMouseClick(Point point) {
        clickAbsolute(Access.RIGHT_MOUSE_BUTTON, toAbsolute(point));
        MenuAccess menu = new MenuAccess(getContextMenu());
        menu.assertVisible();
        return menu;
    }

	@InBackgroundThread
	public void chooseFromContextMenu(String labelPath) {
		MenuAccess menuAccess = activateContextMenuWithMouseClick();
		for (String label : labelPath.split("\\|"))
			menuAccess = menuAccess.activateMenuItemWithMouse(label);
	}

    @InAnyThread
	public void dragToAbsolute(int x, int y) {
	    dragMouseAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick(), new Point(x,y));    
	}

    @InAnyThread
	public void dragTo(ClickableWidgetAccess target) {
	    dragMouseAbsolute(LEFT_MOUSE_BUTTON, getAbsolutePointToClick(), target.getAbsolutePointToClick());    
	}
}
