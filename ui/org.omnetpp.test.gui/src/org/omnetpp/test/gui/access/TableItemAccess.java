package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.TableItem;

public class TableItemAccess extends ClickableWidgetAccess<TableItem>
{
	public TableItemAccess(TableItem widget) {
		super(widget);
	}

	@Override
	protected Point getPointToClick() {
		return widget.getParent().toDisplay(getCenter(widget.getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return (Menu)widget.getParent().getMenu();
	}
}
