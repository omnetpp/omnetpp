package org.omnetpp.scave2.editors.datatable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

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
	public static final int TYPE_SCALAR = 0;
	public static final int TYPE_VECTOR = 1;
	public static final int TYPE_HISTOGRAM = 2;

	private int type;
	private ResultFileManagerEx manager;
	private IDList idlist;
	
	public DataTable(Composite parent, int style, int type) {
		super(parent, style | SWT.VIRTUAL | SWT.FULL_SELECTION);
		Assert.isTrue(type==TYPE_SCALAR || type==TYPE_VECTOR || type==TYPE_HISTOGRAM);
		this.type = type;
		setHeaderVisible(true);
		setLinesVisible(true);
		addColumns();

		addListener(SWT.SetData, new Listener() {
			public void handleEvent(Event e) {
				TableItem item = (TableItem)e.item;
				int lineNumber = indexOf(item);
				fillTableLine(item, lineNumber);
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
	
	public int getType() {
		return type;
	}

	public void setResultFileManager(ResultFileManagerEx manager) {
		this.manager = manager;
	}
	
	public ResultFileManagerEx getResultFileManager() {
		return manager;
	}

	public void setIDList(IDList idlist) {
		this.idlist = idlist;
		refresh();
	}

	public IDList getIDList() {
		return idlist;
	}

	public void refresh() {
		setItemCount((int)idlist.size());
		clearAll();
	}

	protected void addColumns() {
		if (type==TYPE_SCALAR) {
			addColumn("Directory", 60);
			addColumn("File, Run#", 100);
			addColumn("RunName", 100);
			addColumn("Module", 160);
			addColumn("Name", 100);
			addColumn("Value", 80);
		}
		else if (type==TYPE_VECTOR) {
			addColumn("Directory", 60);
			addColumn("File, Run#", 100);
			addColumn("RunName", 100);
			addColumn("Module", 160);
			addColumn("Name", 100);
			addColumn("Count", 50);
			addColumn("Mean", 60);
			addColumn("Stddev", 60);
		}
		else if (type==TYPE_HISTOGRAM) {
			addColumn("Directory", 60);
			addColumn("File, Run#", 100);
			addColumn("RunName", 100);
			addColumn("Module", 160);
			addColumn("Name", 100);
			addColumn("Mean", 60);
			addColumn("Stddev", 60);
		}		
	}

	protected TableColumn addColumn(String text, int width) {
		TableColumn tableColumn = new TableColumn(this, SWT.NONE);
		tableColumn.setWidth(width);
		tableColumn.setText(text);
		return tableColumn;
	}

	protected void fillTableLine(TableItem item, int lineNumber) {
		if (type==TYPE_SCALAR) {
			ScalarResult d = manager.getScalar(idlist.get(lineNumber));
			item.setText(0, d.getFileRun().getFile().getDirectory());
			int runNumber = d.getFileRun().getRun().getRunNumber();
			if (runNumber==0)
				item.setText(1, d.getFileRun().getFile().getFileName());
			else
				item.setText(1, d.getFileRun().getFile().getFileName()+"#"+runNumber);
			item.setText(2, d.getFileRun().getRun().getRunName());
			item.setText(3, d.getModuleName());
			item.setText(4, d.getName());
			item.setText(5, ""+d.getValue());
		}
		else if (type==TYPE_VECTOR) {
			VectorResult d = manager.getVector(idlist.get(lineNumber));
			item.setText(0, d.getFileRun().getFile().getDirectory());
			int runNumber = d.getFileRun().getRun().getRunNumber();
			if (runNumber==0)
				item.setText(1, d.getFileRun().getFile().getFileName());
			else
				item.setText(1, d.getFileRun().getFile().getFileName()+"#"+runNumber);
			item.setText(2, d.getFileRun().getRun().getRunName());
			item.setText(3, d.getModuleName());
			item.setText(4, d.getName());
			item.setText(5, "not counted");
			item.setText(6, "not yet");
			item.setText(7, "not yet");
		}
		else if (type==TYPE_HISTOGRAM) {
			//TODO
		}		
	}
}
