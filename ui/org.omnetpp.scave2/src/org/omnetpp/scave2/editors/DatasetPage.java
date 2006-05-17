package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;

public class DatasetPage extends ScrolledForm {

	Composite datasetPanel;
	
	public DatasetPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);
		initialize();
	}
	
	private void initialize() {
		setExpandHorizontal(true);
		getBody().setLayout(new FillLayout());
		createDatasetPanel();
	}
	
	private void createDatasetPanel() {
		datasetPanel = new DatasetPanel(getBody(), SWT.NONE);
	}
}
