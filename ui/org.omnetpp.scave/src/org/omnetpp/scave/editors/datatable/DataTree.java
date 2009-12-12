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
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
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
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model2.ExperimentMeasurementReplicationPayload;
import org.omnetpp.scave.model2.ExperimentPayload;
import org.omnetpp.scave.model2.MeasurementPayload;
import org.omnetpp.scave.model2.ReplicationPayload;

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
    protected IVirtualTreeContentProvider<Object> contentProvider;

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

    public IVirtualTreeContentProvider<Object> getContentProvider() {
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

    public IAction createFlattenedModuleTreeAction() {
        return new FlattenedModuleTreeAction();
    }
    
    public IAction createFlattenedLogicalTreeAction() {
        return new FlattenedLogicalTreeAction();
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
        item.setExpanded(contentProvider.isExpandedByDefault(node));
    }

    private List<Object> getPath(TreeItem treeItem) {
        List<Object> path = new ArrayList<Object>();

        while (treeItem instanceof TreeItem) {
            path.add(treeItem.getData());
            treeItem = treeItem.getParentItem();
        }

        return path;
    }

    class FlattenedModuleTreeAction extends Action {
        public FlattenedModuleTreeAction() {
            super("Flattened Module Tree", IAction.AS_CHECK_BOX);
        }

        @Override
        public void run() {
            if (contentProvider instanceof ResultFileManagerTreeContentProvider) {
                ResultFileManagerTreeContentProvider resultFileManagerTreeContentProvider = (ResultFileManagerTreeContentProvider)contentProvider;
                resultFileManagerTreeContentProvider.showFlatModuleTree = !resultFileManagerTreeContentProvider.showFlatModuleTree;
                refresh();
            }
        }
    }

    class FlattenedLogicalTreeAction extends Action {
        public FlattenedLogicalTreeAction() {
            super("Flattened Logical Tree", IAction.AS_CHECK_BOX);
        }

        @Override
        public void run() {
            if (contentProvider instanceof ResultFileManagerTreeContentProvider) {
                ResultFileManagerTreeContentProvider resultFileManagerTreeContentProvider = (ResultFileManagerTreeContentProvider)contentProvider;
                resultFileManagerTreeContentProvider.showFlatExperimentMeasurementReplicationTree = !resultFileManagerTreeContentProvider.showFlatExperimentMeasurementReplicationTree;
                refresh();
           }
        }
    }
}

interface IVirtualTreeContentProvider<T> {
    public void setResultFileManager(ResultFileManager manager);

    public void setIDList(IDList idList);

    public List<? extends T> getChildNodes(List<? extends T> path);

    public boolean isExpandedByDefault(Object node);

    public Image getImage(Object node);

    public String getColumnText(Object node, int index);
}

class ResultFileManagerTreeContentProvider implements IVirtualTreeContentProvider<Object> {
    protected ResultFileManager manager;

    protected IDList idList;

    public boolean showFlatModuleTree;

    public boolean showFlatExperimentMeasurementReplicationTree;

    public ResultFileManagerTreeContentProvider() {
    }

    public void setResultFileManager(ResultFileManager manager) {
        this.manager = manager;
    }

    public void setIDList(IDList idList) {
        this.idList = idList;
    }

    public List<? extends Object> getChildNodes(final List<? extends Object> path) {
        final Object firstNode = path == null || path.size() == 0 ? null : path.get(0);

        // TODO: shall we rather use an enum and a generic tree node? we could get rid of the path stuff and use a simpler lazy tree interface
        return ResultFileManager.callWithReadLock(manager, new Callable<List<? extends Object>>() {
            public List<? extends Object> call() throws Exception {
                Set<Object> set = new HashSet<Object>();

                if (firstNode instanceof Long) { // result item level
                    ResultItem resultItem = manager.getItem((Long)firstNode);
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
                        set.add(new BinsPayload((Long)firstNode, histogram.getBins().size()));
                    }
                    else
                        throw new IllegalArgumentException();
                }
                else if (firstNode instanceof BinsPayload) { // bins level for histograms
                    ResultItem resultItem = manager.getItem(((BinsPayload)firstNode).id);
                    HistogramResult histogram = (HistogramResult)resultItem;
                    DoubleVector bins = histogram.getBins();
                    DoubleVector values = histogram.getValues();
                    List<Object> list = new ArrayList<Object>();
                    for (int i = 0; i < bins.size(); i++) {
                        double bin1 = bins.get(i);
                        double bin2 = i < bins.size() - 1 ? bins.get(i + 1) : Double.POSITIVE_INFINITY;
                        double value = values.get(i);
                        double valueFloor = Math.floor(value);
                        list.add(new NameValuePayload(String.valueOf(bin1) + " .. " + String.valueOf(bin2), value == valueFloor ? String.valueOf((long)valueFloor) : String.valueOf(value)));
                    }
                    return list;
                }
                else if (!(firstNode instanceof NameValuePayload)) { // non leaf levels
                    for (int i = 0; i < idList.size(); i++) {
                        long id = idList.get(i);
                        ResultItem resultItem = manager.getItem(id);
                        Run run = resultItem.getFileRun().getRun();
                        if (matchesPath(path, id)) {
                            if (firstNode == null) { // root node
                                if (showFlatExperimentMeasurementReplicationTree)
                                    set.add(new ExperimentMeasurementReplicationPayload(run.getAttribute(RunAttribute.EXPERIMENT), run.getAttribute(RunAttribute.MEASUREMENT), run.getAttribute(RunAttribute.REPLICATION)));
                                else
                                    set.add(new ExperimentPayload(run.getAttribute(RunAttribute.EXPERIMENT)));
                            }
                            else if (firstNode instanceof ExperimentPayload) // logical level
                                set.add(new MeasurementPayload(run.getAttribute(RunAttribute.MEASUREMENT)));
                            else if (firstNode instanceof MeasurementPayload) // logical level
                                set.add(new ReplicationPayload(run.getAttribute(RunAttribute.REPLICATION)));
                            else if (firstNode instanceof ReplicationPayload || firstNode instanceof ExperimentMeasurementReplicationPayload) { // logical level
                                String moduleName = resultItem.getModuleName();

                                if (showFlatModuleTree)
                                    set.add(moduleName);
                                else {
                                    int index = moduleName.indexOf('.');
                                    set.add(index == -1 || ".".equals(moduleName) ? moduleName : moduleName.substring(0, index));
                                }
                            }
                            else if (firstNode instanceof String) { // module tree levels
                                String moduleName = resultItem.getModuleName();
                                String modulePrefix = getModulePrefix(path);
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

    public boolean isExpandedByDefault(Object node) {
        return node instanceof ExperimentPayload || node instanceof MeasurementPayload;
    }

    public Image getImage(Object node) {
        if (node instanceof Long) {
            ResultItem resultItem = manager.getItem((Long)node);
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
        else if (node instanceof ExperimentMeasurementReplicationPayload) {
            ExperimentMeasurementReplicationPayload payload = (ExperimentMeasurementReplicationPayload)node;
            return index == 0 ? payload.experiment + (StringUtils.isEmpty(payload.measurement) ? "" : " : " + payload.measurement) + " : " + payload.replication : "";
        }
        else if (node instanceof String) {
            String payload = (String)node;
            return index == 0 ? payload : "";
        }
        else if (node instanceof Long) {
            ResultItem resultItem = manager.getItem((Long)node);
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

    protected String getModulePrefix(final List<? extends Object> path) {
        StringBuffer modulePrefix = new StringBuffer();
        for (int j =  path.size() - 1; j >= 0; j--) {
            Object element = path.get(j);
            if (element instanceof String) {
                if (modulePrefix.length() == 0)
                    modulePrefix.append(element);
                else {
                    modulePrefix.append('.');
                    modulePrefix.append(element);
                }
            }
        }
        return modulePrefix.toString();
    }

    protected boolean matchesPath(List<? extends Object> path, long id) {
        if (path == null || path.size() == 0)
            return true;
        else {
            int lastIndex = path.size() - 1;
            ResultItem resultItem = manager.getItem(id);
            Run run = resultItem.getFileRun().getRun();
            Object lastElement = path.get(lastIndex);
            Object firstElement = path.get(0);
            String modulePrefix = getModulePrefix(path);

            if (lastElement instanceof ExperimentPayload) {
                ExperimentPayload experimentPayload = lastIndex < 0 ? null : (ExperimentPayload)lastElement;
                MeasurementPayload measurementPayload = lastIndex < 1 ? null : (MeasurementPayload)path.get(lastIndex - 1);
                ReplicationPayload replicationPayload = lastIndex < 2 ? null : (ReplicationPayload)path.get(lastIndex - 2);

                return
                    (experimentPayload == null || experimentPayload.name.equals(run.getAttribute(RunAttribute.EXPERIMENT))) &&
                    (measurementPayload == null || measurementPayload.name.equals(run.getAttribute(RunAttribute.MEASUREMENT))) &&
                    (replicationPayload == null || replicationPayload.name.equals(run.getAttribute(RunAttribute.REPLICATION))) &&
                    (modulePrefix == null || resultItem.getModuleName().startsWith(modulePrefix)) &&
                    (!(firstElement instanceof Long) || (Long)firstElement == id);
            }
            else if (lastElement instanceof ExperimentMeasurementReplicationPayload) {
                ExperimentMeasurementReplicationPayload payload = (ExperimentMeasurementReplicationPayload)lastElement;

                return
                    (payload == null || payload.experiment.equals(run.getAttribute(RunAttribute.EXPERIMENT))) &&
                    (payload == null || payload.measurement.equals(run.getAttribute(RunAttribute.MEASUREMENT))) &&
                    (payload == null || payload.replication.equals(run.getAttribute(RunAttribute.REPLICATION))) &&
                    (modulePrefix == null || resultItem.getModuleName().startsWith(modulePrefix)) &&
                    (!(firstElement instanceof Long) || (Long)firstElement == id);
            }
            else
                throw new IllegalArgumentException("Unknown path");
        }
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

    protected static class BinsPayload extends NameValuePayload {
        public long id;

        public BinsPayload(long id, long count) {
            super("Bins", String.valueOf(count));
            this.id = id;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + (int) (id ^ (id >>> 32));
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
            BinsPayload other = (BinsPayload) obj;
            if (id != other.id)
                return false;
            return true;
        }
    }
}
