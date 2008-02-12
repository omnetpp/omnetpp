package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;

import com.simulcraft.test.gui.core.UIStep;

public class TableColumnAccess extends ClickableWidgetAccess
{
	public TableColumnAccess(TableColumn treeColumn) {
		super(treeColumn);
	}

    @Override
	public TableColumn getWidget() {
		return (TableColumn)widget;
	}

    @UIStep
    public TableAccess getTable() {
        return (TableAccess) createAccess(getWidget().getParent());
    }
	
	@UIStep
	public TableColumnAccess reveal() {
		//TODO horizontally scroll there
		return this;
	}
	
	@Override
	protected Point getAbsolutePointToClick() {
	    getTable().assertHeaderVisible();
        Table tree = (Table)getWidget().getParent();
        Point point = tree.toDisplay(getX() + getWidget().getWidth()/2, tree.getHeaderHeight()/2);
       	Assert.assertTrue("point to click is scrolled out",	getTable().getAbsoluteBounds().contains(point));
        Assert.assertTrue("column has zero width, cannot click", getWidget().getWidth() > 0);
        return point;
	}
    
    @Override
    protected Point toAbsolute(Point point) {
        // TODO:
        throw new RuntimeException();
    }

	@Override
	protected Menu getContextMenu() {
		return (Menu)getWidget().getParent().getMenu();
	}

	/**
	 * Returns the x coordinate of the left edge of the column, relative to the tree.
	 * Horizontal scrolling of tree is also taken into account.
	 */
	@UIStep
	public int getX() {
	    Table tree = (Table)getWidget().getParent();
	    TableColumn[] columns = tree.getColumns();
	    int[] columnOrder = tree.getColumnOrder();
	    int x = 0;
	    for (int i = 0; i < columns.length; i++) {
	        TableColumn col = columns[columnOrder[i]];
	        if (col == getWidget()) {
                int horizontalScrollOffset = tree.getHorizontalBar().getSelection();
                return x - horizontalScrollOffset;
            }
	        x += col.getWidth();
	    }
	    Assert.assertTrue("column is not in the tree", false);
	    return 0;
	}
	
	/**
	 * Sorts the table by this column in SWT.UP or SWT.DOWN direction.
	 */
	@UIStep
	public void sort(int direction) {
		Table table = getWidget().getParent();
		if (table.getSortColumn() != getWidget() || table.getSortDirection() != direction) {
			click();
			if (table.getSortDirection() != direction)
				click();
		}
		Assert.assertTrue("failed to sort table by " + getWidget().getText(),
				table.getSortColumn() == getWidget() && table.getSortDirection() == direction);
		
	}
}
