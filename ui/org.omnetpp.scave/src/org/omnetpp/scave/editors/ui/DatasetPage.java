package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.GroupAction;
import org.omnetpp.scave.actions.NewAction;
import org.omnetpp.scave.actions.OpenAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.actions.UngroupAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;

//FIXME close this page when dataset gets deleted
public class DatasetPage extends ScaveEditorPage {

	private Dataset dataset; //backref to the model object we operate on
	private DatasetPanel datasetPanel;

	private IResultFilesChangeListener resultFilesChangeListener; // we'll need to unhook on dispose()
	private INotifyChangedListener modelChangeListener; // we'll need to unhook on dispose()

	public DatasetPage(Composite parent, ScaveEditor scaveEditor, Dataset dataset) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, scaveEditor);
		this.dataset = dataset;
		initialize();
	}

	public void updatePage(Notification notification) {
		if (ScaveModelPackage.eINSTANCE.getDataset_Name().equals(notification.getFeature())) {
			setPageTitle("Dataset: " + dataset.getName());
			setFormTitle("Dataset: " + dataset.getName());
		}
	}

	public TreeViewer getDatasetTreeViewer() {
		return datasetPanel.getTreeViewer();
	}

	private void initialize() {
		// set up UI
		setPageTitle("Dataset: " + dataset.getName());
		setFormTitle("Dataset: " + dataset.getName());
		setExpandHorizontal(true);
		setExpandVertical(true);
		//setDelayedReflow(false);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		Label label = new Label(getBody(), SWT.WRAP);
		label.setBackground(getBackground());
		label.setText("Here you can edit the dataset. " +
	      "The dataset allows you to create a subset of the input data and work with it. "+
	      "Datasets may include processing steps, and one dataset can serve as input for another.");
		label.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL));
		
		// create dataset treeviewer with buttons
		datasetPanel = new DatasetPanel(getBody(), SWT.NONE);

		// configure dataset treeviewer
		TreeViewer treeViewer = getDatasetTreeViewer();
		scaveEditor.configureTreeViewer(treeViewer);
		treeViewer.setInput(dataset);

		// set up actions
		configureViewerButton(
				datasetPanel.getNewChildButton(),
				datasetPanel.getTreeViewer(),
				new NewAction(dataset, true));
		configureViewerButton(
				datasetPanel.getNewSiblingButton(),
				datasetPanel.getTreeViewer(),
				new NewAction(dataset, false));
		configureViewerButton(
				datasetPanel.getRemoveButton(),
				datasetPanel.getTreeViewer(),
				new RemoveAction());
		configureViewerButton(
				datasetPanel.getEditButton(),
				datasetPanel.getTreeViewer(),
				new EditAction());
		configureViewerButton(
				datasetPanel.getGroupButton(),
				datasetPanel.getTreeViewer(),
				new GroupAction());
		configureViewerButton(
				datasetPanel.getUngroupButton(),
				datasetPanel.getTreeViewer(),
				new UngroupAction());
		configureViewerDefaultButton(
				datasetPanel.getOpenChartButton(),
				datasetPanel.getTreeViewer(),
				new OpenAction());
	}

	@Override
	public void dispose() {
		// deregister listeners we hooked on external objects
		scaveEditor.getResultFileManager().removeListener(resultFilesChangeListener);
		IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
		notifier.removeListener(modelChangeListener);

		super.dispose();
	}
}
