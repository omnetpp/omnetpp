package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;

public class CTabItemAccess extends ClickableWidgetAccess
{
	public CTabItemAccess(CTabItem cTabItem) {
		super(cTabItem);
	}
	
	public CTabItem getCTabItem() {
		return (CTabItem)widget;
	}

	@Override
	protected Menu getContextMenu() {
		return null;
	}

	@Override
	protected Point getPointToClick() {
		return getCTabItem().getParent().toDisplay(getCenter(getCTabItem().getBounds()));
	}
}
