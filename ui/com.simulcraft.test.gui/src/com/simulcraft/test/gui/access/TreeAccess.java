package com.simulcraft.test.gui.access;

import java.util.ArrayList;
import java.util.List;

import junit.framework.Assert;

import org.eclipse.core.runtime.Path;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;


public class TreeAccess extends ControlAccess
{
	public TreeAccess(Tree tree) {
		super(tree);
	}

	public Tree getTree() {
		return (Tree)widget;
	}

	@InUIThread
	public void assertEmpty() {
		Assert.assertTrue(getTree().getItemCount() == 0);
	}

    @InUIThread
    public void assertNotEmpty() {
        Assert.assertTrue(getTree().getItemCount() != 0);
    }

    @InUIThread
    public void assertHeaderVisible() {
        Assert.assertTrue(getTree().getHeaderVisible());
    }

    /**
     * Compares the Tree's content (one or more trees) to the GenericTreeNode
     * trees passed. GenericTreeNode's payloads MUST be strings containing *regex* labels.
     * If you need literal (non-regex) match, quote the strings beforehand using
     * Pattern.quote().
     */
    @NotInUIThread
    public void assertContent(GenericTreeNode... content) {
    	expandAll(); // expand all items because the Tree can be virtual
    	Assert.assertTrue("tree content does not match expected content", compareContent(getItems(), content));
    }
    
    @InUIThread
    public TreeItem[] getItems() {
    	return getTree().getItems();
    }
    
    public TreeItemAccess[] getItemAccesses() {
    	TreeItem[] items = getItems();
    	TreeItemAccess[] treeItems = new TreeItemAccess[items.length];
    	for (int i = 0; i < items.length; ++i)
    		treeItems[i] = (TreeItemAccess)createAccess(items[i]);
    	return treeItems;
    }
    
    @NotInUIThread
    public void expandAll() {
    	for (TreeItemAccess item : getItemAccesses()) {
    		item.reveal();
    		item.click();
        	pressKey(SWT.KEYPAD_MULTIPLY, SWT.NONE); // fully expands the subtree (right arrow or Plus only expands this node)
    	}
    }
	
    @InUIThread
	public TreeItemAccess findTreeItemByPath(String path) {
        // TODO: Note that findTreeItemByContent() does deep search, so this code won't work
        // if any segment is not fully unique in the tree!
        for (String pathSegment : new Path(path).removeLastSegments(1).segments())
            findTreeItemByContent(pathSegment).ensureExpanded();

        return findTreeItemByContent(new Path(path).lastSegment()).reveal();
	}

	@InUIThread
	public TreeItemAccess findTreeItemByContent(final String content) {
		TreeItem treeItem = (TreeItem)theOnlyWidget(collectTreeItems(getTree().getItems(), new IPredicate() {
			public boolean matches(Object object) {
				TreeItem treeItem = (TreeItem)object;
				log(debug, "  checking: " + treeItem);
				String treeItemContent = treeItem.getText();
				return treeItemContent.matches(content);
			}
		}));
		log(debug, treeItem + ":" + treeItem.getBounds()); 
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
	
	@InUIThread
	protected boolean compareContent(TreeItem[] treeItems, GenericTreeNode[] expectedItems) {
		if (treeItems.length != expectedItems.length) {
            System.out.format("Tree content mismatch. Expected %d children, found %d", expectedItems.length, treeItems.length);
			return false;
		}
			
		for (int i = 0; i < treeItems.length; ++i) {
			TreeItem item = treeItems[i];
			GenericTreeNode expected = expectedItems[i];
			
			if (!item.getText().matches((String)expected.getPayload())) {
				System.out.format("Tree content mismatch. Expected regex: %s, found: %s%n", expected.getPayload(), item.getText());
				return false;
			}
			if (!compareContent(item.getItems(), expected.getChildren()))
				return false;
		}
		
		return true;
	}
	
    /**
     * Return the given column; columns are in creation order. 
     */
	@InUIThread
	public TreeColumnAccess getTreeColumn(int index) {
	    return new TreeColumnAccess(getTree().getColumns()[index]);
	}

	@InUIThread
	public int[] getTreeColumnOrder() {
	    return getTree().getColumnOrder();
	}

}
