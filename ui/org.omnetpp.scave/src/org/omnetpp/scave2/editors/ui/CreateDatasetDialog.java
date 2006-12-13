package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

public class CreateDatasetDialog extends DatasetDialog {
	
	private String datasetName;
	private Text datasetNameText;

	public CreateDatasetDialog(Shell parentShell, String dialogTitle) {
		super(parentShell, dialogTitle);
	}
	
	public String getDatasetName() {
		return datasetName;
	}
	

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite composite = (Composite)super.createDialogArea(parent);
		GridLayout gridLayout = new GridLayout();
		gridLayout.numColumns = 2;
		gridLayout.verticalSpacing = 20;
		composite.setLayout(gridLayout);
		GridData gridData1 = new GridData();
		gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		gridData1.grabExcessHorizontalSpace = true;
		Label datasetNameLabel = new Label(composite, SWT.NONE);
		datasetNameLabel.setText("Name:");
		datasetNameText = new Text(composite, SWT.BORDER);
		datasetNameText.setLayoutData(gridData1);
		if (getFilterParams() != null)
			datasetNameText.setText(getFilterParams().toString());
		Composite panel = createDataItemsPanel(composite);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false, 2, 1));
		return composite;
	}

	protected void applyChanges() {
		super.applyChanges();
		datasetName = datasetNameText.getText();
	}
}
