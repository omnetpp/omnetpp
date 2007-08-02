package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.common.util.ReflectionUtils;

public class MenuItemAccess extends WidgetAccess<MenuItem>
{
	public MenuItemAccess(MenuItem menuItem) {
		super(menuItem);
	}
	
	public MenuItem getMenuItem() {
		return widget;
	}

	public MenuAccess activateWithMouseClick() {
		Rectangle parentRectangle = (Rectangle)ReflectionUtils.invokeMethod(widget.getParent(), "getBounds");
		Rectangle rectangle = (Rectangle)ReflectionUtils.invokeMethod(widget, "getBounds");
		rectangle.x = rectangle.x + parentRectangle.x;
		rectangle.y = rectangle.y + parentRectangle.y;
		clickCenter(1, rectangle);

		if (widget.getMenu() == null)
			return null;
		else
			return new MenuAccess(widget.getMenu());
	}
}
