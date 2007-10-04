package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;

import com.simulcraft.test.gui.core.InUIThread;

public class TreeColumnAccess extends ClickableWidgetAccess
{
	public TreeColumnAccess(TreeColumn treeColumn) {
		super(treeColumn);
	}

	public TreeColumn getTreeColumn() {
		return (TreeColumn)widget;
	}

	@InUIThread @Override
	public void click() {
        Tree tree = (Tree)getTreeColumn().getParent();
	    Assert.assertTrue("header not visible", tree.getHeaderVisible());
		//FIXME we should ensure that the item is visible (i.e. not scrolled 
	    // horizontally off-screen). same for doubleClick() etc.
		super.click();
	}

	@InUIThread
	public TreeColumnAccess reveal() {
		//TODO scroll there
		return this;
	}
	
	@Override
	protected Point getAbsolutePointToClick() {
	    // center of the header
        Tree tree = (Tree)getTreeColumn().getParent();
	    return tree.toDisplay(getX() + getTreeColumn().getWidth()/2, tree.getHeaderHeight()/2);
	}

	@Override
	protected Menu getContextMenu() {
		return (Menu)getTreeColumn().getParent().getMenu();
	}

	/**
	 * Returns the x coordinate of the left edge of the column, relative to the tree.
	 * Horizontal scrolling of tree is also taken into account.
	 */
	@InUIThread
	public int getX() {
	    //XXX untested; cf TreeItem.get[Text]Bounds(index)
	    Tree tree = (Tree)getTreeColumn().getParent();
	    TreeColumn[] columns = tree.getColumns();
	    int[] columnOrder = tree.getColumnOrder();
	    int x = 0;
	    for (int i = 0; i < columns.length; i++) {
	        TreeColumn col = columns[columnOrder[i]];
	        if (col == getTreeColumn()) {
                int horizontalScrollOffset = tree.getHorizontalBar().getSelection();
                return x - horizontalScrollOffset;
            }
	        x += col.getWidth();
	    }
	    Assert.assertTrue("column is not in the tree", false);
	    return 0;
	}
}
