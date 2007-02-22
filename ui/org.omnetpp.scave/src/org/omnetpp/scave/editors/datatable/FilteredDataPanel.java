package org.omnetpp.scave.editors.datatable;

import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.MessageBox;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Displays a data table of vectors/scalars/histograms with filter
 * comboboxes.
 *
 * This class is reusable, which means it only knows that it has to
 * display an IDList belonging to a particular ResultFileManager,
 * and has absolutely no reference to the editor, or EMF model objects,
 * or any widgets outside -- nothing.
 *
 * The user is responsible to keep contents up-to-date in case
 * ResultFileManager or IDList contents change. Refreshing can be
 * done by calling setIDList().
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
		updateFilterCombos();
		runFilter();
	}

	public IDList getIDList() {
		return idlist;
	}

	public void setResultFileManager(ResultFileManagerEx manager) {
		table.setResultFileManager(manager);
	}

	public ResultFileManagerEx getResultFileManager() {
		return table.getResultFileManager();
	}

	protected void initialize(int type) {
		GridLayout gridLayout = new GridLayout(1, false);
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		filterPanel = new FilteringPanel(this, SWT.NONE);
		filterPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		table = new DataTable(this, SWT.MULTI, type);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		filterPanel.getToggleFilterTypeButton().addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (filterPanel.isShowingAdvancedFilter())
					switchToSimpleFilter();
				else
					switchToAdvancedFilter();
			}
		});

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
		filterPanel.getAdvancedFilterText().addSelectionListener(selectionListener);
		filterPanel.getRunNameCombo().addSelectionListener(selectionListener);
		filterPanel.getModuleNameCombo().addSelectionListener(selectionListener);
		filterPanel.getNameCombo().addSelectionListener(selectionListener);
	}

	protected void updateFilterCombos() {
		filterPanel.setFilterHints(getFilterHints());
	}

	public FilterHints getFilterHints() {
		return new FilterHints(table.getResultFileManager(), idlist);
	}

	protected void runFilter() {
		// run the filter on the unfiltered IDList, and set the result to the table
		if (idlist != null) {
			Filter params = getFilterParams();
			IDList filteredIDList = ScaveModelUtil.filterIDList(idlist, params, table.getResultFileManager());
			table.setIDList(filteredIDList);
		}
	}

	public Filter getFilterParams() {
		String filterPattern;
		if (filterPanel.isShowingAdvancedFilter())
			filterPattern = filterPanel.getAdvancedFilterText().getText();
		else
			filterPattern = assembleFilterPattern();
		return new Filter(filterPattern);
	}

	private String assembleFilterPattern() {
		String runId = filterPanel.getRunNameCombo().getText();
		String moduleName = filterPanel.getModuleNameCombo().getText();
		String name = filterPanel.getNameCombo().getText();
		return new FilterUtil(runId, moduleName, name).getFilterPattern();
	}

	public void setFilterParams(Filter filter) {
		String filterPattern = filter.getFilterPattern();

		// an arbitrary pattern can only be shown in advanced view -- switch there
		if (!filterPanel.isShowingAdvancedFilter())
			filterPanel.showAdvancedFilter();
		filterPanel.getAdvancedFilterText().setText(filterPattern);
		runFilter();
	}

	public void switchToSimpleFilter() {
		String filterPattern = filterPanel.getAdvancedFilterText().getText();
		FilterUtil filterUtil = new FilterUtil(filterPattern);

		if (!filterUtil.isSimple()) {
			MessageBox messageBox = new MessageBox(getShell(), SWT.ICON_WARNING | SWT.OK | SWT.CANCEL);
			messageBox.setText("Warning");
			messageBox.setMessage("The current filter can only be represented in Basic view after "+
								  "losing some of its details.");
			if (messageBox.open() != SWT.OK)
				return;  // user cancelled
		}

		filterPanel.getRunNameCombo().setText(filterUtil.getField(FilterUtil.FIELD_RUNNAME));
		filterPanel.getModuleNameCombo().setText(filterUtil.getField(FilterUtil.FIELD_MODULENAME));
		filterPanel.getNameCombo().setText(filterUtil.getField(FilterUtil.FIELD_DATANAME));

		filterPanel.showSimpleFilter();
	}

	public void switchToAdvancedFilter() {
		filterPanel.getAdvancedFilterText().setText(assembleFilterPattern());
		filterPanel.showAdvancedFilter();
	}
}
