package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave2.model.FilterHints;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.ScaveModelUtil.RunIdKind;

/**
 * Common base class for CreateDatasetDialog and AddToDatasetDialog.
 *
 * @author tomi
 */
public abstract class DatasetDialog extends Dialog {
	
	private String dialogTitle;
	private DataItemsPanel panel;
	
	private boolean useFilter;
	private FilterParams filterParams;
	private FilterHints filterHints;
	private RunIdKind runIdKind;

	public DatasetDialog(Shell parentShell, String dialogTitle) {
		super(parentShell);
		this.dialogTitle = dialogTitle;
	}
	
	public void setFilterParams(FilterParams params) {
		filterParams = params;
	}
	
	public void setFilterHints(FilterHints hints) {
		filterHints = hints;
	}
	
	public boolean useFilter() {
		return useFilter;
	}
	
	public void setUseFilter(boolean flag) {
		useFilter = flag;
	}
	
	public FilterParams getFilterParams() {
		return filterParams;
	}
	
	public RunIdKind getRunIdKind() {
		return runIdKind;
	}
	
	@Override
	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		shell.setText(dialogTitle);
	}

	protected Composite createDataItemsPanel(Composite parent ) {
		panel = new DataItemsPanel(parent, SWT.NONE);
		panel.setLayoutData(new GridData(GridData.FILL_BOTH));
		panel.setUseFilter(useFilter);
		if (filterParams != null) {
			panel.setFilterParams(filterParams);
		}
		if (filterHints != null) {
			panel.setFilterHints(filterHints);
		}
		return panel;
	}

	@Override
	protected void okPressed() {
		applyChanges();
		super.okPressed();
	}
	
	protected void applyChanges() {
		if (panel != null) {
			useFilter = panel.useFilter();
			filterParams = panel.getFilterParams();
			runIdKind = panel.getRunIdKind();
		}
	}
}
