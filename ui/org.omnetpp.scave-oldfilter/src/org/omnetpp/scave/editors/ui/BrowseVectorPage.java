package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.VectorResultContentProvider;
import org.omnetpp.scave.editors.datatable.VectorResultLabelProvider;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;

/**
 * This is the "Browse vector" page of Scave Editor
 */
public class BrowseVectorPage extends ScaveEditorPage {

	VectorResult vector;
	Table table;
	VirtualTable<OutputVectorEntry> viewer;
	
	public BrowseVectorPage(Composite parent, ScaveEditor editor, VectorResult vector) {
		super(parent, SWT.V_SCROLL, editor);
		this.vector = vector;
		initialize();
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Browse vector");
		setFormTitle("Browse vector");
		//setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		getBody().setLayout(new GridLayout());
		String runName = vector.getFileRun().getRun().getRunName();
		String message = String.format("Here you can see the '%s' values of the '%s' module coming from the '%s' run.",
				vector.getName(), vector.getModuleName(), runName != null ? runName : "<unnamed>");
		Label label = new Label(getBody(), SWT.WRAP);
		label.setText(message);
		label.setBackground(this.getBackground());
		createTable();
		createTableViewer(table);
	}
	
	private void createTableViewer(Table table) {
		viewer = new VirtualTable<OutputVectorEntry>(table);
		viewer.setContentProvider(new VectorResultContentProvider());
		viewer.setLabelProvider(new VectorResultLabelProvider());
		viewer.setInput(vector);
	}
	
	private void createTable() {
		// create pages
		table = new Table(getBody(), SWT.VIRTUAL | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Item#");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Time");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Value");
	}
}
