package com.simulcraft.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.InBackgroundThread;

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
        Control parent = (Control)ReflectionUtils.getFieldValue(getWidget(), "parent");
        return MenuAccess.withOpeningContextMenu(parent,
            new Runnable() {
                public void run() {
                    Rectangle r = getWidget().getBounds();
                    clickAbsolute(Access.LEFT_MOUSE_BUTTON, toAbsolute(new Point(r.x + r.width - 1, r.y + r.height / 2)));
                }
        });
    }
}
