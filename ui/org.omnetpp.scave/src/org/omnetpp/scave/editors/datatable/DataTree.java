package org.omnetpp.scave.editors.datatable;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog;
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.CustomTreeLevelsAction;
import org.omnetpp.scave.actions.FlatModuleTreeAction;
import org.omnetpp.scave.actions.PredefinedLevelsAction;
import org.omnetpp.scave.editors.datatable.DataTreeContentProvider.Node;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IDListBuffer;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * Tree control for the Browse Data page.
 *
 * @author Levy
 */
//TODO setting numeric precision collapses open tree branches!
@SuppressWarnings({"rawtypes", "unchecked"})
public class DataTree extends Tree implements IDataControl {
    protected static final String DATA_TREE_LEVELS = "DataTree.Levels";
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");
    protected ListenerList<IDataListener> listeners;
    protected ResultFileManagerEx manager;
    protected IDList idList;
    protected DataTreeContentProvider contentProvider;
    protected IPreferenceStore preferenceStore = ScavePlugin.getDefault().getPreferenceStore();
    private FlatModuleTreeAction flatModuleTreeAction;

    public DataTree(Composite parent, int style) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        setHeaderVisible(true);
        setLinesVisible(true);
        setMenu(contextMenuManager.createContextMenu(this));
        addColumn("Name", 400);
        addColumn("Value", 200);
        contentProvider = new DataTreeContentProvider();
        flatModuleTreeAction = new FlatModuleTreeAction(this);
        loadPreferences();

        addListener(SWT.SetData, new Listener() {
            public void handleEvent(final Event e) {
                ResultFileManager.runWithReadLock(manager, () -> {
                    TreeItem item = (TreeItem)e.item;
                    int index = item.getParentItem() == null ? indexOf(item) : item.getParentItem().indexOf(item);
                    fillTreeItem(item, index);
                });
            }
        });
    }

    public DataTreeContentProvider getContentProvider() {
        return contentProvider;
    }

    public void setLevels(Class<? extends Node>[] levels) {
        // note: selection will be lost, because restoring it using setSelectedIDs(idList) would be very slow
        contentProvider.setLevels(levels);
        savePreferences();
        refresh();
    }

    public FlatModuleTreeAction getFlatModuleTreeAction() {
        return flatModuleTreeAction;
    }

    public ResultFileManagerEx getResultFileManager() {
        return manager;
    }

    public void setResultFileManager(ResultFileManagerEx newManager) {
        manager = newManager;
        contentProvider.setResultFileManager(newManager);
    }

    public void setNumericPrecision(int prec) {
        if (prec != getNumericPrecision()) {
            contentProvider.setNumericPrecision(prec);
            refresh();
        }
    }

    public int getNumericPrecision() {
        return contentProvider.getNumericPrecision();
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
        IDList idList = getSelectedIDs();
        if (idList.size() != 1)
            return null;
        return manager.getItem(idList.get(0));
    }

    public ResultItem[] getSelectedItems() {
        IDList idList = getSelectedIDs();
        ArrayList<ResultItem> resultItems = new ArrayList<ResultItem>();

        for (int i = 0; i < idList.size(); i++)
            resultItems.add(manager.getItem(idList.get(i)));

        return resultItems.toArray(new ResultItem[0]);
    }

    public IDList getSelectedIDs() {
        return ResultFileManager.callWithReadLock(manager, () -> {
            IDListBuffer ids = new IDListBuffer();
            TreeItem[] treeItems = getSelection();
            for (TreeItem treeItem : treeItems) {
                Node node = (Node)treeItem.getData();
                if (node != null && node.ids != null)
                    ids.append(node.ids);
            }
            return ids.toIDList();
        });
    }

    public void copySelectionToClipboard() {
        if (manager == null)
            return;

        try {
            TimeTriggeredProgressMonitorDialog dialog = new TimeTriggeredProgressMonitorDialog(getShell(), 1000);
            dialog.run(false, true, (monitor) -> doCopySelectionToClipboard(monitor));
        }
        catch (InterruptedException e) {
            // void
        }
        catch (InvocationTargetException e) {
            Throwable ee = e.getCause();
            MessageDialog.openError(getShell(), "Error", "Copying failed: " + ee.getMessage());
            ScavePlugin.logError("Copy to clipboard failed", ee);
        }
    }

    public void doCopySelectionToClipboard(IProgressMonitor monitor) throws InterruptedException {
        CsvWriter writer = new CsvWriter('\t');
        writer.addField("Name");
        writer.addField("Value");
        writer.endRecord();

        TreeItem[] selection = getSelection();
        int batchSize = 100_000;
        monitor.beginTask("Copying", (selection.length + batchSize - 1) / batchSize);

        int count = 0;
        for (TreeItem treeItem : selection) {
            Node node = (Node)treeItem.getData();
            if (node == null)
                continue;
            writer.addField(node.getColumnText(0));
            writer.addField(node.getColumnText(1));
            writer.endRecord();

            if (++count % batchSize == 0) {
                // update UI
                monitor.worked(1);
                while (Display.getCurrent().readAndDispatch());
                if (monitor.isCanceled())
                    throw new InterruptedException();
            }
        }

        Clipboard clipboard = new Clipboard(getDisplay());
        clipboard.setContents(new Object[] {writer.toString()}, new Transfer[] {TextTransfer.getInstance()});
        clipboard.dispose();
    }

    public void setSelectedID(long id) {
        if (idList.indexOf(id) != -1) {
            TreeItem treeItem = getTreeItem(new IDList(id));
            if (treeItem != null)
                setSelection(treeItem);
        }
        else
            setSelection(new TreeItem[0]);
    }

    public void setSelectedIDs(IDList ids) {
        //
        // NOTE: THIS FUNCTION HAS VERY POOR RUNTIME PERFORMANCE -- DO *NOT* USE IT IF POSSIBLE!
        //
        int timeLimitMillis = 1000;
        ArrayList<TreeItem> treeItems = new ArrayList<TreeItem>();
        long begin = System.currentTimeMillis();
        // find the topmost tree items that cover the whole idList set, but not more
        while (ids.size() > 0) {
            TreeItem treeItem = getTreeItem(ids);
            if (treeItem == null)
                break;
            else {
                treeItems.add(treeItem);
                showItem(treeItem);
                Node node = (Node)treeItem.getData();
                if (node != null)
                    ids = ids.subtract(node.ids);
            }
            // don't spend to much time on it, because the gui becomes unresponsive and it's not worth it
            if (System.currentTimeMillis() - begin > timeLimitMillis)
                break;
        }
        setSelection(treeItems.toArray(new TreeItem[0]));
    }

    protected TreeItem getTreeItem(IDList ids) {
        for (TreeItem treeItem : getItems()) {
            TreeItem foundTreeItem = getTreeItem(treeItem, ids);
            if (foundTreeItem != null)
                return foundTreeItem;
        }
        return null;
    }

    protected TreeItem getTreeItem(TreeItem treeItem, IDList ids) {
        // items must be queried first, because getData does not fill up the item
        TreeItem[] childTreeItems = treeItem.getItems();
        Node node = (Node)treeItem.getData();
        if (node != null && node.ids == null)
            return null;
        else if (node != null && node.ids != null) {
            if (node.ids.isSubsetOf(ids))
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
            listeners = new ListenerList<>();
        listeners.add(listener);
    }

    public void removeDataListener(IDataListener listener) {
        if (listeners != null)
            listeners.remove(listener);
    }

    public void refresh() {
            Debug.time("DataTree.refresh()/removeAll", 100, () ->removeAll());
            Debug.time("DataTree.refresh()/clearAll", 100, () ->clearAll(true));
            int length = Debug.timed("DataTree.refresh()/getChildNodes", 100, () -> contentProvider.getChildNodes(new ArrayList<Node>()).length);
            Debug.time("DataTree.refresh()/setItemCount", 100, ()->setItemCount(length));
    }

    public void contributeToContextMenu(IMenuManager menuManager) {
        final ActionContributionItem item = new ActionContributionItem(flatModuleTreeAction);
        menuManager.add(item);
        menuManager.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                item.update();
            }
        });

        IMenuManager subMenuManager = new MenuManager("Tree Levels");
        contributeTreeLevelsActionsTo(subMenuManager);
        menuManager.add(subMenuManager);
    }

    public void contributeTreeLevelsActionsTo(IMenuManager subMenuManager) {
        subMenuManager.add(new PredefinedLevelsAction("Experiment / Measurement / Replication", this, DataTreeContentProvider.LEVELS1));
        subMenuManager.add(new PredefinedLevelsAction("Experiment + Measurement + Replication", this, DataTreeContentProvider.LEVELS2));
        subMenuManager.add(new PredefinedLevelsAction("Config / Run Number", this, DataTreeContentProvider.LEVELS3));
        subMenuManager.add(new PredefinedLevelsAction("Config + Run Number", this, DataTreeContentProvider.LEVELS4));
        subMenuManager.add(new PredefinedLevelsAction("File / Run", this, DataTreeContentProvider.LEVELS5));
        subMenuManager.add(new PredefinedLevelsAction("Run", this, DataTreeContentProvider.LEVELS6));
        subMenuManager.add(new Separator());
        subMenuManager.add(new CustomTreeLevelsAction(this));
        subMenuManager.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                for (IContributionItem item : manager.getItems())
                    item.update();
            }
        });
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
        if (listeners != null)
            for (IDataListener listener : listeners)
                listener.contentChanged(this);
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

        return path; // actually, reverse path (root comes last)
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
