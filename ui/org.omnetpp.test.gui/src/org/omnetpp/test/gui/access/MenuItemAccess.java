package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.test.gui.core.InUIThread;

public class MenuItemAccess extends ClickableWidgetAccess
{
	public MenuItemAccess(MenuItem menuItem) {
		super(menuItem);
	}

	public MenuItem getMenuItem() {
		return (MenuItem)widget;
	}

	@InUIThread
	public void assertEnabled() {
		if (!getMenuItem().isEnabled()) {
	    	MenuAccess.closeMenus();
	    	Assert.assertTrue("menu item is disabled", false);
		}
	}
	
	@InUIThread
	public MenuAccess activateWithMouseClick() {
		assertEnabled();
		click();
		return getMenuItem().getMenu() == null ? null : new MenuAccess(getMenuItem().getMenu());
	}

	@Override
	protected Point getPointToClick() {
		Rectangle parentRectangle = (Rectangle)ReflectionUtils.invokeMethod(getMenuItem().getParent(), "getBounds");
		Rectangle rectangle = (Rectangle)ReflectionUtils.invokeMethod(getMenuItem(), "getBounds");
		rectangle.x = rectangle.x + parentRectangle.x;
		rectangle.y = rectangle.y + parentRectangle.y;
		return getCenter(rectangle);
	}

	@Override
	protected Menu getContextMenu() {
		return null;  // a menu has no context menu
	}
}
