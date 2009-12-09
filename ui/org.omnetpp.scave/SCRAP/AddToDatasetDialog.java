/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.SCRAP;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.model.Dataset;

public class AddToDatasetDialog extends DatasetDialog {

	private Dataset[] datasets;
	private Dataset selectedDataset;
	private Combo datasetCombo;

	public AddToDatasetDialog(Shell parentShell, Dataset[] datasets) {
		super(parentShell, "Add to dataset");
		this.datasets = datasets;
	}

	public Dataset getSelectedDataset() {
		return selectedDataset;
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
		datasetNameLabel.setText("Dataset:");
		datasetCombo = new Combo(composite, SWT.DROP_DOWN | SWT.READ_ONLY);
		datasetCombo.setLayoutData(gridData1);
		datasetCombo.setItems(getDatasetNames());
		datasetCombo.select(0);
		Composite panel = createDataItemsPanel(composite);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.TOP, true, false, 2, 1));
		return composite;
	}

	private String[] getDatasetNames() {
		String[] names = new String[datasets.length];
		for (int i = 0; i < names.length; ++i)
			names[i] = datasets[i].getName();
		return names;
	}

	protected void applyChanges() {
		super.applyChanges();
		int index = datasetCombo.getSelectionIndex();
		if (index >= 0)
			selectedDataset = datasets[index];
	}
}
