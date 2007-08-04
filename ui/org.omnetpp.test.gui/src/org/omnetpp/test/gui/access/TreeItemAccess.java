package org.omnetpp.test.gui.access;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.TreeItem;

public class TreeItemAccess extends ClickableWidgetAccess<TreeItem>
{
	public TreeItemAccess(TreeItem treeItem) {
		super(treeItem);
	}
	
	public TreeItem getTreeItem() {
		return widget;
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
