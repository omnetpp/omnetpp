package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.UIStep;

public class TabItemAccess extends WidgetAccess
{
	public TabItemAccess(TabItem cTabItem) {
		super(cTabItem);
	}
	
    @Override
	public TabItem getWidget() {
		return (TabItem)widget;
	}
    
    @UIStep
    public TabFolderAccess getTabFolder() {
    	return (TabFolderAccess)createAccess(getWidget().getParent());
    }

    @UIStep
    public ControlAccess getClientControl() {
        return (ControlAccess)createAccess(getWidget().getControl());
    }

    @UIStep
    public TabItemAccess reveal() {
        //XXX how to do it?
        return this;
    }

    @UIStep
    public void select() {
        int index = findInTabFolder();
        Assert.assertTrue(index != -1);
        ReflectionUtils.invokeMethod(getWidget().getParent(), "setSelection", index, true);
    }

    protected int findInTabFolder() {
        TabItem[] items = getWidget().getParent().getItems();
        for (int i=0; i<items.length; i++)
            if (items[i] == getWidget())
                return i;
        return -1;
    }

//	@Override
//	protected Menu getContextMenu() {
//		return null;
//	}

//	@Override
//	protected Point getAbsolutePointToClick() {
//		return getWidget().getParent().toDisplay(getCenter(getWidget().getBounds()));
//	}
}
