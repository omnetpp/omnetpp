package com.simulcraft.test.gui.access;

import junit.framework.Assert;

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
    public void assertEmpty() {
        Assert.assertTrue(getTable().getItemCount() == 0);
    }

    @InUIThread
    public void assertNotEmpty() {
        Assert.assertTrue(getTable().getItemCount() != 0);
    }

    @InUIThread
    public void assertHeaderVisible() {
        Assert.assertTrue(getTable().getHeaderVisible());
    }

    @InUIThread
	public TableItemAccess findTableItemByContent(final String content) {
		return new TableItemAccess((TableItem)findObject(getTable().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				// true if the "main" text or any column text matches
				TableItem tableItem = (TableItem)object;
				log(debug, "  checking: " + tableItem);
				if (tableItem.getText().matches(content)) // not the same as column[0]!
					return true;
				for (int i = 0; i < getTable().getColumnCount(); i++)
					if (tableItem.getText(i).matches(content))
						return true;
				return false;
			}
		}));
	}

    /**
     * Return the given column; columns are in creation order. 
     */
    @InUIThread
    public TableColumnAccess getTableColumn(int index) {
        return new TableColumnAccess(getTable().getColumns()[index]);
    }

    @InUIThread
    public int[] getTableColumnOrder() {
        return getTable().getColumnOrder();
    }

}
