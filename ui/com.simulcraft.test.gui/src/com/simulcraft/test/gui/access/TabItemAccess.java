package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.TabItem;

public class TabItemAccess extends WidgetAccess
{
	public TabItemAccess(TabItem cTabItem) {
		super(cTabItem);
	}
	
    @Override
	public TabItem getWidget() {
		return (TabItem)widget;
	}
    
    public CompositeAccess getParent() {
    	return (CompositeAccess)createAccess(getWidget().getParent());
    }
    
    public ControlAccess getControl() {
    	return (ControlAccess)createAccess(getWidget().getControl());
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
