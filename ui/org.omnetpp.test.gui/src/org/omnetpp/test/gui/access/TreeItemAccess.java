package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.test.gui.core.InUIThread;

public class TreeItemAccess extends ClickableWidgetAccess
{
	public TreeItemAccess(TreeItem treeItem) {
		super(treeItem);
	}

	public TreeItem getTreeItem() {
		return (TreeItem)widget;
	}

	@InUIThread @Override
	public void click() {
		//FIXME we must ensure that the item is visible (i.e. not scrolled off-screen,
		// or in an un-expanded subtree). same for doubleClick() etc.
		super.click();
	}

	@InUIThread
	public TreeItemAccess reveal() {
		getTreeItem().getParent().setTopItem(getTreeItem()); // scroll there
		return this;
	}
	
	@Override
	protected Point getPointToClick() {
		return getTreeItem().getParent().toDisplay(getCenter(getTreeItem().getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return (Menu)getTreeItem().getParent().getMenu();
	}
}
