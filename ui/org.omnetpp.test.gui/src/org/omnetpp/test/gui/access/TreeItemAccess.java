package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.TreeItem;

public class TreeItemAccess extends WidgetAccess<TreeItem>
{
	public TreeItemAccess(TreeItem treeItem) {
		super(treeItem);
	}
	
	public TreeItem getTreeItem() {
		return widget;
	}

	public MenuAccess activateContextMenuWithMouseClick() {
		click(3, widget.getParent().toDisplay(getCenter(widget.getBounds())));
		Menu menu = (Menu)widget.getParent().getMenu();
		return new MenuAccess(menu);
	}
}
