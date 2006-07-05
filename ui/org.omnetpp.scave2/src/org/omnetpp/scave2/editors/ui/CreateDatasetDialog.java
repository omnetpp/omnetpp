package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.ScaveModelUtil.RunIdKind;

public class CreateDatasetDialog extends Dialog {
	
	private CreateDatasetDialogArea panel;
	
	private String datasetName;
	private boolean useFilter;
	private FilterParams filterParams;
	private RunIdKind runIdKind;

	public CreateDatasetDialog(Shell parentShell) {
		super(parentShell);
	}
	
	public void setFilterParams(FilterParams params) {
		filterParams = params;
	}
	
	public String getDatasetName() {
		return datasetName;
	}
	
	public boolean useFilter() {
		return useFilter;
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
		shell.setText("Create dataset");
	}
	
	@Override
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite)super.createDialogArea(parent);
		panel = new CreateDatasetDialogArea(composite, SWT.NONE);
		panel.setLayoutData(new GridData(GridData.FILL_BOTH));
		if (filterParams != null) {
			panel.setFilterParams(filterParams);
			panel.getDatasetNameText().setText(filterParams.toString());
		}
		return composite;
	}

	@Override
	protected void okPressed() {
		applyChanges();
		super.okPressed();
	}
	
	private void applyChanges() {
		datasetName = panel.getDatasetNameText().getText();
		useFilter = panel.useFilter();
		filterParams = panel.getFilterParams();
		runIdKind = panel.getRunIdKind();
	}
}
