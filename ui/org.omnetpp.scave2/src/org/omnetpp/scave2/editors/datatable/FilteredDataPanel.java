package org.omnetpp.scave2.editors.datatable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Displays a data table of vectors/scalars/histograms with filter
 * comboboxes.
 * 
 * @author andras
 */
public class FilteredDataPanel extends Composite {
	private FilteringPanel filterPanel;
	private DataTable table;
	private IDList idlist; // the unfiltered data list
	
	public FilteredDataPanel(Composite parent, int style, int type) {
		super(parent, style);
		initialize(type);
		configureFilterPanel();
	}

	public FilteringPanel getFilterPanel() {
		return filterPanel;
	}

	public DataTable getTable() {
		return table;
	}

	public void setIDList(IDList idlist) {
		this.idlist = idlist;
		runFilter();
	}

	public IDList getIDList() {
		return idlist;
	}

	public void setResultFileManager(ResultFileManager manager) {
		table.setResultFileManager(manager);
	}

	public ResultFileManager getResultFileManager() {
		return table.getResultFileManager();
	}

	protected void initialize(int type) {
		filterPanel = new FilteringPanel(this, SWT.NONE);
		table = new DataTable(this, SWT.MULTI, type);
		setLayout(new GridLayout(1, false));
		filterPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	}

	protected void configureFilterPanel() {
		SelectionListener selectionListener = new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				runFilter();
			}

			public void widgetDefaultSelected(SelectionEvent e) {
				runFilter();
			}
		};
			
		// when the filter button gets pressed, update the table
		filterPanel.getFilterButton().addSelectionListener(selectionListener);
		filterPanel.getRunNameCombo().addSelectionListener(selectionListener);
		filterPanel.getModuleNameCombo().addSelectionListener(selectionListener);
		filterPanel.getNameCombo().addSelectionListener(selectionListener);
	}

	protected void runFilter() {
		// run the filter on the unfiltered IDList, and set the result to the table
		ResultFileManager manager = table.getResultFileManager();
		String fileAndRunFilter = filterPanel.getRunNameCombo().getText();
		String moduleFilter = filterPanel.getModuleNameCombo().getText();
		String nameFilter = filterPanel.getNameCombo().getText();

		IDList filteredIDList = manager.getFilteredList(idlist,
				fileAndRunFilter, moduleFilter, nameFilter);
		
		table.setIDList(filteredIDList);
	}
}
