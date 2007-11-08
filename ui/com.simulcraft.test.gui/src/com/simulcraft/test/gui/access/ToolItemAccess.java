package com.simulcraft.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.ToolItem;

import com.simulcraft.test.gui.core.InBackgroundThread;
import com.simulcraft.test.gui.core.UIStep;

public class ToolItemAccess 
    extends ClickableWidgetAccess
{
    public ToolItemAccess(ToolItem toolItem) {
        super(toolItem);
    }
    
    @Override
    public ToolItem getWidget() {
        return (ToolItem)widget;
    }

    @Override
    protected Point getAbsolutePointToClick() {
        return toAbsolute(getCenter(getWidget().getBounds()));
    }

    @Override
    protected Point toAbsolute(Point point) {
        return getWidget().getParent().toDisplay(point);
    }

    @Override
    protected Menu getContextMenu() {
        return null;
    }
    
    @InBackgroundThread
    public MenuAccess activateDropDownMenu() {
        clickDropDownArrow();
        return MenuAccess.findPopupMenu();
    }

    @UIStep
    public void clickDropDownArrow() {
        Rectangle r = getWidget().getBounds();
        clickAbsolute(Access.LEFT_MOUSE_BUTTON, toAbsolute(new Point(r.x + r.width - 1, r.y + r.height / 2)));
    }
}
