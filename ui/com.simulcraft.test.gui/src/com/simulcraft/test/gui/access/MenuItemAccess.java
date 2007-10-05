package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.InUIThread;

public class MenuItemAccess extends ClickableWidgetAccess
{
	public MenuItemAccess(MenuItem menuItem) {
		super(menuItem);
	}

    @Override
	public MenuItem getWidget() {
		return (MenuItem)widget;
	}

	@InUIThread
	public void assertEnabled() {
		if (!getWidget().isEnabled()) {
	    	MenuAccess.closeMenus();
	    	Assert.assertTrue("menu item '"+getWidget().getText()+"' should be enabled", false);
		}
	}
	
    @InUIThread
    public void assertDisabled() {
        if (getWidget().isEnabled()) {
            MenuAccess.closeMenus();
            Assert.assertTrue("menu item '"+getWidget().getText()+"' should be disabled", false);
        }
    }
    
	@InUIThread
	public MenuAccess activateWithMouseClick() {
		assertEnabled();
		click();
		return getWidget().getMenu() == null ? null : new MenuAccess(getWidget().getMenu());
	}

	@Override
	protected Point getAbsolutePointToClick() {
		Rectangle parentRectangle = (Rectangle)ReflectionUtils.invokeMethod(getWidget().getParent(), "getBounds");
		Rectangle rectangle = (Rectangle)ReflectionUtils.invokeMethod(getWidget(), "getBounds");
		rectangle.x = rectangle.x + parentRectangle.x;
		rectangle.y = rectangle.y + parentRectangle.y;
		return getCenter(rectangle);
	}

	@Override
	protected Menu getContextMenu() {
		return null;  // a menu has no context menu
	}
}
