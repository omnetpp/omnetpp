package com.simulcraft.test.gui.access;

import java.util.ArrayList;

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

	@Override
	public Table getControl() {
		return (Table)widget;
	}

    @InUIThread
    public void assertEmpty() {
        Assert.assertTrue(getControl().getItemCount() == 0);
    }

    @InUIThread
    public void assertNotEmpty() {
        Assert.assertTrue(getControl().getItemCount() != 0);
    }

    @InUIThread
    public void assertHeaderVisible() {
        Assert.assertTrue(getControl().getHeaderVisible());
    }

    @InUIThread
	public TableItemAccess findTableItemByContent(final String content) {
		return new TableItemAccess((TableItem)findObject(getControl().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				// true if the "main" text or any column text matches
				TableItem tableItem = (TableItem)object;
				log(debug, "  checking: " + tableItem);
				if (tableItem.getText().matches(content)) // not the same as column[0]!
					return true;
				for (int i = 0; i < getControl().getColumnCount(); i++)
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
        return new TableColumnAccess(getControl().getColumns()[index]);
    }

    @InUIThread
    public int[] getTableColumnOrder() {
        return getControl().getColumnOrder();
    }

    @InUIThread
    public TableItemAccess[] getSelection() {
        ArrayList<TableItemAccess> result = new ArrayList<TableItemAccess>(); 
        for (TableItem item : getControl().getSelection())
            result.add((TableItemAccess)createAccess(item));
        return result.toArray(new TableItemAccess[]{});
    }

}
