package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

public class TableAccess extends ControlAccess<Table> 
{
	public TableAccess(Table control) {
		super(control);
	}
	
	public Table getTable() {
		return widget;
	}

	public TableItemAccess findTableItemByContent(final String content) {
		return new TableItemAccess((TableItem)findObject(widget.getItems(), new IPredicate() {
			public boolean matches(Object object) {
				TableItem tableItem = (TableItem)object;
				
				for (int i = 0; i < widget.getColumnCount(); i++)
					if (tableItem.getText(i).matches(content))
						return true;

				return false;
			}
		}));
	}
}
