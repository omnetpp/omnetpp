/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.viewers.StyledString;
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
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.Debug;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.engine.MeasureTextFunctor;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engineext.IMatchableObject;
import org.omnetpp.common.largetable.AbstractLargeTableRowRenderer;
import org.omnetpp.common.largetable.LargeTable;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog2;
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.QuantityFormatterRegistry;
import org.omnetpp.scave.editors.ui.ScaveUtil;
import org.omnetpp.scave.engine.Histogram;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IntVector;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ParameterResult;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Scave;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * This is a preconfigured VIRTUAL table, which displays a list of
 * output vectors, output scalars, parameters, or histograms, given
 * an IDList and the corresponding ResultFileManager as input. It is
 * optimized for very large amounts of data. (Display time is constant,
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
public class DataTable extends LargeTable implements IDataControl {
    private static final boolean isLightTheme = !DisplayUtils.isDarkTheme();
    private static final Color GREYED_OUT_COLOR = isLightTheme ? ColorFactory.GREY60 : ColorFactory.GREY40;
    private static final Color QUANTITY_UNIT_COLOR = isLightTheme ? ColorFactory.BLUE3 : ColorFactory.LIGHT_SKY_BLUE;

    private static final String COLUMNROLE_KEY = "role";

    private static final StyledString NA = new StyledString("-"); // "not applicable"

    private enum ColumnRole {
        COL_DIRECTORY("Folder", null, 60, false, false),
        COL_FILE("File", Scave.FILE, 120, false, false),
        COL_CONFIG("Config", Scave.RUNATTR_PREFIX + Scave.CONFIGNAME, 120, false, false),
        COL_RUNNUMBER("Run number", Scave.RUNATTR_PREFIX + Scave.RUNNUMBER, 60, false, false),
        COL_RUN_ID("RunId", Scave.RUN, 100, false, false),
        COL_EXPERIMENT("Experiment", Scave.RUNATTR_PREFIX + Scave.EXPERIMENT, 120, true, false),
        COL_MEASUREMENT("Measurement", Scave.RUNATTR_PREFIX + Scave.MEASUREMENT, 160, true, false),
        COL_REPLICATION("Replication", Scave.RUNATTR_PREFIX + Scave.REPLICATION, 60, true, false),
        COL_MODULE("Module", Scave.MODULE, 160, true, false),
        COL_NAME("Name", Scave.NAME, 120, true, false),
        COL_PARAM_VALUE("Value", null, 120, true, true),
        COL_SCALAR_VALUE("Value", null, 120, true, true),
        COL_KIND("Kind", null, 40, true, false),
        COL_COUNT("Count", null, 80, true, true),
        COL_SUMWEIGHTS("SumWeights", null, 120, true, true),
        COL_MEAN("Mean", null, 120, true, true),
        COL_STDDEV("StdDev", null, 120, true, true),
        COL_VARIANCE("Variance", null, 120, true, true),
        COL_MIN("Min", null, 120, false, true),
        COL_MAX("Max", null, 120, false, true),
        COL_NUMBINS("#Bins", null, 40, true, true),
        COL_HISTOGRAMRANGE("Hist. Range", null, 120, true, true),
        COL_VECTOR_ID("Vector id", null, 40, false, true),
        COL_MIN_TIME("Min time", null, 120, false, true),
        COL_MAX_TIME("Max time", null, 120, false, true);

        private final String label;
        private final String fieldName;
        private final int defaultWidth;
        private final boolean defaultVisible;
        private final boolean defaultRightAligned;

        ColumnRole(String text, String fieldName, int defaultWidth, boolean defaultVisible, boolean rightAligned) {
            this.label = text;
            this.fieldName = fieldName;
            this.defaultWidth = defaultWidth;
            this.defaultVisible = defaultVisible;
            this.defaultRightAligned = rightAligned;
        }

        public String getLabel() {
            return label;
        }

        public String getFieldName() {
            return fieldName;
        }

        public int getDefaultWidth() {
            return defaultWidth;
        }

        public boolean isDefaultVisible() {
            return defaultVisible;
        }

        public boolean isDefaultRightAligned() {
            return defaultRightAligned;
        }

        public static final ColumnRole[] ALL_SCALAR_COLUMNS = new ColumnRole[] {
                COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
                COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
                COL_MODULE, COL_NAME,
                COL_SCALAR_VALUE
        };

        public static final ColumnRole[] ALL_PARAMETER_COLUMNS = new ColumnRole[] {
                COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
                COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
                COL_MODULE, COL_NAME,
                COL_PARAM_VALUE
        };

        public static final ColumnRole[] ALL_VECTOR_COLUMNS = new ColumnRole[] {
                COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
                COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
                COL_MODULE, COL_NAME,
                COL_VECTOR_ID,
                COL_COUNT, COL_MEAN, COL_STDDEV, COL_VARIANCE, COL_MIN, COL_MAX, COL_MIN_TIME, COL_MAX_TIME
        };

        public static final ColumnRole[] ALL_HISTOGRAM_COLUMNS = new ColumnRole[] {
                COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID,
                COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
                COL_MODULE, COL_NAME, COL_KIND,
                COL_COUNT, COL_SUMWEIGHTS, COL_MEAN, COL_STDDEV, COL_VARIANCE, COL_MIN, COL_MAX,
                COL_NUMBINS, COL_HISTOGRAMRANGE
        };
    };


    private PanelType type;
    private ResultFileManagerEx manager;
    private IDList idList = new IDList();
    private boolean numberFormattingEnabled = true;
    private int numericPrecision = 6;
    private ListenerList<IDataListener> listeners;
    private int minColumnWidth = 5; // for usability
    private List<ColumnRole> visibleColumns = new ArrayList<>(); // list of visible columns, this list will be saved and restored
    private IPreferenceStore preferences = ScavePlugin.getDefault().getPreferenceStore();

    private boolean showNetworkNames = true;
    private boolean colorNetworkNames = true;
    private boolean colorResultSuffixes = true;
    private boolean colorNumberSeparators = true;
    private boolean colorMeasurementUnits = true;

    // holds actions for the context menu for this data table
    private MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    private static final ResultItem[] NULL_SELECTION = new ResultItem[0];

    private TableColumn selectedColumn; // the last column selected by a mouse click

    public DataTable(Composite parent, int style, PanelType type) {
        super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
        Assert.isTrue(type==PanelType.SCALARS || type==PanelType.PARAMETERS || type==PanelType.VECTORS || type==PanelType.HISTOGRAMS);
        this.type = type;
        setLinesVisible(true);

        // add a last, blank column, otherwise the right edge of the last column sticks to the table's right border which is often inconvenient
        TableColumn blankColumn = createColumn(SWT.NONE);
        blankColumn.setWidth(minColumnWidth);

        loadState();

        setRowRenderer(new AbstractLargeTableRowRenderer() {
            @Override
            public String getText(int rowIndex, int columnIndex) {
                return getCellValue(rowIndex, columnIndex, null, -1).getString();
            }

            @Override
            public StyledString getStyledText(int rowIndex, int columnIndex, GC gc, int alignment) {
                // the last, blank column is not included in visibleColumns
                if (columnIndex < visibleColumns.size()) {
                    ColumnRole column = visibleColumns.get(columnIndex);
                    return getCellValue(rowIndex, column, gc, (getColumn(columnIndex).getWidth() - CELL_HORIZONTAL_MARGIN*2));
                }
                else
                    return new StyledString("");
            }

            @Override
            public String getTooltipText(int rowIndex, int columnIndex) {
                return null;
            }

            @Override
            public int getIndentation(int rowIndex, int columnIndex) {
                return 0;
            }

            @Override
            public Image getImage(int rowIndex, int columnIndex) {
                return null;
            }
        });

        setMenu(contextMenuManager.createContextMenu(this));

        getCanvas().addMouseListener(new MouseAdapter() {
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

    public PanelType getType() {
        return type;
    }

    public void setResultFileManager(ResultFileManagerEx manager) {
        this.manager = manager;
    }

    public ResultFileManagerEx getResultFileManager() {
        return manager;
    }

    public void setNumberFormattingEnabled(boolean enabled) {
        if (this.numberFormattingEnabled != enabled) {
            this.numberFormattingEnabled = enabled;
            refresh();
        }
    }

    public boolean getNumberFormattingEnabled() {
        return numberFormattingEnabled;
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

    public boolean getShowNetworkNames() {
        return showNetworkNames;
    }

    public void setShowNetworkNames(boolean showNetworkName) {
        this.showNetworkNames = showNetworkName;
        refresh();
    }

    public boolean getColorNetworkNames() {
        return colorNetworkNames;
    }

    public void setColorNetworkNames(boolean colorNetworkNames) {
        this.colorNetworkNames = colorNetworkNames;
        refresh();
    }

    public boolean getColorResultSuffixes() {
        return colorResultSuffixes;
    }

    public void setColorResultSuffixes(boolean colorResultSuffixes) {
        this.colorResultSuffixes = colorResultSuffixes;
        refresh();
    }

    public boolean getColorNumberSeparators() {
        return colorNumberSeparators;
    }

    public void setColorNumberSeparators(boolean colorNumberSeparators) {
        this.colorNumberSeparators = colorNumberSeparators;
        refresh();
    }

    public boolean getColorMeasurementUnits() {
        return colorMeasurementUnits;
    }

    public void setColorMeasurementUnits(boolean colorMeasurementUnits) {
        this.colorMeasurementUnits = colorMeasurementUnits;
        refresh();
    }

    public void setIDList(IDList newIdList) {
        if (newIdList.equals(this.idList))
            return;

        // save old focus. note: saving the selection is not done because:
        // (1) it's likely not too useful or missed by users, and at the
        // same time (2) difficult due to listener hell
        long focusID = getFocusedID();
        clearSelection();  // not really needed, as setItemCount() does it too

        // set new input
        setItemCount(newIdList.size()); // includes another clearSelection() & firing empty selection change
        this.idList = newIdList;
        restoreSortOrder(); // in-place sorts the IDList

        // try restoring old focus
        setFocusedID(focusID); // note: this indirectly fires selection change, so state should be fully updated at this point

        // refresh display, notify listeners
        refresh();
        fireContentChangedEvent();
    }

    public IDList getIDList() {
        return idList;
    }

    public long getFocusedID() {
        int index = getFocusIndex();
        return (index >= 0 && index < idList.size()) ? idList.get(index) : -1;
    }

    public void setFocusedID(long id) {
        int index = idList.indexOf(id); // index=-1 if not found
        if (index != -1)
            gotoIndex(index);
    }

    public IMenuManager getContextMenuManager() {
        return contextMenuManager;
    }

    protected ColumnRole[] getAllColumns() {
        switch (type) {
        case SCALARS:     return ColumnRole.ALL_SCALAR_COLUMNS;
        case PARAMETERS:  return ColumnRole.ALL_PARAMETER_COLUMNS;
        case VECTORS:     return ColumnRole.ALL_VECTOR_COLUMNS;
        case HISTOGRAMS:  return ColumnRole.ALL_HISTOGRAM_COLUMNS;
        default: return null;
        }
    }

    public String[] getAllColumnNames() {
        ColumnRole[] columns = getAllColumns();
        String[] columnNames = new String[columns.length];
        for (int i = 0; i < columns.length; ++i)
            columnNames[i] = columns[i].label;
        return columnNames;
    }

    public String[] getVisibleColumnNames() {
        String[] columnNames = new String[visibleColumns.size()];
        for (int i = 0; i < visibleColumns.size(); ++i)
            columnNames[i] = visibleColumns.get(i).label;
        return columnNames;
    }

    public void setVisibleColumns(String[] columnTexts) {
        for (TableColumn c : getColumnsExceptLastBlank())
            c.dispose();
        visibleColumns.clear();

        for (ColumnRole column : getAllColumns()) {
            boolean requestedVisible = ArrayUtils.indexOf(columnTexts, column.label) != -1;
            if (requestedVisible)
                addColumn(column);
        }

        int[] columnOrder = new int[getColumnCount()];
        for (int i = 0; i < columnOrder.length; ++i)
            columnOrder[i] = i;

        table.setColumnOrder(columnOrder);

        saveState();
        refresh();

        // Workaround: Without the following, the right ~80% of the table disappears after OKing the
        // "Choose table columns" dialog, because the width of the ScrolledComposite's Composite child
        // becomes too small.
        //UIUtils.dumpLayout(this); // for debugging the layout

        composite.setSize(composite.computeSize(-1, -1));
        composite.layout(true);
    }

    public IDList getSelectedIDs() {
        int[] selectionIndices = getSelectionIndices().toArray();
        if (idList != null)
            return idList.getSubsetByIndices(selectionIndices);
        else
            return null;
    }

    public ResultItem[] getSelectedItems() {
        if (manager == null)
            return NULL_SELECTION;

        int[] selectionIndices = getSelectionIndices().toArray();
        ResultItem[] items = new ResultItem[selectionIndices.length];

        for (int i = 0; i < items.length; ++i)
            items[i] = manager.getItem(idList.get(selectionIndices[i]));

        return items;
    }

    public TableColumn[] getColumnsExceptLastBlank() {
        TableColumn[] columns = super.getColumns();
        Assert.isTrue(columns[columns.length-1].getData(COLUMNROLE_KEY) == null);
        return Arrays.copyOfRange(columns, 0, columns.length-1);
    }

    protected TableColumn getTableColumn(ColumnRole column) {
        for (TableColumn tableColumn : getColumnsExceptLastBlank())
            if (tableColumn.getData(COLUMNROLE_KEY).equals(column))
                return tableColumn;
        return null;
    }

    protected int getTableColumnIndex(ColumnRole column) {
        TableColumn[] columns = getColumnsExceptLastBlank();
        for (int index = 0; index < columns.length; index++) {
            TableColumn tableColumn = columns[index];
            if (tableColumn.getData(COLUMNROLE_KEY).equals(column))
                return index;
        }
        return -1;
    }

    protected TableColumn addColumn(ColumnRole column) {
        visibleColumns.add(column);

        String columnPrefix = getPreferenceKeyPrefix(column);
        int width = getPreference(columnPrefix + "width", column.getDefaultWidth());
        boolean rightAlign = getPreference(columnPrefix + "rightAlign", column.isDefaultRightAligned());

        TableColumn tableColumn = createColumn(rightAlign ? SWT.RIGHT : SWT.NONE, getColumnsExceptLastBlank().length);
        tableColumn.setText(column.getLabel());
        tableColumn.setWidth(width);
        tableColumn.setData(COLUMNROLE_KEY, column);

        tableColumn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                TableColumn tableColumn = (TableColumn)e.widget;
                if (!tableColumn.isDisposed()) {
                    int sortDirection = (getSortColumn() == tableColumn && getSortDirection() == SWT.UP ? SWT.DOWN : SWT.UP);
                    setSorting(tableColumn, sortDirection);
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

    @Override
    protected void fillContextMenu(Menu contextMenu) {
        addAdjustWidthMenuItem(contextMenu);
        super.fillContextMenu(contextMenu); // original context menu
        new MenuItem(contextMenu, SWT.SEPARATOR);
        addChooseTableColumnsMenuItem(contextMenu);
    }

    protected void addChooseTableColumnsMenuItem(Menu contextMenu) {
        MenuItem item = new MenuItem(contextMenu, SWT.NONE);
        final ChooseTableColumnsAction action = new ChooseTableColumnsAction(this);
        item.setText(action.getText());
        item.addSelectionListener(SelectionListener.widgetSelectedAdapter(e -> {
            action.run();
        }));
    }

    protected void addAdjustWidthMenuItem(Menu contextMenu) {
        //
        // NOTE: This would optimally be triggered by a double-click on the column separator in the header.
        // However, that's pretty much impossible to implement, due to the following factors:
        //
        // - Mouse events are NOT delivered to the TableColumn or Table at all
        // - Resize events are delivered to the TableColumn, but it's nearly impossible
        //   to distinguish a double-click from resize-by-dragging
        // - There are no overridable methods on Table/TableColumn to influence the resizing or width computation
        //
        // Two possible but ugly workarounds:
        // - Global event filter hooked on Display for intercepting double-clicks
        // - An invisible row in the table, with tweaked content to force the appropriate width
        //
        // Hence, we stick with the context menu item.
        //

        MenuItem item = new MenuItem(contextMenu, SWT.NONE);
        item.setText("Fit Column Width to Content");
        item.addSelectionListener(SelectionListener.widgetSelectedAdapter(e -> {
            TableColumn column = getLastContextMenuHeaderColumn();
            if (column != null)
                column.setWidth(getAutoColumnWidth(column));
        }));
    }

    protected int getAutoColumnWidth(TableColumn tableColumn) {
        ColumnRole column = (ColumnRole)tableColumn.getData(COLUMNROLE_KEY);
        int width = 0;
        GC gc = new GC(this);
        for (int row = getTopIndex(); row <= getBottomIndex(); row++) {
            String text = getCellValue(row, column, null, -1).getString();
            width = Math.max(width, gc.textExtent(text).x);
        }
        gc.dispose();
        int paddingWidth = 10;
        return width + paddingWidth;
    }

    private void ensureMinimumColumnWidths() {
        for (TableColumn tableColumn : getColumns())
            if (tableColumn.getWidth() < minColumnWidth)
                tableColumn.setWidth(minColumnWidth);  // this will either take effect in the rendering or not... (GTK3)
    }

    private void restoreSortOrder() {
        TableColumn sortColumn = getSortColumn();
        int sortDirection = getSortDirection();
        if (sortColumn != null && sortDirection != SWT.NONE)
            sortRows(sortColumn, sortDirection);
    }

    public void setSorting(TableColumn tableColumn, int sortDirection) {
        setSortColumn(tableColumn);
        setSortDirection(sortDirection);
        sortRows(tableColumn, sortDirection);
        refresh();
        fireContentChangedEvent();
    }

    protected void sortRows(TableColumn sortColumn, int sortDirection) {
        if (manager == null || idList.isEmpty()) // no/empty input
            return;

        ColumnRole column = (ColumnRole)sortColumn.getData(COLUMNROLE_KEY);
        if (column == null) // requested column has no sort key
            return;

        boolean allSelected = getSelectionCount() == idList.size();
        IntVector tmpSelectionIndices = allSelected ? new IntVector() : IntVector.fromArray(getSelectionIndices().toArray()); // optimize the common & expensive case when all items are selected
        long focusID = getFocusedID();

        TimeTriggeredProgressMonitorDialog2.runWithDialog("Sorting", (monitor) -> {
            InterruptedFlag interrupted = TimeTriggeredProgressMonitorDialog2.getActiveInstance().getInterruptedFlag();
            Debug.time("sorting", 1, () -> sortBy(idList, tmpSelectionIndices, column, sortDirection, interrupted)); // sort idList together with tmpSelectionIndices
        });

        if (!allSelected) {
            int[] array = tmpSelectionIndices.toArray();
            Arrays.sort(array);
            setSelectionIndices(array);
        }

        setFocusedID(focusID);
    }

    protected void sortBy(IDList idList, IntVector selectionIndices, ColumnRole column, int direction, InterruptedFlag interrupted) {
        boolean ascending = direction == SWT.UP;
        switch (column) {
        case COL_DIRECTORY:
            idList.sortByDirectory(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_FILE:
            idList.sortByFileName(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_CONFIG:
            idList.sortByRunAttribute(manager, Scave.CONFIGNAME, ascending, selectionIndices, interrupted);
            break;
        case COL_RUNNUMBER:
            idList.sortByRunAttribute(manager, Scave.RUNNUMBER, ascending, selectionIndices, interrupted);
            break;
        case COL_RUN_ID:
            idList.sortByRun(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_MODULE:
            idList.sortByModule(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_NAME:
            idList.sortByName(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_PARAM_VALUE:
            idList.sortParametersByValue(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_SCALAR_VALUE:
            idList.sortScalarsByValue(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_VECTOR_ID:
            idList.sortVectorsByVectorId(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_KIND:
            ; //TODO
            break;
        case COL_COUNT:
            if (idList.areAllStatistics())
                idList.sortStatisticsByCount(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByCount(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_SUMWEIGHTS:
            if (idList.areAllStatistics())
                idList.sortStatisticsBySumWeights(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortStatisticsBySumWeights(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_MEAN:
            if (idList.areAllStatistics())
                idList.sortStatisticsByMean(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByMean(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_STDDEV:
            if (idList.areAllStatistics())
                idList.sortStatisticsByStdDev(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByStdDev(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_MIN:
            if (idList.areAllStatistics())
                idList.sortStatisticsByMin(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByMin(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_MAX:
            if (idList.areAllStatistics())
                idList.sortStatisticsByMax(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByMax(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_VARIANCE:
            if (idList.areAllStatistics())
                idList.sortStatisticsByVariance(manager, ascending, selectionIndices, interrupted);
            else if (idList.areAllVectors())
                idList.sortVectorsByVariance(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_NUMBINS:
            if (idList.areAllHistograms()) //TODO areAllStatistics(), and sorting should not crash
                idList.sortHistogramsByNumBins(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_HISTOGRAMRANGE:
            if (idList.areAllHistograms()) //TODO areAllStatistics(), and sorting should not crash
                idList.sortHistogramsByHistogramRange(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_EXPERIMENT:
            idList.sortByRunAttribute(manager, Scave.EXPERIMENT, ascending, selectionIndices, interrupted);
            break;
        case COL_MEASUREMENT:
            idList.sortByRunAttribute(manager, Scave.MEASUREMENT, ascending, selectionIndices, interrupted);
            break;
        case COL_REPLICATION:
            idList.sortByRunAttribute(manager, Scave.REPLICATION, ascending, selectionIndices, interrupted);
            break;
        case COL_MIN_TIME:
            idList.sortVectorsByStartTime(manager, ascending, selectionIndices, interrupted);
            break;
        case COL_MAX_TIME:
            idList.sortVectorsByEndTime(manager, ascending, selectionIndices, interrupted);
            break;
        }
    }

    protected StyledString getCellValue(int rowIndex, int columnIndex, GC gc, int width) {
        if (columnIndex >= visibleColumns.size())
            return new StyledString("");
        ColumnRole column = visibleColumns.get(columnIndex);
        return getCellValue(rowIndex, column, gc, width);
    }

    protected StyledString getCellValue(int row, ColumnRole column, GC gc, int width) {
        if (manager == null)
            return new StyledString("");

        try {
            // Note: code very similar to ResultItemPropertySource -- make them common?
            long id = idList.get(row);
            ResultItem result = manager.getItem(id);

            String unit = result.getAttribute("unit");
            if (unit.equals(""))
                unit = null;

            switch (column) {
            case COL_DIRECTORY:
                return new StyledString(result.getFile().getDirectory());
            case COL_FILE:
                return new StyledString(result.getFile().getFileName());
            case COL_CONFIG: {
                String config = result.getFileRun().getRun().getAttribute(Scave.CONFIGNAME);
                return config != null ? new StyledString(config) : NA;
            }
            case COL_RUNNUMBER: {
                String runNumber = result.getFileRun().getRun().getAttribute(Scave.RUNNUMBER);
                return runNumber != null ? new StyledString(runNumber) : NA;
            }
            case COL_RUN_ID:
                return new StyledString(result.getFileRun().getRun().getRunName());
            case COL_MODULE: {
                String name = result.getModuleName();
                if (!showNetworkNames) {
                    int index = name.indexOf('.');
                    return new StyledString(name.substring(index+1));
                }
                StyledString styledString = new StyledString(name);
                if (colorNetworkNames) {
                    int index = name.indexOf('.');
                    if (index != -1)
                        setStyleStringColor(styledString, 0, index, GREYED_OUT_COLOR);
                }
                return styledString;
            }
            case COL_NAME: {
                String name = result.getName();
                StyledString styledString = new StyledString(name);
                if (colorResultSuffixes) {
                    int index = name.lastIndexOf(':');
                    if (index != -1)
                        setStyleStringColor(styledString, index, name.length() - index, GREYED_OUT_COLOR);
                }
                return styledString;
            }
            case COL_EXPERIMENT: {
                String experiment = result.getFileRun().getRun().getAttribute(Scave.EXPERIMENT);
                return experiment != null ? new StyledString(experiment) : NA;
            }
            case COL_MEASUREMENT: {
                String measurement = result.getFileRun().getRun().getAttribute(Scave.MEASUREMENT);
                return measurement != null ? new StyledString(measurement) : NA;
            }
            case COL_REPLICATION: {
                String replication = result.getFileRun().getRun().getAttribute(Scave.REPLICATION);
                return replication != null ? new StyledString(replication) : NA;
            }
            case COL_PARAM_VALUE: {
                ParameterResult parameter = (ParameterResult)result;
                return new StyledString(parameter.getValue());
            }
            case COL_SCALAR_VALUE: {
                ScalarResult scalar = (ScalarResult)result;
                return formatNumber(result, column.label, scalar.getValue(), unit, gc, width);
            }
            default: {
                if (type == PanelType.VECTORS) {
                    VectorResult vector = (VectorResult)result;
                    switch (column) {
                    case COL_VECTOR_ID: {
                        return new StyledString(String.valueOf(vector.getVectorId()));
                    }
                    case COL_COUNT: {
                        long count = vector.getStatistics().getCount();
                        return count >= 0 ? formatNumber(result, column.label, count, null, gc, width) : NA;
                    }
                    case COL_MEAN: {
                        double mean = vector.getStatistics().getMean();
                        return Double.isNaN(mean) ? NA : formatNumber(result, column.label, mean, unit, gc, width);
                    }
                    case COL_STDDEV: {
                        double stddev = vector.getStatistics().getStddev();
                        return Double.isNaN(stddev) ? NA : formatNumber(result, column.label, stddev, unit, gc, width);
                    }
                    case COL_VARIANCE: {
                        double variance = vector.getStatistics().getVariance();
                        String unitSquared = unit;
                        if (unit != null)
                            unitSquared = unit + "\u00B2"; // "Superscript Two"
                        return Double.isNaN(variance) ? NA : formatNumber(result, column.label, variance, unitSquared, gc, width);
                    }
                    case COL_MIN: {
                        double min = vector.getStatistics().getMin();
                        return Double.isNaN(min) ? NA : formatNumber(result, column.label, min, unit, gc, width);
                    }
                    case COL_MAX: {
                        double max = vector.getStatistics().getMax();
                        return Double.isNaN(max) ? NA : formatNumber(result, column.label, max, unit, gc, width);
                    }
                    case COL_MIN_TIME: {
                        BigDecimal minTime = vector.getStartTime();
                        return minTime == null || minTime.isNaN() ? NA : formatNumber(result, column.label, minTime, gc, width);
                    }
                    case COL_MAX_TIME: {
                        BigDecimal maxTime = vector.getEndTime();
                        return maxTime == null || maxTime.isNaN() ? NA : formatNumber(result, column.label, maxTime, gc, width);
                    }
                    default: // silence warning
                    }
                }
                else if (type == PanelType.HISTOGRAMS) {
                    StatisticsResult stats = (StatisticsResult)result;
                    switch(column) {
                    case COL_KIND: {
                        boolean isHistogram = result instanceof HistogramResult;
                        boolean isWeighted = stats.getStatistics().isWeighted();
                        return new StyledString(isHistogram ? (isWeighted ? "wh" : "h") : (isWeighted ? "ws" : "s"));
                    }
                    case COL_COUNT: {
                        long count = stats.getStatistics().getCount();
                        return count >= 0 ? formatNumber(result, column.label, count, null, gc, width) : NA;
                    }
                    case COL_SUMWEIGHTS: {
                        if (!stats.getStatistics().isWeighted())
                            return NA;
                        double sumWeights = stats.getStatistics().getSumWeights();
                        return sumWeights >= 0 ? formatNumber(result, column.label, sumWeights, "", gc, width) : NA;
                    }
                    case COL_MEAN: {
                        double mean = stats.getStatistics().getMean();
                        return Double.isNaN(mean) ? NA : formatNumber(result, column.label, mean, unit, gc, width);
                    }
                    case COL_STDDEV: {
                        double stddev = stats.getStatistics().getStddev();
                        return Double.isNaN(stddev) ? NA : formatNumber(result, column.label, stddev, unit, gc, width);
                    }
                    case COL_VARIANCE: {
                        double variance = stats.getStatistics().getVariance();
                        String unitSquared = unit;
                        if (unit != null)
                            unitSquared = unit + "\u00B2"; // "Superscript Two"
                        return Double.isNaN(variance) ? NA : formatNumber(result, column.label, variance, unitSquared, gc, width);
                    }
                    case COL_MIN: {
                        double min = stats.getStatistics().getMin();
                        return Double.isNaN(min) ? NA : formatNumber(result, column.label, min, unit, gc, width);
                    }
                    case COL_MAX: {
                        double max = stats.getStatistics().getMax();
                        return Double.isNaN(max) ? NA : formatNumber(result, column.label, max, unit, gc, width);
                    }
                    case COL_NUMBINS: {
                        if (result instanceof HistogramResult)
                            return new StyledString(String.valueOf(((HistogramResult)result).getHistogram().getNumBins()));
                        else
                            return NA;
                    }
                    case COL_HISTOGRAMRANGE: {
                        if (result instanceof HistogramResult) {
                            Histogram bins = ((HistogramResult)result).getHistogram();
                            if (bins.getNumBins() == 0)
                                return NA;
                            double lo = bins.getBinEdge(0);
                            double up = bins.getBinEdge(bins.getNumBins());
                            return formatNumber(result, column.label, lo, unit, gc, width).append(" .. ").append(formatNumber(result, column.label, up, unit, gc, width));
                        }
                        else
                            return NA;
                    }
                    default: // silence warning
                    }
                }
            }
            }
        }
        catch (RuntimeException e) {
            ScavePlugin.logError(e);
            return new StyledString("<error>", makeColorStyler(ColorFactory.RED));
        }

        return new StyledString("");
    }

    protected StyledString formatNumber(ResultItem resultItem, String column, double d, String unit, GC gc, int width) {
        if (!numberFormattingEnabled) {
            String result = ScaveUtil.formatNumber(d, getNumericPrecision());
            if (!StringUtils.isEmpty(unit))
                result += " " + unit;
            return new StyledString(result);
        }
        else {
            QuantityFormatter quantityFormatter = QuantityFormatterRegistry.getInstance().getQuantityFormatter(new IMatchableObject() {
                @Override
                public String getAsString(String name) {
                    if ("editor".equals(name))
                        return "scave";
                    if ("context".equals(name))
                        return "datatable";
                    else if ("column".equals(name))
                        return column;
                    else if ("unit".equals(name))
                        return unit;
                    else if ("module".equals(name))
                        return resultItem.getModuleName();
                    else if ("run".equals(name))
                        return resultItem.getFileRun().getRun().getRunName();
                    else if ("name".equals(name))
                        return resultItem.getName();
                    else
                        return resultItem.getAttribute(name);
                }

                @Override
                public String getAsString() {
                    return unit;
                }
            });
            QuantityFormatter.Options quantityFormatterOptions = quantityFormatter.getOptions();
            quantityFormatterOptions.setNumAvailablePixels(gc != null ? width : Integer.MAX_VALUE);
            quantityFormatterOptions.setMeasureTextFromJava(new MeasureTextFunctor() {
                @Override
                public int call(String text) {
                    return gc.textExtent(text).x;
                }
            });
            quantityFormatterOptions.setMaxSignificantDigits(Math.min(getNumericPrecision(), quantityFormatterOptions.getNumAccurateDigits()));
            QuantityFormatter.Output output = quantityFormatter.formatQuantity(d, unit);
            int numChars = output.getText().length();
            if (!output.getFitsIntoAvailableSpace()) {
                StyledString styledString = new StyledString("#".repeat(numChars));
                setStyleStringColor(styledString, 0, numChars, GREYED_OUT_COLOR);
                return styledString;
            }
            else {
                StyledString styledString = new StyledString(output.getText());
                if (colorNumberSeparators || colorMeasurementUnits) {
                    String role = output.getRole();
                    for (int i = 0; i < numChars; i++) {
                        char c = role.charAt(i);
                        if (colorNumberSeparators && (c == ',' || c == '~'))
                            setStyleStringColor(styledString, i, 1, GREYED_OUT_COLOR);
                        else if (colorMeasurementUnits && c == 'u')
                            setStyleStringColor(styledString, i, 1, QUANTITY_UNIT_COLOR);
                    }
                }
                return styledString;
            }
        }
    }

    protected void setStyleStringColor(StyledString styledString, int index, int length, Color color) {
        StyledString.Styler styler = makeColorStyler(color);
        styledString.setStyle(index, length, styler);
    }

    protected StyledString.Styler makeColorStyler(Color color) {
        return new StyledString.Styler() {
            @Override
            public void applyStyles(TextStyle textStyle) {
                textStyle.foreground = color;
            }
        };
    }

    protected StyledString formatNumber(ResultItem result, String column, BigDecimal d, GC gc, int width) {
        return new StyledString(ScaveUtil.formatNumber(d, getNumericPrecision()) + " s");
    }

    public void copyRowsToClipboard(IProgressMonitor monitor) throws InterruptedException {
        if (manager == null)
            return;

        ResultFileManagerEx.runWithReadLock(manager, () -> {
            CsvWriter writer = new CsvWriter('\t');

            // add header
            for (ColumnRole column : visibleColumns)
                writer.addField(column.label);
            writer.endRecord();

            // add selected lines
            int[] selection = getSelectionIndices().toArray();
            int batchSize = 100_000;
            monitor.beginTask("Copying", (selection.length + batchSize - 1) / batchSize);

            int count = 0;
            for (int rowIndex : selection) {
                for (ColumnRole column : visibleColumns)
                    writer.addField(getCellValue(rowIndex, column, null, -1).getString());
                writer.endRecord();

                if (++count % batchSize == 0) {
                    // update UI
                    monitor.worked(1);
                    while (Display.getCurrent().readAndDispatch());
                    if (monitor.isCanceled())
                        return;  // cannot throw checked exception InterruptedException :(
                }
            }

            // copy to clipboard
            Clipboard clipboard = new Clipboard(getDisplay());
            clipboard.setContents(new Object[] {writer.toString()}, new Transfer[] {TextTransfer.getInstance()});
            clipboard.dispose();
        });
        if (monitor.isCanceled())
            throw new InterruptedException();

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

    protected void fireContentChangedEvent() {
        if (listeners != null)
            for (IDataListener listener : listeners)
                listener.contentChanged(this);
    }

    protected void loadState() {
        String prefix = getPreferenceKeyPrefix();
        visibleColumns.clear();
        for (ColumnRole column : getAllColumns()) {
            String columnPrefix = getPreferenceKeyPrefix(column);
            boolean visible = getPreference(columnPrefix + "visible", column.isDefaultVisible());
            if (visible)
                addColumn(column);
        }

        showNetworkNames = getPreference(prefix + "showNetworkNames", showNetworkNames);
        colorNetworkNames = getPreference(prefix + "colorNetworkNames", colorNetworkNames);
        colorResultSuffixes = getPreference(prefix + "colorResultSuffixes", colorResultSuffixes);
        colorNumberSeparators = getPreference(prefix + "colorNumberSeparators", colorNumberSeparators);
        colorMeasurementUnits = getPreference(prefix + "colorMeasurementUnits", colorMeasurementUnits);
    }

    @Override
    protected void saveState() {
        String prefix = getPreferenceKeyPrefix();
        for (ColumnRole column : getAllColumns()) {
            String columnPrefix = getPreferenceKeyPrefix(column);
            boolean visible = visibleColumns.indexOf(column) >= 0;
            preferences.setValue(columnPrefix + "visible", visible);
            if (visible) {
                preferences.setValue(columnPrefix + "width", getTableColumn(column).getWidth());
                preferences.setValue(columnPrefix + "rightAlign", getTableColumn(column).getAlignment() == SWT.RIGHT);
            }
        }

        preferences.setValue(prefix + "showNetworkNames", showNetworkNames);
        preferences.setValue(prefix + "colorNetworkNames", colorNetworkNames);
        preferences.setValue(prefix + "colorResultSuffixes", colorResultSuffixes);
        preferences.setValue(prefix + "colorNumberSeparators", colorNumberSeparators);
        preferences.setValue(prefix + "colorMeasurementUnits", colorMeasurementUnits);
    }

    protected String getPreferenceKeyPrefix() {
        return "DataTable." + type + ".";
    }

    protected String getPreferenceKeyPrefix(ColumnRole column) {
        return "DataTable." + type + "." + column.label + ".";
    }

    protected int getPreference(String key, int defaultValue) {
        return preferences.contains(key) ? preferences.getInt(key) : defaultValue;
    }

    protected boolean getPreference(String key, boolean defaultValue) {
        return preferences.contains(key) ? preferences.getBoolean(key) : defaultValue;
    }

    private void handleMouseDown(MouseEvent event) {
        if (isDisposed() || !isVisible())
            return;
        int columnIndex = getColumnIndexAt(event.x);
        selectedColumn = columnIndex < 0 ? null : getColumn(columnIndex);
        Debug.println("DataTable: selected column " + columnIndex + " which is " + selectedColumn.getText());
    }

    public String getSelectedField() {
        if (selectedColumn != null && !selectedColumn.isDisposed()) {
            ColumnRole column = (ColumnRole)selectedColumn.getData(COLUMNROLE_KEY);
            if (column != null)
                return column.getFieldName();
        }
        return null;
    }

    public String getSelectedCell() {
        if (getItemCount() == 0 || selectedColumn == null || selectedColumn.isDisposed())
            return null;
        ColumnRole column = (ColumnRole)selectedColumn.getData(COLUMNROLE_KEY);
        return column == null ? null : getCellValue(getFocusIndex(), column, null, -1).getString();
    }

    public void setSelectedID(long id) {
        int index = idList.indexOf(id);
        if (index != -1)
            setSelectionIndex(index);
    }

    public void setSelectedIDs(IDList selectedIDList, InterruptedFlag interrupted) throws InterruptedException {
        int[] indices = getIndices(selectedIDList, interrupted);
        Arrays.sort(indices);
        setSelectionIndices(indices);
    }

    public int[] getIndices(IDList selectedIDList, InterruptedFlag interrupted) throws InterruptedException {
        // note: this method is O(n^2) which can be prohibitive for large input+selection pairs!
        ArrayList<Integer> indicesList = new ArrayList<>();
        for (int i = 0; i < selectedIDList.size(); i++) {
            int index = idList.indexOf(selectedIDList.get(i)); // note: linear search
            if (index != -1)
                indicesList.add(index);
            if ((i & 1023) == 0 && interrupted.getFlag())
                throw new InterruptedException();
        }
        int[] indices = new int[indicesList.size()];
        for (int i = 0; i < indices.length; i++)
            indices[i] = indicesList.get(i);
        return indices;
    }
}
