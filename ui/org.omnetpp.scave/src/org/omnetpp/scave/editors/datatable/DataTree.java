package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.scave.editors.datatable.ResultFileManagerTreeContentProvider.Node;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;

/**
 * This is a class that Levy didn't have time to document :)
 *
 * @author Levy
 */
public class DataTree extends Tree implements IDataControl {
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");
    protected ListenerList listeners;
    protected ResultFileManager manager;
    protected IDList idList;
    protected ResultFileManagerTreeContentProvider contentProvider;

    public DataTree(Composite parent, int style) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        setHeaderVisible(true);
        setLinesVisible(true);
        setMenu(contextMenuManager.createContextMenu(this));
        addColumn("Name", 400);
        addColumn("Value", 200);
        contentProvider = new ResultFileManagerTreeContentProvider();

        addListener(SWT.SetData, new Listener() {
            public void handleEvent(final Event e) {
                ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                    public Object call() {
                        TreeItem item = (TreeItem)e.item;
                        int index = item.getParentItem() == null ? indexOf(item) : item.getParentItem().indexOf(item);
                        fillTreeItem(item, index);
                        return null;
                    }
                });
            }
        });
    }

    public ResultFileManagerTreeContentProvider getContentProvider() {
        return contentProvider;
    }

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public void setResultFileManager(ResultFileManager manager) {
        this.manager = manager;
        contentProvider.setResultFileManager(manager);
    }

    public IDList getIDList() {
        return idList;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
        contentProvider.setIDList(idList);
        refresh();
        fireContentChangedEvent();
    }

    public IMenuManager getContextMenuManager() {
        return contextMenuManager;
    }

    public String getSelectedField() {
        return null;
    }

    public ResultItem getSelectedItem() {
        ResultItem[] resultItems = getSelectedItems();

        if (resultItems.length == 1)
            return resultItems[0];
        else
            return null;
    }

    public ResultItem[] getSelectedItems() {
        IDList idList = getSelectedIDs();
        ArrayList<ResultItem> resultItems = new ArrayList<ResultItem>();

        for (int i = 0; i < idList.size(); i++)
            resultItems.add(manager.getItem(idList.get(i)));

        return resultItems.toArray(new ResultItem[0]);
    }

    public IDList getSelectedIDs() {
        return ResultFileManager.callWithReadLock(manager, new Callable<IDList>() {
            public IDList call() throws Exception {
                IDList resultIdList = new IDList();
                TreeItem[] treeItems = getSelection();
                for (int i = 0; i < idList.size(); i++) {
                    long id = idList.get(i);
                    ResultItem resultItem = manager.getItem(id);
                    for (TreeItem treeItem : treeItems)
                        if (contentProvider.matchesPath(getPath(treeItem), id, resultItem))
                            resultIdList.add(id);
                }
                return resultIdList;
            }
        });
    }

    public void copySelectionToClipboard() {
        CsvWriter writer = new CsvWriter('\t');
        writer.addField("Name");
        writer.addField("Value");
        writer.endRecord();

        for (TreeItem treeItem : getSelection()) {
            Node node = (Node)treeItem.getData();
            writer.addField(node.getColumnText(0));
            writer.addField(node.getColumnText(1));
            writer.endRecord();
        }

        Clipboard clipboard = new Clipboard(getDisplay());
        clipboard.setContents(new Object[] {writer.toString()}, new Transfer[] {TextTransfer.getInstance()});
        clipboard.dispose();
    }

    public void setSelectionByID(long id) {
        if (idList.indexOf(id) != -1) {
            ResultItem resultItem = manager.getItem(id);
            // TODO:
        }
        else
            setSelection(new TreeItem[0]);
    }

    public void addDataListener(IDataListener listener) {
        if (listeners == null)
            listeners = new ListenerList();
        listeners.add(listener);
    }

    public void removeDataListener(IDataListener listener) {
        if (listeners != null)
            listeners.remove(listener);
    }

    public void refresh() {
        removeAll();
        clearAll(true);
        setItemCount(contentProvider.getChildNodes(new ArrayList<Node>()).size());
    }

    /**
     * Override the ban on subclassing of Tree, after having read the doc of
     * checkSubclass(). In this class we only build upon the public interface
     * of Tree, so there can be no unwanted side effects. We prefer subclassing
     * to delegating all 1,000,000 Tree methods to an internal Tree instance.
     */
    @Override
    protected void checkSubclass() {
    }

    protected void fireContentChangedEvent() {
        if (listeners != null) {
            for (Object listener : new ArrayList<Object>(Arrays.asList(this.listeners.getListeners())))
                ((IDataListener)listener).contentChanged(this);
        }
    }

    protected TreeColumn addColumn(String text, int width) {
        TreeColumn column = new TreeColumn(this, SWT.NONE);
        column.setText(text);
        column.setWidth(width);
        return column;
    }

    protected void fillTreeItem(TreeItem item, int index) {
        if (manager == null)
            return;

        List<Node> path = getPath(item.getParentItem());
        Node node = contentProvider.getChildNodes(path).get(index);
        path.add(0, node);

        item.setText(0, node.getColumnText(0));
        item.setText(1, node.getColumnText(1));
        item.setData(node);
        item.setImage(node.getImage());
        item.setItemCount(contentProvider.getChildNodes(path).size());
        item.setExpanded(node.isExpandedByDefault());
    }

    private List<Node> getPath(TreeItem treeItem) {
        List<Node> path = new ArrayList<Node>();

        while (treeItem instanceof TreeItem) {
            path.add((Node)treeItem.getData());
            treeItem = treeItem.getParentItem();
        }

        return path;
    }
}
