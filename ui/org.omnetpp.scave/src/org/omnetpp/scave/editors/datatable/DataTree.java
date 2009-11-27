package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.util.CollectionUtils;
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model2.ExperimentPayload;
import org.omnetpp.scave.model2.MeasurementPayload;
import org.omnetpp.scave.model2.ReplicationPayload;

public class DataTree extends Tree implements IDataControl {
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    protected ListenerList listeners;

    protected ResultFileManager manager;

    protected IDList idList;
    
    protected IVirtualTreeContentProvider<Object> contentProvider;

    public DataTree(Composite parent, int style) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        setHeaderVisible(true);
        setLinesVisible(true);
        setMenu(contextMenuManager.createContextMenu(this));
        addColumn("Name", 400);
        addColumn("Value", 200);
        
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

    public ResultFileManager getResultFileManager() {
        return manager;
    }

    public void setResultFileManager(ResultFileManager manager) {
        this.manager = manager;
    }

    public IDList getIDList() {
        return idList;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
        contentProvider = new ResultFileManagerTreeContentProvider(manager, idList);
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
        return (IDList)ResultFileManager.callWithReadLock(manager, new Callable<IDList>() {
            public IDList call() throws Exception {
                IDList resultIdList = new IDList();
                TreeItem[] treeItems = getSelection();
        
                for (int i = 0; i < idList.size(); i++) {
                    long id = idList.get(i);
                    for (TreeItem treeItem : treeItems)
                        if (((ResultFileManagerTreeContentProvider)contentProvider).matchesPath(getPath(treeItem), id))
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
            Object node = treeItem.getData();
            writer.addField(contentProvider.getColumnText(node, 0));
            writer.addField(contentProvider.getColumnText(node, 1));
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
        clearAll(true);
        setItemCount(contentProvider.getChildNodes(null).size());
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

        List<Object> path = getPath(item.getParentItem());
        Object node = contentProvider.getChildNodes(path).get(index);
        path.add(0, node);

        item.setText(0, contentProvider.getColumnText(node, 0));
        item.setText(1, contentProvider.getColumnText(node, 1));
        item.setData(node);
        item.setImage(contentProvider.getImage(node));
        item.setItemCount(contentProvider.getChildNodes(path).size());
    }

    private List<Object> getPath(TreeItem treeItem) {
        List<Object> path = new ArrayList<Object>();
        
        while (treeItem instanceof TreeItem) {
            path.add(treeItem.getData());
            treeItem = treeItem.getParentItem();
        }

        return path;
    }
}

interface IVirtualTreeContentProvider<T> {
    public List<? extends T> getChildNodes(List<? extends T> path);

    public Image getImage(Object node);
    
    public String getColumnText(Object node, int index);
}

class ResultFileManagerTreeContentProvider implements IVirtualTreeContentProvider<Object> {
    protected ResultFileManager manager;

    protected IDList idList;
    
    public ResultFileManagerTreeContentProvider(ResultFileManager manager, IDList idList) {
        this.manager = manager;
        this.idList = idList;
    }

    public List<? extends Object> getChildNodes(final List<? extends Object> path) {
        final Object firstNode = path == null || path.size() == 0 ? null : path.get(0);
        final int lastIndex = path == null ? -1 : path.size() - 1;

        return (List<? extends Object>)ResultFileManager.callWithReadLock(manager, new Callable<List<? extends Object>>() {
            public List<? extends Object> call() throws Exception {
                Set<Object> set = new HashSet<Object>();

                if (firstNode instanceof Long) {
                    ResultItem resultItem = (ResultItem)manager.getItem((Long)firstNode);
                    set.add(new NameValuePayload("Module name", String.valueOf(resultItem.getModuleName())));
                    set.add(new NameValuePayload("Type", resultItem.getType().toString().replaceAll("TYPE_", "").toLowerCase()));

                    if (resultItem instanceof ScalarResult) {
                        ScalarResult scalar = (ScalarResult)resultItem;
                        set.add(new NameValuePayload("Value", String.valueOf(scalar.getValue())));
                    }
                    else if (resultItem instanceof VectorResult) {
                        VectorResult vector = (VectorResult)resultItem;
                        set.add(new NameValuePayload("Count", String.valueOf(vector.getCount())));
                        set.add(new NameValuePayload("Min", String.valueOf(vector.getMin())));
                        set.add(new NameValuePayload("Max", String.valueOf(vector.getMax())));
                        set.add(new NameValuePayload("Mean", String.valueOf(vector.getMean())));
                        set.add(new NameValuePayload("StdDev", String.valueOf(vector.getStddev())));
                        set.add(new NameValuePayload("Variance", String.valueOf(vector.getVariance())));
                        set.add(new NameValuePayload("Start event number", String.valueOf(vector.getStartEventNum())));
                        set.add(new NameValuePayload("End event number", String.valueOf(vector.getEndEventNum())));
                        set.add(new NameValuePayload("Start time", String.valueOf(vector.getStartTime())));
                        set.add(new NameValuePayload("End time", String.valueOf(vector.getEndTime())));
                    }
                    else if (resultItem instanceof HistogramResult) {
                        HistogramResult histogram = (HistogramResult)resultItem;
                        set.add(new NameValuePayload("Count", String.valueOf(histogram.getCount())));
                        set.add(new NameValuePayload("Min", String.valueOf(histogram.getMin())));
                        set.add(new NameValuePayload("Max", String.valueOf(histogram.getMax())));
                        set.add(new NameValuePayload("Mean", String.valueOf(histogram.getMean())));
                        set.add(new NameValuePayload("StdDev", String.valueOf(histogram.getStddev())));
                        set.add(new NameValuePayload("Variance", String.valueOf(histogram.getVariance())));
                    }
                }
                else {
                    for (int i = 0; i < idList.size(); i++) {
                        long id = idList.get(i);
                        ResultItem resultItem = manager.getItem(id);
                        Run run = resultItem.getFileRun().getRun();
                        if (matchesPath(path, id)) {
                            if (firstNode == null)
                                set.add(new ExperimentPayload(run.getAttribute(RunAttribute.EXPERIMENT)));
                            else if (firstNode instanceof ExperimentPayload)
                                set.add(new MeasurementPayload(run.getAttribute(RunAttribute.MEASUREMENT)));
                            else if (firstNode instanceof MeasurementPayload)
                                set.add(new ReplicationPayload(run.getAttribute(RunAttribute.REPLICATION)));
                            else if (firstNode instanceof ReplicationPayload) {
                                String moduleName = resultItem.getModuleName();
                                int index = moduleName.indexOf('.');
                                set.add(index == -1 ? moduleName : moduleName.substring(0, index));
                            }
                            else if (firstNode instanceof String) {
                                String moduleName = resultItem.getModuleName();
                                String modulePrefix = lastIndex < 3 ? null : StringUtils.join(CollectionUtils.toReversed(path.subList(0, lastIndex - 2)), ".");
                                if (moduleName.equals(modulePrefix))
                                    set.add(id);
                                else if (moduleName.startsWith(modulePrefix)) {
                                    int index = modulePrefix.length() + 1;
                                    int nextIndex = moduleName.indexOf('.', index);
                                    set.add(nextIndex == -1 ? moduleName.substring(index) : moduleName.substring(index, nextIndex));
                                }
                            }
                        }
                    }
                }

                List<? extends Object> result = CollectionUtils.toSorted(new ArrayList<Object>(set), new Comparator<Object>() {
                    @SuppressWarnings("unchecked")
                    public int compare(Object o1, Object o2) {
                        if (o1 instanceof Long)
                            return manager.getItem((Long)o1).getName().compareTo(manager.getItem((Long)o2).getName());
                        else
                            return ((Comparable)o1).compareTo(o2);
                    }
                });

                return result;
            }
        });
    }
    
    public Image getImage(Object node) {
        if (node instanceof Long) {
            ResultItem resultItem = (ResultItem)manager.getItem((Long)node);
            if (resultItem instanceof ScalarResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWSCALARS);
            else if (resultItem instanceof VectorResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWVECTORS);
            else if (resultItem instanceof HistogramResult)
                return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS);
            else
                return ImageFactory.getIconImage(ImageFactory.DEFAULT);
        }
        else if (node instanceof String)
            return ImageFactory.getIconImage(ImageFactory.MODEL_IMAGE_SIMPLEMODULE);
        else if (node instanceof NameValuePayload)
            return ImageFactory.getIconImage(ImageFactory.TOOLBAR_IMAGE_PROPERTIES);
        else
            return ImageFactory.getIconImage(ImageFactory.MODEL_IMAGE_FOLDER);
    }

    public String getColumnText(Object node, int index) {
        if (node instanceof ExperimentPayload) {
            ExperimentPayload payload = (ExperimentPayload)node;
            return index == 0 ? payload.name + " (experiment)" : "";
        }
        else if (node instanceof MeasurementPayload) {
            MeasurementPayload payload = (MeasurementPayload)node;
            return index == 0 ? StringUtils.defaultIfEmpty(payload.name, "default")  + " (measurement)" : "";
        }
        else if (node instanceof ReplicationPayload) {
            ReplicationPayload payload = (ReplicationPayload)node;
            return index == 0 ? payload.name + " (replication)" : "";
        }
        else if (node instanceof String) {
            String payload = (String)node;
            return index == 0 ? payload : "";
        }
        else if (node instanceof Long) {
            ResultItem resultItem = (ResultItem)manager.getItem((Long)node);
            String value = "";

            if (resultItem instanceof ScalarResult) {
                ScalarResult scalar = (ScalarResult)resultItem;
                value = String.valueOf(scalar.getValue());
            }
            else if (resultItem instanceof VectorResult) {
                VectorResult vector = (VectorResult)resultItem;
                value = String.valueOf(vector.getMean()) + " (" + String.valueOf(vector.getCount()) + ")";
            }
            else if (resultItem instanceof HistogramResult) {
                HistogramResult histogram = (HistogramResult)resultItem;
                value = String.valueOf(histogram.getMean()) + " (" + String.valueOf(histogram.getCount()) + ")";
            }

            return index == 0 ? resultItem.getName() + " (" + resultItem.getClass().getSimpleName().replaceAll("Result", "").toLowerCase() + ")" : value;
        }
        else if (node instanceof NameValuePayload) {
            NameValuePayload payload = (NameValuePayload)node;
            return index == 0 ? payload.name : payload.value;
        }
        else
            throw new IllegalArgumentException();
    }
    
    protected boolean matchesPath(List<? extends Object> path, long id) {
        int lastIndex = path == null ? -1 : path.size() - 1;
        ResultItem resultItem = manager.getItem(id);
        Run run = resultItem.getFileRun().getRun();
        ExperimentPayload experimentPayload = lastIndex < 0 ? null : (ExperimentPayload)path.get(lastIndex);
        MeasurementPayload measurementPayload = lastIndex < 1 ? null : (MeasurementPayload)path.get(lastIndex - 1);
        ReplicationPayload replicationPayload = lastIndex < 2 ? null : (ReplicationPayload)path.get(lastIndex - 2);
        Object firstElement = path == null || path.size() == 0 ? null : path.get(0);
        String modulePrefix = lastIndex < 3 ? null : StringUtils.join(CollectionUtils.toReversed(path.subList(firstElement instanceof Long ? 1 : 0, lastIndex - 2)), ".");
        
        return
            (experimentPayload == null || experimentPayload.name.equals(run.getAttribute(RunAttribute.EXPERIMENT))) &&
            (measurementPayload == null || measurementPayload.name.equals(run.getAttribute(RunAttribute.MEASUREMENT))) && 
            (replicationPayload == null || replicationPayload.name.equals(run.getAttribute(RunAttribute.REPLICATION))) &&
            (modulePrefix == null || resultItem.getModuleName().startsWith(modulePrefix)) &&
            (!(firstElement instanceof Long) || (Long)firstElement == id); 
    }
    
    protected static class NameValuePayload implements Comparable<NameValuePayload> {
        public String name;
        public String value;

        public NameValuePayload(String name, String value) {
            this.name = name;
            this.value = value;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((name == null) ? 0 : name.hashCode());
            result = prime * result + ((value == null) ? 0 : value.hashCode());
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            NameValuePayload other = (NameValuePayload) obj;
            if (name == null) {
                if (other.name != null)
                    return false;
            }
            else if (!name.equals(other.name))
                return false;
            if (value == null) {
                if (other.value != null)
                    return false;
            }
            else if (!value.equals(other.value))
                return false;
            return true;
        }

        public int compareTo(NameValuePayload other) {
            return name.compareTo(other.name);
        }
    }
}
