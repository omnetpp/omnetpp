package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.UIStep;

public class CTabItemAccess extends ClickableWidgetAccess
{
	public CTabItemAccess(CTabItem cTabItem) {
		super(cTabItem);
	}
	
    @Override
	public CTabItem getWidget() {
		return (CTabItem)widget;
	}
    
    @UIStep
    public CTabFolderAccess getCTabFolder() {
        return (CTabFolderAccess)createAccess(getWidget().getParent());
    }
    
    @UIStep
    public boolean isClosable() {
		return (Boolean)ReflectionUtils.getFieldValue(getWidget(), "showClose");
    }
	
	@Override
	protected Menu getContextMenu() {
		return null;
	}

	@Override
	protected Point getAbsolutePointToClick() {
		return toAbsolute(getCenter(getWidget().getBounds()));
	}
	
	@Override
	protected Point toAbsolute(Point point) {
        return getWidget().getParent().toDisplay(point);
	}
	
	@UIStep
	public void clickOnCloseIcon() {
		Assert.assertTrue("CTabItem is not closable", isClosable());
		Rectangle rect = (Rectangle)ReflectionUtils.getFieldValue(getWidget(), "closeRect");
		Assert.assertTrue("Close icon is not yet displayed", rect != null && !rect.isEmpty());
		Point center = new Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
		clickAbsolute(LEFT_MOUSE_BUTTON, getWidget().getParent().toDisplay(center));
	}
	
    @UIStep
    public CTabItemAccess reveal() {
        getWidget().getParent().showItem(getWidget());
        return this;
    }
	
}
