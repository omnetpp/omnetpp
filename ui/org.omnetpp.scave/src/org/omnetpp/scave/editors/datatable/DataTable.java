package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.RunAttribute.CONFIG;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;
import static org.omnetpp.scave.engine.RunAttribute.RUNNUMBER;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
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
import org.omnetpp.common.util.CsvWriter;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
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
public class DataTable extends Table {

	/**
	 * Keys used in getData(),setData()
	 */
	public static final String COLUMN_KEY = "DataTable.Column";
	public static final String ITEM_KEY = "DataTable.Item";

	static class Column {

		private String text;
		private String fieldName;
		private int weight;
		private boolean defaultVisible;

		public Column(String text, String fieldName, int weight, boolean visible) {
			this.text = text;
			this.fieldName = fieldName;
			this.weight = weight;
			this.defaultVisible = visible;
		}

		public Column clone() {
			return new Column(this.text, this.fieldName, this.weight, this.defaultVisible);
		}

		public boolean equals(Object other) {
			return other instanceof Column && this.text.equals(((Column)other).text);
		}

		public int hashCode() {
			return text.hashCode();
		}
	}

	private static final Column COL_DIRECTORY = new Column("Directory", null, 60, true);
	private static final Column COL_FILE = new Column("File name", FILE,100, true);
	private static final Column COL_CONFIG = new Column("Config name", CONFIG, 100, true);
	private static final Column COL_RUNNUMBER = new Column("Run number", RUNNUMBER, 20, true);
	private static final Column COL_RUN_ID = new Column("Run id", RUN, 100, true);
	private static final Column COL_MODULE = new Column("Module", MODULE, 160, true);
	private static final Column COL_DATA = new Column("Name", NAME, 100, true);
	private static final Column COL_VALUE = new Column("Value", null, 80, true);
	private static final Column COL_COUNT = new Column("Count", null, 50, true);
	private static final Column COL_MEAN = new Column("Mean", null, 60, true);
	private static final Column COL_STDDEV = new Column("StdDev", null, 60, true);
	private static final Column COL_MIN = new Column("Min", null, 60, false);
	private static final Column COL_MAX = new Column("Max", null, 60, false);
	private static final Column COL_EXPERIMENT = new Column("Experiment", EXPERIMENT, 60, false);
	private static final Column COL_MEASUREMENT = new Column("Measurement", MEASUREMENT, 60, false);
	private static final Column COL_REPLICATION = new Column("Replication", REPLICATION, 60, false);
	
	private static final Column[] allScalarColumns = new Column[] {
		COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID, COL_MODULE, COL_DATA,
		COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
		COL_VALUE
	};

	private static final Column[] allVectorColumns = new Column[] {
		COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID, COL_MODULE, COL_DATA,
		COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
		COL_COUNT, COL_MEAN, COL_STDDEV, COL_MIN, COL_MAX
	};

	private static final Column[] allHistogramColumns = new Column[] {
		COL_DIRECTORY, COL_FILE, COL_CONFIG, COL_RUNNUMBER, COL_RUN_ID, COL_MODULE, COL_DATA,
		COL_EXPERIMENT, COL_MEASUREMENT, COL_REPLICATION,
		COL_COUNT, COL_MEAN, COL_STDDEV, COL_MIN, COL_MAX
	};

	private ResultType type;
	private ResultFileManager manager;
	private IDList idlist;
	private ListenerList listeners;
	private List<Column> visibleColumns; // list of visible columns, this list will be saved and restored
	private IPreferenceStore preferences = ScavePlugin.getDefault().getPreferenceStore();

	// holds actions for the context menu for this data table
	private MenuManager contextMenuManager = new MenuManager("#PopupMenu");
	
	private static final ResultItem[] NULL_SELECTION = new ResultItem[0];
	
	private TableItem selectedItem;
	private TableColumn selectedColumn;

	public DataTable(Composite parent, int style, ResultType type) {
		super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
		Assert.isTrue(type==ResultType.SCALAR_LITERAL || type==ResultType.VECTOR_LITERAL || type==ResultType.HISTOGRAM_LITERAL);
		this.type = type;
		setHeaderVisible(true);
		setLinesVisible(true);
		initDefaultState();
		initColumns();

		addListener(SWT.SetData, new Listener() {
			public void handleEvent(Event e) {
				TableItem item = (TableItem)e.item;
				int lineNumber = indexOf(item);
				fillTableLine(item, lineNumber);
			}
		});

		setMenu(contextMenuManager.createContextMenu(this));

		addMouseListener(new MouseAdapter() {
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

	public void setResultFileManager(ResultFileManager manager) {
		this.manager = manager;
	}

	public ResultFileManager getResultFileManager() {
		return manager;
	}

	public void setIDList(IDList idlist) {
		this.idlist = idlist;
		restoreSortOrder();
		refresh();
		fireContentChangedEvent();
	}
	
	public IDList getIDList() {
		return idlist;
	}

	public IMenuManager getContextMenuManager() {
		return contextMenuManager;
	}
	
	protected Column[] getAllColumns() {
		switch (type.getValue()) {
		case ResultType.SCALAR:		return allScalarColumns;
		case ResultType.VECTOR:		return allVectorColumns;
		case ResultType.HISTOGRAM:	return allHistogramColumns;
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
		List<String> visibleColumnTexts = Arrays.asList(columnTexts);

		visibleColumns.clear();
		for (TableColumn column : getColumns())
			column.dispose();
		
		for (Column column : getAllColumns()) {
			boolean visible = visibleColumnTexts.indexOf(column.text) >= 0;
			if (visible)
				addColumn(column);
		}
		saveState();
		layoutColumns();
		refresh();
	}

	public IDList getSelectedIDs() {
		int[] selectionIndices = getSelectionIndices();
		IDList items = new IDList();

		for (int i = 0; i < selectionIndices.length; ++i)
			items.add(idlist.get(selectionIndices[i]));
		
		return items;
	}

	public ResultItem[] getSelectedItems() {
		if (manager == null)
			return NULL_SELECTION;
		
		int[] selectionIndices = getSelectionIndices();
		ResultItem[] items = new ResultItem[selectionIndices.length];

		for (int i = 0; i < items.length; ++i) {
			items[i] = manager.getItem(idlist.get(selectionIndices[i]));
		}

		return items;
	}

	public void refresh() {
		setItemCount((int)idlist.size());
		clearAll();
	}
	
	protected void initColumns() {
		visibleColumns = new ArrayList<Column>();
		loadState();
	}

	protected TableColumn getTableColumn(Column column) {
		for (TableColumn tableColumn : getColumns()) {
			if (tableColumn.getData(COLUMN_KEY).equals(column)) {
				return tableColumn;
			}
		}
		return null;
	}
	
	protected void addColumn(Column newColumn) {
		visibleColumns.add(newColumn);
		TableColumn tableColumn = new TableColumn(this, SWT.NONE);
		tableColumn.setText(newColumn.text);
		tableColumn.setWidth(newColumn.weight);
		tableColumn.setData(COLUMN_KEY, newColumn);
		tableColumn.addSelectionListener(new SelectionAdapter() {
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
			idlist.sortByDirectory(manager, ascending);
		else if (COL_FILE.equals(column))
			idlist.sortByFileName(manager, ascending);
		else if (COL_CONFIG.equals(column))
			idlist.sortByRunAttribute(manager, CONFIG, ascending);
		else if (COL_RUNNUMBER.equals(column))
			idlist.sortByRunAttribute(manager, RUNNUMBER, ascending);
		else if (COL_RUN_ID.equals(column))
			idlist.sortByRun(manager, ascending);
		else if (COL_MODULE.equals(column))
			idlist.sortByModule(manager, ascending);
		else if (COL_DATA.equals(column))
			idlist.sortByName(manager, ascending);
		else if (COL_VALUE.equals(column))
			idlist.sortScalarsByValue(manager, ascending);
		else if (COL_COUNT.equals(column))
			idlist.sortVectorsByLength(manager, ascending);
		else if (COL_MEAN.equals(column))
			idlist.sortVectorsByMean(manager, ascending);
		else if (COL_STDDEV.equals(column))
			idlist.sortVectorsByStdDev(manager, ascending);
		else if (COL_MIN.equals(column))
			idlist.sortVectorsByMin(manager, ascending);
		else if (COL_MAX.equals(column))
			idlist.sortVectorsByMax(manager, ascending);
		else if (COL_EXPERIMENT.equals(column))
			idlist.sortByRunAttribute(manager, EXPERIMENT, ascending);
		else if (COL_MEASUREMENT.equals(column))
			idlist.sortByRunAttribute(manager, MEASUREMENT, ascending);
		else if (COL_REPLICATION.equals(column))
			idlist.sortByRunAttribute(manager, REPLICATION, ascending);
	}

	protected void layoutColumns() {
		double unit = getWidthUnit();
		for (int i = 0; i < visibleColumns.size(); ++i) {
			Column column = visibleColumns.get(i);
			TableColumn tableColumn = getTableColumn(column);
			if (tableColumn != null)
				tableColumn.setWidth((int)(column.weight * unit));
		}
	}

	private double getWidthUnit() {
		int sumOfWeights = 0;
		for (Column column : visibleColumns)
			sumOfWeights += (column.weight);

		return (double)getSize().x / sumOfWeights;
	}

	protected void fillTableLine(TableItem item, int lineNumber) {
		if (manager == null)
			return;
		
		long id = idlist.get(lineNumber);
		ResultItem result = manager.getItem(id);
		item.setData(ITEM_KEY, result);

		for (int i = 0; i < visibleColumns.size(); ++i) {
			Column column = visibleColumns.get(i);
			if (COL_DIRECTORY.equals(column))
				item.setText(i, result.getFileRun().getFile().getDirectory());
			else if (COL_FILE.equals(column)) {
				String fileName = result.getFileRun().getFile().getFileName();
				item.setText(i, fileName);
			}
			else if (COL_CONFIG.equals(column)) {
				String config = result.getFileRun().getRun().getAttribute(CONFIG);
				item.setText(i, config != null ? config : "n.a.");
			}
			else if (COL_RUNNUMBER.equals(column)) {
				String runNumber = result.getFileRun().getRun().getAttribute(RUNNUMBER);
				item.setText(i, runNumber != null ? runNumber : "n.a.");
			}
			else if (COL_RUN_ID.equals(column))
				item.setText(i, result.getFileRun().getRun().getRunName());
			else if (COL_MODULE.equals(column))
				item.setText(i, result.getModuleName());
			else if (COL_DATA.equals(column))
				item.setText(i, result.getName());
			else if (COL_EXPERIMENT.equals(column)) {
				String experiment = result.getFileRun().getRun().getAttribute(EXPERIMENT);
				item.setText(i, experiment != null ? experiment : "n.a.");
			}
			else if (COL_MEASUREMENT.equals(column)) {
				String measurement = result.getFileRun().getRun().getAttribute(MEASUREMENT);
				item.setText(i, measurement != null ? measurement : "n.a.");
			}
			else if (COL_REPLICATION.equals(column)) {
				String replication = result.getFileRun().getRun().getAttribute(REPLICATION);
				item.setText(i, replication != null ? replication : "n.a.");
			}
			else if (type == ResultType.SCALAR_LITERAL) {
				ScalarResult scalar = (ScalarResult)result;
				if (COL_VALUE.equals(column))
					item.setText(i, String.valueOf(scalar.getValue()));
			}
			else if (type == ResultType.VECTOR_LITERAL) {
				VectorResult vector = (VectorResult)result;
				if (COL_COUNT.equals(column)) {
					int count = vector.count();
					item.setText(i, count >= 0 ? String.valueOf(count) : "n.a.");
				}
				else if (COL_MEAN.equals(column)) {
					double mean = vector.mean();
					item.setText(i, Double.isNaN(mean) ? "n.a." : String.valueOf(mean));
				}
				else if (COL_STDDEV.equals(column)) {
					double stddev = vector.stddev();
					item.setText(i, Double.isNaN(stddev) ? "n.a." : String.valueOf(stddev));
				}
				else if (COL_MIN.equals(column)) {
					double min = vector.min();
					item.setText(i, Double.isNaN(min) ? "n.a." : String.valueOf(min));
				}
				else if (COL_MAX.equals(column)) {
					double max = vector.max();
					item.setText(i, Double.isNaN(max) ? "n.a." : String.valueOf(max));
				}
			}
			else if (type == ResultType.HISTOGRAM_LITERAL) {
				HistogramResult histogram = (HistogramResult)result;
				if (COL_COUNT.equals(column)) {
					int count = histogram.count();
					item.setText(i, count >= 0 ? String.valueOf(count) : "n.a.");
				}
				else if (COL_MEAN.equals(column)) {
					double mean = histogram.mean();
					item.setText(i, Double.isNaN(mean) ? "n.a." : String.valueOf(mean));
				}
				else if (COL_STDDEV.equals(column)) {
					double stddev = histogram.stddev();
					item.setText(i, Double.isNaN(stddev) ? "n.a." : String.valueOf(stddev));
				}
				else if (COL_MIN.equals(column)) {
					double min = histogram.min();
					item.setText(i, Double.isNaN(min) ? "n.a." : String.valueOf(min));
				}
				else if (COL_MAX.equals(column)) {
					double max = histogram.max();
					item.setText(i, Double.isNaN(max) ? "n.a." : String.valueOf(max));
				}
			}
		}
	}

	protected void toCSV(CsvWriter writer, int lineNumber) {
		if (manager == null)
			return;
		
		ResultItem result = manager.getItem(idlist.get(lineNumber));

		for (int i = 0; i < visibleColumns.size(); ++i) {
			Column column = visibleColumns.get(i);
			if (COL_DIRECTORY.equals(column))
				writer.addField(result.getFileRun().getFile().getDirectory());
			else if (COL_FILE.equals(column)) {
				String fileName = result.getFileRun().getFile().getFileName();
				writer.addField(fileName);
			}
			else if (COL_CONFIG.equals(column)) {
				String config = result.getFileRun().getRun().getAttribute(CONFIG);
				writer.addField(config != null ? config : "n.a.");
			}
			else if (COL_RUNNUMBER.equals(column)) {
				String config = result.getFileRun().getRun().getAttribute(RUNNUMBER);
				writer.addField(config != null ? config : "n.a.");
			}
			else if (COL_RUN_ID.equals(column))
				writer.addField(result.getFileRun().getRun().getRunName());
			else if (COL_MODULE.equals(column))
				writer.addField(result.getModuleName());
			else if (COL_DATA.equals(column))
				writer.addField(result.getName());
			else if (COL_EXPERIMENT.equals(column)) {
				String experiment = result.getFileRun().getRun().getAttribute(EXPERIMENT);
				writer.addField(experiment != null ? experiment : "n.a.");
			}
			else if (COL_MEASUREMENT.equals(column)) {
				String measurement = result.getFileRun().getRun().getAttribute(MEASUREMENT);
				writer.addField(measurement != null ? measurement : "n.a.");
			}
			else if (COL_REPLICATION.equals(column)) {
				String replication = result.getFileRun().getRun().getAttribute(REPLICATION);
				writer.addField(replication != null ? replication : "n.a.");
			}
			else if (type == ResultType.SCALAR_LITERAL) {
				ScalarResult scalar = manager.getScalar(idlist.get(lineNumber));
				if (COL_VALUE.equals(column))
					writer.addField(String.valueOf(scalar.getValue()));
			}
			else if (type == ResultType.VECTOR_LITERAL) {
				VectorResult vector = manager.getVector(idlist.get(lineNumber));
				if (COL_COUNT.equals(column)) {
					int count = vector.count();
					writer.addField(count >= 0 ? String.valueOf(count) : "n.a.");
				}
				else if (COL_MEAN.equals(column)) {
					double mean = vector.mean();
					writer.addField(Double.isNaN(mean) ? "n.a." : String.valueOf(mean));
				}
				else if (COL_STDDEV.equals(column)) {
					double stddev = vector.stddev(); 
					writer.addField(Double.isNaN(stddev) ? "n.a." : String.valueOf(stddev));
				}
				else if (COL_MIN.equals(column)) {
					double min = vector.min();
					writer.addField(Double.isNaN(min) ? "n.a." : String.valueOf(min));
				}
				else if (COL_MAX.equals(column)) {
					double max = vector.max();
					writer.addField(Double.isNaN(max) ? "n.a." : String.valueOf(max));
				}
			}
			else if (type == ResultType.HISTOGRAM_LITERAL) {
				// TODO histogram table columns
			}
		}

		writer.endRecord();
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

	public void addDataTableListener(IDataTableListener listener) {
		if (listeners == null)
			listeners = new ListenerList();
		listeners.add(listener);
	}

	public void removeDataTableListener(IDataTableListener listener) {
		if (listeners != null)
			listeners.remove(listener);
	}

	protected void fireContentChangedEvent() {
		if (listeners != null) {
			for (Object listener : new ArrayList<Object>(Arrays.asList(this.listeners.getListeners())))
				((IDataTableListener)listener).contentChanged(this);
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
			}
		}
	}
	
	protected void loadState() {
		if (preferences != null) {
			visibleColumns.clear();
			for (Column column : getAllColumns()) {
				boolean visible = preferences.getBoolean(getPreferenceStoreKey(column, "visible"));
				if (visible)
					addColumn(column.clone());
			}
		}
	}
	
	protected void saveState() {
		if (preferences != null) {
			for (Column column : getAllColumns()) {
				boolean visible = visibleColumns.indexOf(column) >= 0;
				preferences.setValue(getPreferenceStoreKey(column, "visible"), visible);
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
				if (rect.y > clientRect.y + clientRect.height) 	return;
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
			return (ResultItem)selectedItem.getData(ITEM_KEY);
		}
		return null;
	}
	
	public void setSelectionByID(long id) {
		int index = idlist.indexOf(id);
		if (index >= 0)
			setSelection(index);
	}
}
