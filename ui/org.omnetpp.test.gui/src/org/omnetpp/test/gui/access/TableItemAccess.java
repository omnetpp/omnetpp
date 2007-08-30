package org.omnetpp.test.gui.access;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.test.gui.core.InUIThread;

public class TableItemAccess extends ClickableWidgetAccess
{
	public TableItemAccess(TableItem widget) {
		super(widget);
	}
	
	public TableItem getTableItem() {
		return (TableItem)widget;
	}

	@InUIThread
	public TableItemAccess reveal() {
		Table table = getTableItem().getParent();
		int myIndex = ArrayUtils.indexOf(table.getItems(), getTableItem());
		table.setTopIndex(myIndex); // scroll there
		return this;
	}

	@Override
	protected Point getPointToClick() {
		return getTableItem().getParent().toDisplay(getCenter(getTableItem().getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return (Menu)getTableItem().getParent().getMenu();
	}
}
