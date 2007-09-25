package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.InUIThread;


public class TableAccess extends ControlAccess
{
	public TableAccess(Table control) {
		super(control);
	}

	public Table getTable() {
		return (Table)widget;
	}

	@InUIThread
	public TableItemAccess findTableItemByContent(final String content) {
		return new TableItemAccess((TableItem)findObject(getTable().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				// true if the "main" text or any column text matches
				TableItem tableItem = (TableItem)object;
				System.out.println("  checking: " + tableItem);
				if (tableItem.getText().matches(content)) // not the same as column[0]!
					return true;
				for (int i = 0; i < getTable().getColumnCount(); i++)
					if (tableItem.getText(i).matches(content))
						return true;
				return false;
			}
		}));
	}
}
