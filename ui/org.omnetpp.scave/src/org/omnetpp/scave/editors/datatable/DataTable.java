/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.RunAttribute.CONFIGNAME;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;
import static org.omnetpp.scave.engine.RunAttribute.RUNNUMBER;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.ScaveUtil;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.ResultType;

/**
 * This is a preconfigured VIRTUAL table, which displays a list of
 * output vectors, output scalars or histograms, given an IDList and
 * the corresponding ResultFileManager as input. It is optimized
 * for very large amounts of data. (Display time is constant,
 * so it can be used with even millions of table lines without
 * performance degradation).
 *
 * The user is responsible to keep contents up-to-date in case
 * ResultFileManager or IDList contents change. Refreshing can be
 * done either by a call to setIDList(), or by refresh().
 *
 * @author andras
 */
//TODO use s/histogram/statistics/ in the whole file
public class DataTable extends Table implements IDataControl {

    /**
     * Keys used in getData(),setData()
     */
    public static final String COLUMN_KEY = "DataTable.Column";
    public static final String ITEM_KEY = "DataTable.Item";

    private static final String NA = "-"; // "not applicable"

    static class Column {

        private String text;
        private String fieldName;
        private int defaultWidth;
        private boolean defaultVisible;
        private boolean rightAligned;

        public Column(String text, String fieldName, int defaultWidth, boolean defaultVisible, boolean rightAligned) {
            this.text = text;
            this.fieldName = fieldName;
            this.defaultWidth = defaultWidth;
            this.defaultVisible = defaultVisible;
            this.rightAligned = rightAligned;
        }

        @Override
        public Column clone() {
            return new Column(this.text, this.fieldName, this.defaultWidth, this.defaultVisible, this.rightAligned);
        }

        @Override
        public boolean equals(Object other) {
            return other instanceof Column && this.text.equals(((Column)other).text);
        }

        @Override
        public int hashCode() {
            return text.hashCode();
        }
    }

    private static final Column
        COL_DIRECTORY = new Column("Folder", null, 60, false, false),
        COL_FILE = new Column("File", FILE, 120, false, false),
        COL_CONFIG = new Column("Config", CONFIGNAME, 120, false, false),
        COL_RUNNUMBER = new Column("Run number", RUNNUMBER, 60, false, false),
        COL_RUN_ID = new Column("RunId", RUN, 100, false, false),
        COL_EXPERIMENT = new Column("Experiment", EXPERIMENT, 120, true, false),
        COL_MEASUREMENT = new Column("Measurement", MEASUREMENT, 160, true, false),
        COL_REPLICATION = new Column("Replication", REPLICATION, 60, true, false),
        COL_MODULE = new Column("Module", MODULE, 160, true, false),
        COL_NAME = new Column("Name", NAME, 120, true, false),
        COL_VALUE = new Column("Value", null, 120, true, true),
        COL_KIND = new Column("Kind", null, 40, true, false),
        COL_COUNT = new Column("Count", null, 80, true, true),
        COL_SUMWEIGHTS = new Column("SumWeights", null, 120, true, true),
        COL_MEAN = new Column("Mean", null, 120, true, true),
        COL_STDDEV = new Column("StdDev", null, 120, true, true),
        COL_VARIANCE = new Column("Variance", null, 120, true, true),
        COL_MIN = new Column("Min", null, 120, false, true),
        COL_MAX = new Column("Max", null, 120, false, true),
        COL_NUMBINS = new Column("#Bins", null, 40, true, true),
        COL_HISTOGRAMRANGE = new Column("Hist. Range", null, 120, true, true),
        COL_VECTOR_ID = new Column("Vector id", null, 40, false, true),
        COL_MIN_TIME = new Column("Min time", null, 120, false, true),
        COL_MAX_TIME = new Column("Max time", null, 120, false, true);

    private static final Column[] allScalarColumns = new Column[] {
        COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
        COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
        COL_MODULE, COL_NAME,
        COL_VALUE
    };

    private static final Column[] allVectorColumns = new Column[] {
        COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
        COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
        COL_MODULE, COL_NAME,
        COL_VECTOR_ID,
        COL_COUNT, COL_MEAN, COL_STDDEV, COL_VARIANCE, COL_MIN, COL_MAX, COL_MIN_TIME, COL_MAX_TIME
    };

    private static final Column[] allHistogramColumns = new Column[] {
        COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
        COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
        COL_MODULE, COL_NAME, COL_KIND,
        COL_COUNT, COL_SUMWEIGHTS, COL_MEAN, COL_STDDEV, COL_VARIANCE, COL_MIN, COL_MAX,
        COL_NUMBINS, COL_HISTOGRAMRANGE
    };

    private ResultType type;
    private ResultFileManagerEx manager;
    private IDList idList;
    private int numericPrecision = 6;
    private ListenerList listeners;
    private int minColumnWidth = 5; // for usability
    private List<Column> visibleColumns; // list of visible columns, this list will be saved and restored
    private IPreferenceStore preferences = ScavePlugin.getDefault().getPreferenceStore();

    // holds actions for the context menu for this data table
    private MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    private static final ResultItem[] NULL_SELECTION = new ResultItem[0];

    private TableItem selectedItem;
    private TableColumn selectedColumn;

    public DataTable(Composite parent, int style, ResultType type) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        Assert.isTrue(type==ResultType.SCALAR || type==ResultType.VECTOR || type==ResultType.HISTOGRAM);
        this.type = type;
        setHeaderVisible(true);
        setLinesVisible(true);
        initDefaultState();
        initColumns();

        addListener(SWT.SetData, new Listener() {
            public void handleEvent(final Event e) {
                ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                    public Object call() {
                        TableItem item = (TableItem)e.item;
                        int lineNumber = indexOf(item);
                        fillTableLine(item, lineNumber);
                        return null;
                    }
                });
            }
        });

        setMenu(contextMenuManager.createContextMenu(this));

        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseDown(MouseEvent event) {
                handleMouseDown(event);
            }
        });
    }

    /**
     * Override the ban on subclassing of Table, after having read the doc of
     * checkSubclass(). In this class we only build upon the public interface
     * of Table, so there can be no unwanted side effects. We prefer subclassing
     * to delegating all 1,000,000 Table methods to an internal Table instance.
     */
    @Override
    protected void checkSubclass() {
    }

    public ResultType getType() {
        return type;
    }

    public void setResultFileManager(ResultFileManagerEx manager) {
        this.manager = manager;
    }

    public ResultFileManagerEx getResultFileManager() {
        return manager;
    }

    public void setNumericPrecision(int numericPrecision) {
        if (this.numericPrecision != numericPrecision) {
            this.numericPrecision = numericPrecision;
            refresh();
        }
    }

    public int getNumericPrecision() {
        return numericPrecision;
    }

    public void setIDList(IDList idlist) {
        this.idList = idlist;
        restoreSortOrder();
        refresh();
        fireContentChangedEvent();
    }

    public IDList getIDList() {
        return idList;
    }

    public IMenuManager getContextMenuManager() {
        return contextMenuManager;
    }

    protected Column[] getAllColumns() {
        switch (type) {
        case SCALAR:     return allScalarColumns;
        case VECTOR:     return allVectorColumns;
        case HISTOGRAM:  return allHistogramColumns;
        default: return null;
        }
    }

    public String[] getAllColumnNames() {
        Column[] columns = getAllColumns();
        String[] columnNames = new String[columns.length];
        for (int i = 0; i < columns.length; ++i)
            columnNames[i] = columns[i].text;
        return columnNames;
    }

    public String[] getVisibleColumnNames() {
        String[] columnNames = new String[visibleColumns.size()];
        for (int i = 0; i < visibleColumns.size(); ++i)
            columnNames[i] = visibleColumns.get(i).text;
        return columnNames;
    }

    public void setVisibleColumns(String[] columnTexts) {
        for (Column column : getAllColumns()) {
            TableColumn tableColumn = getTableColumn(column);
            boolean currentlyVisible = tableColumn != null;
            boolean requestedVisible = ArrayUtils.indexOf(columnTexts, column.text) != -1;

            if (requestedVisible && !currentlyVisible)
                addColumn(column);
            else if (!requestedVisible && currentlyVisible) {
                visibleColumns.remove(column);
                tableColumn.dispose();
            }
        }

        int position = 0;
        int[] columnOrder = new int[getColumnCount()];
        for (Column column : getAllColumns())
            if (visibleColumns.indexOf(column) != -1)
                columnOrder[position++] = getTableColumnIndex(column);
        columnOrder[position] = getColumnCount()-1; // the last, blank column

        setColumnOrder(columnOrder);

        saveState();
        refresh();
    }

    public IDList getSelectedIDs() {
        int[] selectionIndices = getSelectionIndices();
        return idList.getSubsetByIndices(selectionIndices);
    }

    public ResultItem[] getSelectedItems() {
        if (manager == null)
            return NULL_SELECTION;

        int[] selectionIndices = getSelectionIndices();
        ResultItem[] items = new ResultItem[selectionIndices.length];

        for (int i = 0; i < items.length; ++i)
            items[i] = manager.getItem(idList.get(selectionIndices[i]));

        return items;
    }

    public void refresh() {
        setItemCount(idList.size());
        clearAll();
    }

    protected void initColumns() {
        // add a last, blank column, otherwise the right edge of the last column sticks to the table's right border which is often inconvenient
        TableColumn blankColumn = new TableColumn(this, SWT.NONE);
        blankColumn.setWidth(minColumnWidth);

        visibleColumns = new ArrayList<>();
        loadState();
    }

    public TableColumn[] getColumnsExceptLastBlank() {
        TableColumn[] columns = super.getColumns();
        Assert.isTrue(columns[columns.length-1].getData(COLUMN_KEY) == null);
        return Arrays.copyOfRange(columns, 0, columns.length-1);
    }

    protected TableColumn getTableColumn(Column column) {
        for (TableColumn tableColumn : getColumnsExceptLastBlank())
            if (tableColumn.getData(COLUMN_KEY).equals(column))
                return tableColumn;
        return null;
    }

    protected int getTableColumnIndex(Column column) {
        TableColumn[] columns = getColumnsExceptLastBlank();
        for (int index = 0; index < columns.length; index++) {
            TableColumn tableColumn = columns[index];
            if (tableColumn.getData(COLUMN_KEY).equals(column))
                return index;
        }
        return -1;
    }

    protected TableColumn addColumn(Column newColumn) {
        visibleColumns.add(newColumn);
        TableColumn tableColumn = new TableColumn(this, newColumn.rightAligned ? SWT.RIGHT : SWT.NONE, getColumnsExceptLastBlank().length);
        tableColumn.setText(newColumn.text);
        tableColumn.setWidth(newColumn.defaultWidth);
        tableColumn.setData(COLUMN_KEY, newColumn);
        tableColumn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                TableColumn tableColumn = (TableColumn)e.widget;
                if (!tableColumn.isDisposed()) {
                    Column column = (Column)tableColumn.getData(COLUMN_KEY);
                    int sortDirection = (getSortColumn() == tableColumn && getSortDirection() == SWT.UP ? SWT.DOWN : SWT.UP);
                    setSortColumn(tableColumn);
                    setSortDirection(sortDirection);
                    sortBy(column, sortDirection);
                    refresh();
                    fireContentChangedEvent();
                }
            }
        });
        tableColumn.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent e) {
                ensureMinimumColumnWidths(); // this is an attempt to enforce a minimum column width; simply calling setWidth() on the resized column does not really take effect
                saveState();
            }
        });

        return tableColumn;
    }

    private void ensureMinimumColumnWidths() {
        for (TableColumn tableColumn : getColumns())
            if (tableColumn.getWidth() < minColumnWidth)
                tableColumn.setWidth(minColumnWidth);  // this will either take effect in the rendering or not... (GTK3)
    }

    private void restoreSortOrder() {
        TableColumn sortColumn = getSortColumn();
        int sortDirection = getSortDirection();
        if (sortColumn != null && sortDirection != SWT.NONE) {
            Column column = (Column)sortColumn.getData(COLUMN_KEY);
            if (column != null)
                sortBy(column, sortDirection);
        }
    }

    private void sortBy(Column column, int direction) {
        if (manager == null)
            return;

        boolean ascending = direction == SWT.UP;
        if (COL_DIRECTORY.equals(column))
            idList.sortByDirectory(manager, ascending);
        else if (COL_FILE.equals(column))
            idList.sortByFileName(manager, ascending);
        else if (COL_CONFIG.equals(column))
            idList.sortByRunAttribute(manager, CONFIGNAME, ascending);
        else if (COL_RUNNUMBER.equals(column))
            idList.sortByRunAttribute(manager, RUNNUMBER, ascending);
        else if (COL_RUN_ID.equals(column))
            idList.sortByRun(manager, ascending);
        else if (COL_MODULE.equals(column))
            idList.sortByModule(manager, ascending);
        else if (COL_NAME.equals(column))
            idList.sortByName(manager, ascending);
        else if (COL_VALUE.equals(column))
            idList.sortScalarsByValue(manager, ascending);
        else if (COL_VECTOR_ID.equals(column))
            idList.sortVectorsByVectorId(manager, ascending);
        else if (COL_KIND.equals(column))
            ; //TODO
        else if (COL_COUNT.equals(column)) {
            if (idList.areAllHistograms())
                idList.sortHistogramsByLength(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByLength(manager, ascending);
        }
        else if (COL_SUMWEIGHTS.equals(column))
            ; //TODO
        else if (COL_MEAN.equals(column)) {
            if (idList.areAllHistograms()) //TODO
                idList.sortHistogramsByMean(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByMean(manager, ascending);
        }
        else if (COL_STDDEV.equals(column)) {
            if (idList.areAllHistograms()) //TODO
                idList.sortHistogramsByStdDev(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByStdDev(manager, ascending);
        }
        else if (COL_MIN.equals(column)) {
            if (idList.areAllHistograms()) //TODO
                idList.sortHistogramsByMin(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByMin(manager, ascending);
        }
        else if (COL_MAX.equals(column)) {
            if (idList.areAllHistograms()) //TODO
                idList.sortHistogramsByMax(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByMax(manager, ascending);
        }
        else if (COL_VARIANCE.equals(column)) {
            if (idList.areAllHistograms()) //TODO
                idList.sortHistogramsByVariance(manager, ascending);
            else if (idList.areAllVectors())
                idList.sortVectorsByVariance(manager, ascending);
        }
        else if (COL_NUMBINS.equals(column))
            ; //TODO
        else if (COL_HISTOGRAMRANGE.equals(column))
            ; //TODO
        else if (COL_EXPERIMENT.equals(column))
            idList.sortByRunAttribute(manager, EXPERIMENT, ascending);
        else if (COL_MEASUREMENT.equals(column))
            idList.sortByRunAttribute(manager, MEASUREMENT, ascending);
        else if (COL_REPLICATION.equals(column))
            idList.sortByRunAttribute(manager, REPLICATION, ascending);
        else if (COL_MIN_TIME.equals(column))
            idList.sortVectorsByStartTime(manager, ascending);
        else if (COL_MAX_TIME.equals(column))
            idList.sortVectorsByEndTime(manager, ascending);
    }

    protected void fillTableLine(TableItem item, int lineNumber) {
        if (manager == null)
            return;

        long id = idList.get(lineNumber);
        item.setData(ITEM_KEY, id);

        for (int i = 0; i < visibleColumns.size(); ++i) {
            Column column = visibleColumns.get(i);
            String value = getCellValue(lineNumber, column);
            item.setText(i, value);
        }
    }

    protected void toCSV(CsvWriter writer, int lineNumber) {
        if (manager == null)
            return;

        for (int i = 0; i < visibleColumns.size(); ++i) {
            Column column = visibleColumns.get(i);
            writer.addField(getCellValue(lineNumber, column));
        }

        writer.endRecord();
    }

    protected String getCellValue(int row, Column column) {
        if (manager == null)
            return "";

        try {
            // Note: code very similar to ResultItemPropertySource -- make them common?
            long id = idList.get(row);
            ResultItem result = manager.getItem(id);

            if (COL_DIRECTORY.equals(column))
                return result.getFileRun().getFile().getDirectory();
            else if (COL_FILE.equals(column)) {
                String fileName = result.getFileRun().getFile().getFileName();
                return fileName;
            }
            else if (COL_CONFIG.equals(column)) {
                String config = result.getFileRun().getRun().getAttribute(CONFIGNAME);
                return config != null ? config : NA;
            }
            else if (COL_RUNNUMBER.equals(column)) {
                String runNumber = result.getFileRun().getRun().getAttribute(RUNNUMBER);
                return runNumber != null ? runNumber : NA;
            }
            else if (COL_RUN_ID.equals(column))
                return result.getFileRun().getRun().getRunName();
            else if (COL_MODULE.equals(column))
                return result.getModuleName();
            else if (COL_NAME.equals(column))
                return result.getName();
            else if (COL_EXPERIMENT.equals(column)) {
                String experiment = result.getFileRun().getRun().getAttribute(EXPERIMENT);
                return experiment != null ? experiment : NA;
            }
            else if (COL_MEASUREMENT.equals(column)) {
                String measurement = result.getFileRun().getRun().getAttribute(MEASUREMENT);
                return measurement != null ? measurement : NA;
            }
            else if (COL_REPLICATION.equals(column)) {
                String replication = result.getFileRun().getRun().getAttribute(REPLICATION);
                return replication != null ? replication : NA;
            }
            else if (type == ResultType.SCALAR) {
                ScalarResult scalar = (ScalarResult)result;
                if (COL_VALUE.equals(column))
                    return formatNumber(scalar.getValue());
            }
            else if (type == ResultType.VECTOR) {
                VectorResult vector = (VectorResult)result;
                if (COL_VECTOR_ID.equals(column)) {
                    return String.valueOf(vector.getVectorId());
                }
                else if (COL_COUNT.equals(column)) {
                    long count = vector.getStatistics().getCount();
                    return count >= 0 ? String.valueOf(count) : NA;
                }
                else if (COL_MEAN.equals(column)) {
                    double mean = vector.getStatistics().getMean();
                    return Double.isNaN(mean) ? NA : formatNumber(mean);
                }
                else if (COL_STDDEV.equals(column)) {
                    double stddev = vector.getStatistics().getStddev();
                    return Double.isNaN(stddev) ? NA : formatNumber(stddev);
                }
                else if (COL_VARIANCE.equals(column)) {
                    double variance = vector.getStatistics().getVariance();
                    return Double.isNaN(variance) ? NA : formatNumber(variance);
                }
                else if (COL_MIN.equals(column)) {
                    double min = vector.getStatistics().getMin();
                    return Double.isNaN(min) ? NA : formatNumber(min);
                }
                else if (COL_MAX.equals(column)) {
                    double max = vector.getStatistics().getMax();
                    return Double.isNaN(max) ? NA : formatNumber(max);
                }
                else if (COL_MIN_TIME.equals(column)) {
                    BigDecimal minTime = vector.getStartTime();
                    return minTime == null || minTime.isNaN() ? NA : formatNumber(minTime);
                }
                else if (COL_MAX_TIME.equals(column)) {
                    BigDecimal maxTime = vector.getEndTime();
                    return maxTime == null || maxTime.isNaN() ? NA : formatNumber(maxTime);
                }
            }
            else if (type == ResultType.HISTOGRAM) {
                StatisticsResult stats = (StatisticsResult)result;
                if (COL_KIND.equals(column)) {
                    boolean isHistogram = result instanceof HistogramResult;
                    boolean isWeighted = stats.getStatistics().isWeighted();
                    return isHistogram ? (isWeighted ? "wh" : "h") : (isWeighted ? "ws" : "s");
                }
                else if (COL_COUNT.equals(column)) {
                    long count = stats.getStatistics().getCount();
                    return count >= 0 ? String.valueOf(count) : NA;
                }
                else if (COL_SUMWEIGHTS.equals(column)) {
                    if (!stats.getStatistics().isWeighted())
                        return NA;
                    double sumWeights = stats.getStatistics().getSumWeights();
                    return sumWeights >= 0 ? formatNumber(sumWeights) : NA;
                }
                else if (COL_MEAN.equals(column)) {
                    double mean = stats.getStatistics().getMean();
                    return Double.isNaN(mean) ? NA : formatNumber(mean);
                }
                else if (COL_STDDEV.equals(column)) {
                    double stddev = stats.getStatistics().getStddev();
                    return Double.isNaN(stddev) ? NA : formatNumber(stddev);
                }
                else if (COL_VARIANCE.equals(column)) {
                    double variance = stats.getStatistics().getVariance();
                    return Double.isNaN(variance) ? NA : formatNumber(variance);
                }
                else if (COL_MIN.equals(column)) {
                    double min = stats.getStatistics().getMin();
                    return Double.isNaN(min) ? NA : formatNumber(min);
                }
                else if (COL_MAX.equals(column)) {
                    double max = stats.getStatistics().getMax();
                    return Double.isNaN(max) ? NA : formatNumber(max);
                }
                else if (COL_NUMBINS.equals(column)) {
                    if (result instanceof HistogramResult)
                        return String.valueOf(((HistogramResult)result).getHistogram().getNumBins()-2); //TODO fix this brain damage of underflow and overflow "cells" !!!
                    else
                        return NA;
                }
                else if (COL_HISTOGRAMRANGE.equals(column)) {
                    if (result instanceof HistogramResult) {
                        Histogram bins = ((HistogramResult)result).getHistogram();
                        if (bins.getNumBins() <= 2) //TODO fix this brain damage of underflow and overflow "cells" !!!
                            return NA;
                        double lo = bins.getBinLowerBound(1); //TODO (0)!
                        double up = bins.getBinUpperBound(bins.getNumBins()-2); //TODO ()-1!
                        return formatNumber(lo) + ".." + formatNumber(up);
                    }
                    else
                        return NA;
                }
            }
        }
        catch (RuntimeException e) {
            // stale ID?
            return "";
        }

        return "";
    }

    protected String formatNumber(double d) {
        return ScaveUtil.formatNumber(d, getNumericPrecision());
    }

    protected String formatNumber(BigDecimal d) {
        return ScaveUtil.formatNumber(d, getNumericPrecision());
    }

    public void copySelectionToClipboard() {
        CsvWriter writer = new CsvWriter('\t');
        // add header
        for (Column column : visibleColumns)
            writer.addField(column.text);
        writer.endRecord();
        // add selected lines
        int[] selection = getSelectionIndices();
        for (int i = 0; i < selection.length; ++i)
            toCSV(writer, selection[i]);

        Clipboard clipboard = new Clipboard(getDisplay());
        clipboard.setContents(new Object[] {writer.toString()}, new Transfer[] {TextTransfer.getInstance()});
        clipboard.dispose();

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

    protected void fireContentChangedEvent() {
        if (listeners != null) {
            for (Object listener : new ArrayList<>(Arrays.asList(this.listeners.getListeners())))
                ((IDataListener)listener).contentChanged(this);
        }
    }

    /*
     * Save/load state
     */

    protected String getPreferenceStoreKey(Column column, String field) {
        return "DataTable." + type + "." + column.text + "." + field;
    }

    protected void initDefaultState() {
        if (preferences != null) {
            for (Column column : getAllColumns()) {
                preferences.setDefault(getPreferenceStoreKey(column, "visible"), column.defaultVisible);
                preferences.setDefault(getPreferenceStoreKey(column, "width"), column.defaultWidth);
            }
        }
    }

    protected void loadState() {
        if (preferences != null) {
            visibleColumns.clear();
            for (Column column : getAllColumns()) {
                boolean visible = preferences.getBoolean(getPreferenceStoreKey(column, "visible"));
                if (visible) {
                    Column clone = column.clone();
                    clone.defaultWidth = preferences.getInt(getPreferenceStoreKey(column, "width"));
                    addColumn(clone);
                }
            }
        }
    }

    protected void saveState() {
        if (preferences != null) {
            for (Column column : getAllColumns()) {
                boolean visible = visibleColumns.indexOf(column) >= 0;
                preferences.setValue(getPreferenceStoreKey(column, "visible"), visible);
                if (visible)
                    preferences.setValue(getPreferenceStoreKey(column, "width"), getTableColumn(column).getWidth());
            }
        }
    }

    /*
     * Select cells.
     */
    void handleMouseDown(MouseEvent event) {
        if (isDisposed() || !isVisible()) return;
        Point pt = new Point(event.x, event.y);
        int lineWidth = getLinesVisible() ? getGridLineWidth() : 0;
        TableItem item = getItem(pt);
        if ((getStyle() & SWT.FULL_SELECTION) != 0) {
            if (item == null) return;
        }
        else {
            int start = item != null ? indexOf(item) : getTopIndex();
            int end = getItemCount();
            Rectangle clientRect = getClientArea();
            for (int i = start; i < end; i++) {
                TableItem nextItem = getItem(i);
                Rectangle rect = nextItem.getBounds(0);
                if (pt.y >= rect.y && pt.y < rect.y + rect.height + lineWidth) {
                    item = nextItem;
                    break;
                }
                if (rect.y > clientRect.y + clientRect.height)  return;
            }
            if (item == null) return;
        }
        TableColumn newColumn = null;
        int columnCount = getColumnCount();
        if (columnCount > 0) {
            for (int i = 0; i < columnCount; i++) {
                Rectangle rect = item.getBounds(i);
                rect.width += lineWidth;
                rect.height += lineWidth;
                if (rect.contains(pt)) {
                    newColumn = getColumn(i);
                    break;
                }
            }
            if (newColumn == null) {
                newColumn = getColumn(0);
            }
        }
        setSelectedCell(item, newColumn);
    }

    private void setSelectedCell(TableItem item, TableColumn column) {
        selectedItem = item;
        selectedColumn = column;
    }

    public String getSelectedField() {
        if (selectedColumn != null && !selectedColumn.isDisposed()) {
            Column column = (Column)selectedColumn.getData(COLUMN_KEY);
            if (column != null)
                return column.fieldName;
        }
        return null;
    }

    public ResultItem getSelectedItem() {
        if (selectedItem != null && !selectedItem.isDisposed()) {
            long id = (Long)selectedItem.getData(ITEM_KEY);
            if (!manager.isStaleID(id))
                return manager.getItem(id);
        }
        return null;
    }

    public void setSelectedID(long id) {
        int index = idList.indexOf(id);
        if (index != -1)
            setSelection(index);
    }

    public void setSelectedIDs(IDList selectedIDList) {
        ArrayList<Integer> indicesList = new ArrayList<>();
        for (int i = 0; i < selectedIDList.size(); i++) {
            int index = idList.indexOf(selectedIDList.get(i));
            if (index != -1)
                indicesList.add(index);
        }
        int[] indices = new int[indicesList.size()];
        for (int i = 0; i < indices.length; i++)
            indices[i] = indicesList.get(i);
        setSelection(indices);
    }
}
