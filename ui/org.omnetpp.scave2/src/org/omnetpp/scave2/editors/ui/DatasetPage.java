package org.omnetpp.scave2.editors.ui;

import static org.omnetpp.scave2.model.DatasetType.SCALAR;
import static org.omnetpp.scave2.model.DatasetType.VECTOR;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave2.actions.EditAction;
import org.omnetpp.scave2.actions.GroupAction;
import org.omnetpp.scave2.actions.NewAction;
import org.omnetpp.scave2.actions.OpenAction;
import org.omnetpp.scave2.actions.RemoveAction;
import org.omnetpp.scave2.actions.UngroupAction;
import org.omnetpp.scave2.editors.ScaveEditor;

public class DatasetPage extends ScrolledForm {

	private Label label;
	private SashForm sashform;
	private DatasetPanel datasetPanel;
	private FilterPanel filterPanel;
	private ScaveEditor scaveEditor = null;  // the containing editor
	private String type; // "vector","scalar" or "histogram"
	
	public DatasetPage(Composite parent, int style, ScaveEditor scaveEditor, String type) {
		super(parent, style | SWT.V_SCROLL | SWT.H_SCROLL);
		this.scaveEditor = scaveEditor;
		this.type = type;
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

		scaveEditor.configureViewerButton(
				datasetPanel.getAddButton(),
				datasetPanel.getTreeViewer(),
				new NewAction()); //XXX "Add new item"?
		scaveEditor.configureViewerButton(
				datasetPanel.getRemoveButton(), 
				datasetPanel.getTreeViewer(),
				new RemoveAction());
		scaveEditor.configureViewerButton(
				datasetPanel.getEditButton(),
				datasetPanel.getTreeViewer(), 
				new EditAction());
		scaveEditor.configureViewerButton(
				datasetPanel.getGroupButton(),
				datasetPanel.getTreeViewer(), 
				new GroupAction());
		scaveEditor.configureViewerButton(
				datasetPanel.getUngroupButton(),
				datasetPanel.getTreeViewer(), 
				new UngroupAction());
		scaveEditor.configureViewerDefaultButton(
				datasetPanel.getOpenChartButton(),
				datasetPanel.getTreeViewer(), 
				new OpenAction());
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
		if (SCALAR.equals(type))
			filterPanel = new ScalarsPanel(sashform, SWT.NONE);
		else if (VECTOR.equals(type))
			filterPanel = new VectorsPanel(sashform, SWT.NONE);
		else
			filterPanel = new ScalarsPanel(sashform, SWT.NONE);
	}
}
