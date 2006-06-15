package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.editors.ScaveEditor;

public class DatasetPage extends ScrolledForm {

	private Label label;
	private SashForm sashform;
	private DatasetPanel datasetPanel;
	private FilterPanel filterPanel;
	private ScaveEditor scaveEditor = null;  // the containing editor
	
	public DatasetPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style | SWT.V_SCROLL | SWT.H_SCROLL);
		this.scaveEditor = scaveEditor;
		initialize();
	}
	
	public TreeViewer getDatasetTreeViewer() {
		return datasetPanel.getTreeViewer();
	}
	
	public TableViewer getDatasetTableViewer() {
		return filterPanel != null ? filterPanel.getTableViewer() : null;
	}
	
	public FilterPanel getFilterPanel() {
		return filterPanel;
	}
	
	public Button getAddButton() {
		return datasetPanel.getAddButton();
	}

	public Button getRemoveButton() {
		return datasetPanel.getRemoveButton();
	}

	public Button getEditButton() {
		return datasetPanel.getEditButton();
	}

	public Button getCreateChartButton() {
		return datasetPanel.getCreateChartButton();
	}
	
	public Button getOpenChartButton() {
		return datasetPanel.getOpenChartButton();
	}

	public Button getGroupButton() {
		return datasetPanel.getGroupButton();
	}

	public Button getUngroupButton() {
		return datasetPanel.getUngroupButton();
	}
	
	
	public void addScalarsPanel() {
		filterPanel = new ScalarsPanel(sashform, SWT.NONE);
	}
	
	public void addVectorsPanel() {
		filterPanel = new VectorsPanel(sashform, SWT.NONE);
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
		//sashform.setMaximizedControl(datasetPanel);

		final TreeViewer treeViewer = datasetPanel.getTreeViewer();
		//TODO buttons
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
}
