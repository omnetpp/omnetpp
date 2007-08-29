package org.omnetpp.test.gui.access;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.test.gui.core.InUIThread;

public class TableItemAccess extends ClickableWidgetAccess<TableItem>
{
	public TableItemAccess(TableItem widget) {
		super(widget);
	}

	@InUIThread
	public TableItemAccess reveal() {
		Table table = widget.getParent();
		int myIndex = ArrayUtils.indexOf(table.getItems(), widget);
		table.setTopIndex(myIndex); // scroll there
		return this;
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
