/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

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
	
	public CreateDatasetDialog(Shell parent, String title) {
		this(parent, SHOW_SELECTION | SHOW_FILTER, title);
	}

	public CreateDatasetDialog(Shell parentShell, int style, String dialogTitle) {
		super(parentShell, style, dialogTitle);
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
