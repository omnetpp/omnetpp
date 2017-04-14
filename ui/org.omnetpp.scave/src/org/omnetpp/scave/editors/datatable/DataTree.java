package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.preference.IPreferenceStore;
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
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.CustomTreeLevelsAction;
import org.omnetpp.scave.actions.FlatModuleTreeAction;
import org.omnetpp.scave.actions.PredefinedLevelsAction;
import org.omnetpp.scave.editors.datatable.ResultFileManagerTreeContentProvider.Node;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * This is a class that Levy didn't have time to document :)
 *
 * @author Levy
 */
@SuppressWarnings("unchecked")
public class DataTree extends Tree implements IDataControl {
    protected static final String DATA_TREE_LEVELS = "DataTree.Levels";
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");
    protected ListenerList listeners;
    protected ResultFileManagerEx manager;
    protected IDList idList;
    protected ResultFileManagerTreeContentProvider contentProvider;
    protected IPreferenceStore preferenceStore = ScavePlugin.getDefault().getPreferenceStore();
    protected DelayedJob refreshJob = new DelayedJob(200) {
        public void run() {
            refresh();
        }
    };
    protected IResultFilesChangeListener resultFilesChangeListener = new IResultFilesChangeListener() {
        public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
            refreshJob.restartTimer();
        }
    };

    public DataTree(Composite parent, int style) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        setHeaderVisible(true);
        setLinesVisible(true);
        setMenu(contextMenuManager.createContextMenu(this));
        addColumn("Name", 400);
        addColumn("Value", 200);
        contentProvider = new ResultFileManagerTreeContentProvider();
        loadPreferences();

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

    @Override
    public void dispose() {
        refreshJob.cancel();
        super.dispose();
        if (!manager.isDisposed())
            manager.removeChangeListener(resultFilesChangeListener);
    }

    public ResultFileManagerTreeContentProvider getContentProvider() {
        return contentProvider;
    }

    public void setLevels(Class[] levels) {
        IDList idList = getSelectedIDs();
        contentProvider.setLevels(levels);
        savePreferences();
        refresh();
        setSelectedIDs(idList);
    }

    public ResultFileManagerEx getResultFileManager() {
        return manager;
    }

    public void setResultFileManager(ResultFileManagerEx newManager) {
        if (manager != null && !manager.isDisposed())
            manager.removeChangeListener(resultFilesChangeListener);
        manager = newManager;
        contentProvider.setResultFileManager(newManager);
        if (newManager != null)
            newManager.addChangeListener(resultFilesChangeListener);
    }

    public IDList getIDList() {
        return idList;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
        contentProvider.setIDList(idList);
        refreshJob.restartTimer();
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
                for (TreeItem treeItem : treeItems) {
                    Node node = (Node)treeItem.getData();
                    if (node != null && node.ids != null)
                        for (long id : node.ids)
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

    public void setSelectedID(long id) {
        if (idList.indexOf(id) != -1) {
            TreeItem treeItem = getTreeItem(new long[] {id});
            if (treeItem != null)
                setSelection(treeItem);
        }
        else
            setSelection(new TreeItem[0]);
    }

    public void setSelectedIDs(IDList idList) {
        ArrayList<TreeItem> treeItems = new ArrayList<TreeItem>();
        List<Long> ids = Arrays.asList(idList.toArray());
        long begin = System.currentTimeMillis();
        // find the topmost tree items that cover the whole idList set, but not more
        while (ids.size() > 0) {
            TreeItem treeItem = getTreeItem(ArrayUtils.toPrimitive(ids.toArray(new Long[0])));
            if (treeItem == null)
                break;
            else {
                treeItems.add(treeItem);
                showItem(treeItem);
                Node node = (Node)treeItem.getData();
                if (node != null)
                    ids = (ArrayList<Long>)CollectionUtils.subtract(ids, Arrays.asList(ArrayUtils.toObject(node.ids)));
            }
            // don't spend to much time on it, because the gui becomes unresponsive and it's not worth it
            if (System.currentTimeMillis() - begin > 5000)
                break;
        }
        setSelection(treeItems.toArray(new TreeItem[0]));
    }

    protected TreeItem getTreeItem(long[] ids) {
        for (TreeItem treeItem : getItems()) {
            TreeItem foundTreeItem = getTreeItem(treeItem, ids);
            if (foundTreeItem != null)
                return foundTreeItem;
        }
        return null;
    }

    protected TreeItem getTreeItem(TreeItem treeItem, long[] ids) {
        // items must be queried first, because getData does not fill up the item
        TreeItem[] childTreeItems = treeItem.getItems();
        Node node = (Node)treeItem.getData();
        if (node != null && node.ids == null)
            return null;
        else if (node != null && node.ids != null) {
            boolean subset = true;
            for (long id : node.ids) {
                if (ArrayUtils.indexOf(ids, id) == -1) {
                    subset = false;
                    break;
                }
            }
            if (subset)
                return treeItem;
        }
        for (TreeItem childTreeItem : childTreeItems) {
            TreeItem foundTreeItem = getTreeItem(childTreeItem, ids);
            if (foundTreeItem != null)
                return foundTreeItem;
        }
        return null;
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
        refreshJob.cancel();
        removeAll();
        clearAll(true);
        setItemCount(contentProvider.getChildNodes(new ArrayList<Node>()).length);
    }

    public void contributeToContextMenu(IMenuManager menuManager) {
        final ActionContributionItem item = new ActionContributionItem(new FlatModuleTreeAction("Flat Module Tree", Action.AS_CHECK_BOX, this));
        menuManager.add(item);
        menuManager.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                item.update();
            }
        });
        IMenuManager subMenuManager = new MenuManager("Tree Levels");
        subMenuManager.add(new PredefinedLevelsAction("Experiment / Measurement / Replication", this, ResultFileManagerTreeContentProvider.LEVELS1));
        subMenuManager.add(new PredefinedLevelsAction("Experiment + Measurement + Replication", this, ResultFileManagerTreeContentProvider.LEVELS2));
        subMenuManager.add(new PredefinedLevelsAction("Config / Run Number", this, ResultFileManagerTreeContentProvider.LEVELS3));
        subMenuManager.add(new PredefinedLevelsAction("Config + Run Number", this, ResultFileManagerTreeContentProvider.LEVELS4));
        subMenuManager.add(new PredefinedLevelsAction("File / Run", this, ResultFileManagerTreeContentProvider.LEVELS5));
        subMenuManager.add(new PredefinedLevelsAction("Run", this, ResultFileManagerTreeContentProvider.LEVELS6));
        subMenuManager.add(new Separator());
        subMenuManager.add(new CustomTreeLevelsAction(this));
        subMenuManager.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                for (IContributionItem item : manager.getItems())
                    item.update();
            }
        });
        menuManager.add(subMenuManager);
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
        Node node = contentProvider.getChildNodes(path)[index];
        path.add(0, node);

        setRedraw(false);
        item.setText(0, node.getColumnText(0));
        item.setText(1, node.getColumnText(1));
        item.setData(node);
        item.setImage(node.getImage());
        item.setItemCount(contentProvider.getChildNodes(path).length);
        item.setExpanded(node.isExpandedByDefault());
        setRedraw(true);
    }

    private List<Node> getPath(TreeItem treeItem) {
        List<Node> path = new ArrayList<Node>();

        while (treeItem instanceof TreeItem) {
            path.add((Node)treeItem.getData());
            treeItem = treeItem.getParentItem();
        }

        return path;
    }

    protected void loadPreferences() {
        String[] levelClassNames = preferenceStore.getString(DATA_TREE_LEVELS).split(",");
        if (levelClassNames != null) {
            Class[] levels = new Class[levelClassNames.length];
            for (int i = 0; i < levelClassNames.length; i++) {
                try {
                    levels[i] = Class.forName(levelClassNames[i]);
                }
                catch (ClassNotFoundException e) {
                    return;
                }
            }
            contentProvider.setLevels(levels);
        }
    }

    protected void savePreferences() {
        StringBuffer levelClassNames = new StringBuffer();
        Class<? extends Node>[] levels = contentProvider.getLevels();
        for (int i = 0; i < levels.length; i++) {
            Class level = levels[i];
            if (i != 0)
                levelClassNames.append(',');
            levelClassNames.append(level.getName());
        }
        preferenceStore.setValue(DATA_TREE_LEVELS, levelClassNames.toString());
    }
}
