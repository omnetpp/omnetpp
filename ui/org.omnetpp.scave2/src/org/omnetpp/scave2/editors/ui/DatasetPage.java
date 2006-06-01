package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;

public class DatasetPage extends ScrolledForm {

	private Label label;
	private SashForm sashform;
	private DatasetPanel datasetPanel;
	private FilterPanel filterPanel;
	
	public DatasetPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL | SWT.H_SCROLL);
		initialize();
	}
	
	public TreeViewer getDatasetTreeViewer() {
		return datasetPanel.getTreeViewer();
	}
	
	public FilterPanel getFilterPanel() {
		return filterPanel;
	}
	
	private void initialize() {
		setExpandHorizontal(true);
		setExpandVertical(true);
		//setDelayedReflow(false);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		label = new Label(getBody(), SWT.NONE);
		label.setBackground(getBackground());
		label.setText("Here you can edit the dataset. " +
	      "The dataset allows you to create a subset of the input data and work with it.");
		label.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL));
		createSashForm();
		createDatasetPanel();
		createFilterPanel();
		//sashform.setMaximizedControl(datasetPanel);
	}
	
	private void createSashForm() {
		sashform = new SashForm(getBody(), SWT.VERTICAL | SWT.BORDER | SWT.SMOOTH);
		sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));
	}
	
	private void createDatasetPanel() {
		datasetPanel = new DatasetPanel(sashform, SWT.NONE);
	}
	
	private void createFilterPanel() {
		filterPanel = new ScalarsPanel(sashform, SWT.NONE);
	}
}
