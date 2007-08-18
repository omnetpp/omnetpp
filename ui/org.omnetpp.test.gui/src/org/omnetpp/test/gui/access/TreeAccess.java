package org.omnetpp.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.test.gui.InUIThread;

public class TreeAccess extends ControlAccess<Tree>
{
	public TreeAccess(Tree tree) {
		super(tree);
	}

	public Tree getTree() {
		return widget;
	}

	@InUIThread
	public TreeItemAccess findTreeItemByContent(final String content) {
		TreeItem treeItem = (TreeItem)theOnlyWidget(collectTreeItems(getTree().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				TreeItem treeItem = (TreeItem)object;
				System.out.println("  checking: " + treeItem);
				String treeItemContent = treeItem.getText();
				return treeItemContent.matches(content);
			}
		}));
		System.out.println(treeItem + ":" + treeItem.getBounds()); 
		return new TreeItemAccess(treeItem);
	}

	@InUIThread
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
