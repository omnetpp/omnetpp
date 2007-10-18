package com.simulcraft.test.gui.access;

import java.util.ArrayList;

import junit.framework.Assert;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.util.Predicate;


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
        Assert.assertTrue("table is not empty", getControl().getItemCount() == 0);
    }

    @InUIThread
    public void assertNotEmpty() {
        Assert.assertTrue("table is empty", getControl().getItemCount() != 0);
    }
    
    @InUIThread
    public void assertItemCount(int count) {
        Assert.assertTrue("Expected " + count + " item, found " + getControl().getItemCount() + " in table", getControl().getItemCount() == count);
    }

    @InUIThread
    public void assertHeaderVisible() {
        Assert.assertTrue("table header not visible", getControl().getHeaderVisible());
    }

    @InUIThread
    public void assertHeaderNotVisible() {
        Assert.assertTrue("table header visible", !getControl().getHeaderVisible());
    }

    @InUIThread
    public void assertContent(String items[]) {
        Assert.assertEquals("Table item count does not match.", items.length, getControl().getItemCount());
        for (int i = 0; i< items.length; ++i) {
            String itemText = getControl().getItem(i).getText();
            Assert.assertTrue("Table item does not match. Expected: '"+items[i]+"' found: '"+itemText+"'", itemText.matches(items[i]));
        }
    }
    
    @InUIThread
    public void assertContent(String[]... items) {
        Assert.assertEquals("Table item count does not match.", items.length, getControl().getItemCount());
        for (int i = 0; i< items.length; ++i) {
        	String[] item = items[i];
        	Assert.assertTrue("Table does not have enough columns. Expected: " + item.length + ", found: " + getControl().getColumnCount(),
        			item.length <= getControl().getColumnCount());
        	for (int j = 0; j < item.length; ++j) {
        		String itemText = getControl().getItem(i).getText(j);
        		Assert.assertTrue("Table item does not match. Expected: '"+item[j]+"' found: '"+itemText+"'", itemText.matches(item[j]));
        	}
        }
    }
    
    /**
     * Table starts with the provided items, the rest of the table should match restShouldMatch or can be anything
     * if restShouldMatch is NULL.
     */
    @InUIThread
    public void assertContentStartWith(String items[], String restShouldMatch) {
    	assertContentStartWith(0, items, restShouldMatch);
    }
    
    @InUIThread
    public void assertContentStartWith(int columnIndex, String items[], String restShouldMatch) {
        Assert.assertTrue("Table item count less than expected.", items.length <= getControl().getItemCount());
        for (int i = 0; i< items.length; ++i) {
            String itemText = getControl().getItem(i).getText(columnIndex);
            Assert.assertTrue("Table item does not match. Expected: '"+items[i]+"' found: '"+itemText+"'", itemText.matches(items[i]));
        }
        if (restShouldMatch != null)
            for (int i = items.length; i < getControl().getItemCount(); ++i) {
                String itemText = getControl().getItem(i).getText(columnIndex);
                Assert.assertTrue("Table item does not match. Expected: '"+restShouldMatch+"' found: '"+itemText+"'", itemText.matches(restShouldMatch));
            }
    }
    
    public void assertColumnContentMatches(String columnName, String pattern) {
    	assertColumnContentStartWith(getTableColumnIndex(columnName), ArrayUtils.EMPTY_STRING_ARRAY, pattern);
    }
    
    public void assertColumnContentMatches(TableColumnAccess column, String pattern) {
    	assertColumnContentStartWith(getTableColumnIndex(column), ArrayUtils.EMPTY_STRING_ARRAY, pattern);
    }
    
    @InUIThread
    public void assertColumnContentStartWith(int columnIndex, String[] items, String restShouldMatch) {
    	assertContentStartWith(columnIndex, items, restShouldMatch);
    }
    
    @InUIThread
    public void assertSelectionCount(int count) {
    	Assert.assertEquals("Selection count does not match,", count, getControl().getSelectionCount());
    }
    
	public TableItemAccess findTableItemByContent(String content) {
		return findTableItemByContent(0, content);
	}
	
	@InUIThread
	public TableItemAccess findTableItemByContent(String columnName, String content) {
		return findTableItemByContent(getTableColumnIndex(columnName), content);
	}
	
    @InUIThread
	public TableItemAccess findTableItemByContent(int columnIndex, final String content) {
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

    @InUIThread
    public TableColumnAccess[] getTableColumns() {
        ArrayList<TableColumnAccess> result = new ArrayList<TableColumnAccess>(); 
        for (TableColumn item : getControl().getColumns())
            result.add(wrapTableColumn(item));
        return result.toArray(new TableColumnAccess[]{});
    }

    /**
     * Return the given column; columns are in creation order. 
     */
    @InUIThread
    public TableColumnAccess getTableColumn(int index) {
        return new TableColumnAccess(getControl().getColumns()[index]);
    }
    
    @InUIThread
    public TableColumnAccess getTableColumn(String columnName) {
    	return wrapTableColumn(findObject(getControl().getColumns(), Predicate.itemWithText(columnName)));
    }
    
    @InUIThread
    public int getTableColumnIndex(String columnName) {
    	return getTableColumnIndex(getTableColumn(columnName));
    }
    
    @InUIThread
    public int getTableColumnIndex(TableColumnAccess column) {
    	return getControl().indexOf(column.getWidget());
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
    
    private TableColumnAccess wrapTableColumn(Object column) {
    	return column != null ? (TableColumnAccess)createAccess(column) : null;
    }
}
