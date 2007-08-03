package org.omnetpp.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;

public class TreeAccess extends ControlAccess<Tree>
{
	public TreeAccess(Tree tree) {
		super(tree);
	}
	
	public Tree getTree() {
		return widget;
	}

	public TreeItemAccess findTreeItemByContent(final String content) {
		return new TreeItemAccess((TreeItem)theOnlyWidget(collectTreeItems(getTree().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				String treeItemContent = ((TreeItem)object).getText();
				
				if (debug)
					System.out.println("checking tree item: " + treeItemContent);
				return treeItemContent.matches(content);
			}
		})));
	}
	
	public List<TreeItem> collectTreeItems(TreeItem[] treeItems, IPredicate predicate) {
		ArrayList<TreeItem> resultTreeItems = new ArrayList<TreeItem>();
		collectTreeItems(treeItems, predicate, resultTreeItems);
		return resultTreeItems;
	}

	protected void collectTreeItems(TreeItem[] treeItems, IPredicate predicate, List<TreeItem> resultTreeItems) {
		for (TreeItem treeItem : treeItems) {
			collectTreeItems(treeItem.getItems(), predicate, resultTreeItems);

			if (predicate.matches(treeItem))
				resultTreeItems.add(treeItem);
		}
	}
}
