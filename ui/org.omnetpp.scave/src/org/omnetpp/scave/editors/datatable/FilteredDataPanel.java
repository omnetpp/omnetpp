package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Displays a data table of vectors/scalars/histograms with filter
 * combo boxes.
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
	
	private static final IDList EMPTY_IDLIST = new IDList();

	public FilteredDataPanel(Composite parent, int style, ResultType type) {
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

	public void setResultFileManager(ResultFileManager manager) {
		table.setResultFileManager(manager);
	}

	public ResultFileManager getResultFileManager() {
		return table.getResultFileManager();
	}

	protected void initialize(ResultType type) {
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
					trySwitchToSimpleFilter();
				else
					switchToAdvancedFilter();
			}
		});
	}
	
	protected void configureFilterPanel() {
		SelectionListener selectionListener = new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				// check the filter string
				if (!isFilterPatternValid()) {
					MessageDialog.openWarning(getShell(), "Error in Filter Expression", "Filter expression is invalid, please fix it. Table contents not changed.");
					return;
				}
				runFilter();
			}
			public void widgetDefaultSelected(SelectionEvent e) {
				widgetSelected(e);  //delegate
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
		if (!filterPanel.isDisposed())
			filterPanel.setFilterHints(getFilterHints());
	}

	public FilterHints getFilterHints() {
		if (table.getResultFileManager() != null)
			return new FilterHints(table.getResultFileManager(), idlist);
		else
			return new FilterHints();
	}

	protected void runFilter() {
		Assert.isTrue(idlist!=null);
		
		if (table.getResultFileManager() == null) {
			// no result file manager, show empty table
			table.setIDList(EMPTY_IDLIST);
		}
		else if (isFilterPatternValid()) {
			// run the filter on the unfiltered IDList, and set the result to the table
			Filter filter = getFilter();
			IDList filteredIDList = ScaveModelUtil.filterIDList(idlist, filter, table.getResultFileManager());
			table.setIDList(filteredIDList);
		}
		else {
			// fallback: if filter is not valid, just show an unfiltered list. This should be
			// done because we get invoked indirectly as well, like when files get added/removed
			// on the Inputs page. For the same reason, this function should not bring up an 
			// error dialog (but the Filter button itself may do so).
			table.setIDList(idlist);
		}
	}

	public boolean isFilterPatternValid() {
		try {
			ResultFileManager.checkPattern(getFilter().getFilterPattern());
		} catch (Exception e) {
			return false; // apparently not valid
		}
		return true;
	}

	public Filter getFilter() {
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

	/**
	 * Switches the filter from "Advanced" to "Basic" mode. If this cannot be done
	 * (filter string invalid or too complex), the user is prompted with a dialog,
	 * and switching may or may not actually take place depending on the answer. 
	 * @return true if switching was actually done.
	 */
	public boolean trySwitchToSimpleFilter() {
		if (!isFilterPatternValid()) {
			MessageDialog.openWarning(getShell(), "Error in Filter Expression", "Filter expression is invalid, please fix it first. (Or, just delete the whole text.)");
			return false;
		}
		
		String filterPattern = filterPanel.getAdvancedFilterText().getText();
		FilterUtil filterUtil = new FilterUtil(filterPattern, true);
		if (filterUtil.isLossy()) {
			boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter cannot be represented in Basic view without losing some of its details.");
			if (!ok)
				return false;  // user cancelled
		}

		String[] supportedFields = new String[] {RUN, MODULE, NAME};
		if (!filterUtil.containsOnly(supportedFields)) {
			boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter contains fields not present in Basic view. These extra fields will be discarded.");
			if (!ok)
				return false;  // user cancelled
		}

		filterPanel.getRunNameCombo().setText(filterUtil.getField(RUN));
		filterPanel.getModuleNameCombo().setText(filterUtil.getField(MODULE));
		filterPanel.getNameCombo().setText(filterUtil.getField(NAME));

		filterPanel.showSimpleFilter();
		runFilter();
		return true;
	}

	/**
	 * Switches the filter from "Basic" to "Advanced" mode. This is always successful (unlike the opposite way).
	 */
	public void switchToAdvancedFilter() {
		filterPanel.getAdvancedFilterText().setText(assembleFilterPattern());
		filterPanel.showAdvancedFilter();
		runFilter();
	}
	
	/**
	 * Shows/hides the filter panel.
	 */
	public void showFilterPanel(boolean show) {
		if (show != isFilterPanelVisible()) {
			filterPanel.setVisible(show);
			GridData data = (GridData)filterPanel.getLayoutData();
			data.exclude = !show;
			layout(true, true);
		}
	}
	
	/**
	 * Returns {@code true} iff the filter panel is visible.
	 */
	public boolean isFilterPanelVisible() {
		GridData data = (GridData)filterPanel.getLayoutData();
		return !data.exclude;
	}
}
