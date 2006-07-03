package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.actions.EditAction;
import org.omnetpp.scave2.actions.GroupAction;
import org.omnetpp.scave2.actions.NewAction;
import org.omnetpp.scave2.actions.OpenAction;
import org.omnetpp.scave2.actions.RemoveAction;
import org.omnetpp.scave2.actions.UngroupAction;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.DataTable;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

//FIXME close this page when dataset gets deleted
public class DatasetPage extends ScaveEditorPage {

	private Dataset dataset; //backref to the model object we operate on
	private DatasetPanel datasetPanel;
	private FilteredDataPanel filterPanel;
	
	private IResultFilesChangeListener resultFilesChangeListener; // we'll need to unhook on dispose()
	private INotifyChangedListener modelChangeListener; // we'll need to unhook on dispose()
	
	public DatasetPage(Composite parent, ScaveEditor scaveEditor, Dataset dataset) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, scaveEditor);
		this.dataset = dataset;
		initialize();
	}
	
	public TreeViewer getDatasetTreeViewer() {
		return datasetPanel.getTreeViewer();
	}
	
	public FilteredDataPanel getFilterPanel() {
		return filterPanel;
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
		Label label = new Label(getBody(), SWT.NONE);
		label.setBackground(getBackground()); //XXX make it wrap
		label.setText("Here you can edit the dataset. " +
	      "The dataset allows you to create a subset of the input data and work with it. "+
	      "Datasets may include processing steps, and one dataset can serve as input for another.");
		label.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL));
		createFormContents();
		
		// configure dataset treeviewer
		TreeViewer treeViewer = getDatasetTreeViewer();
		scaveEditor.configureTreeViewer(treeViewer);
		treeViewer.setInput(dataset);

		// set up filtered data panel
		filterPanel.setResultFileManager(scaveEditor.getResultFileManager());
		updateDataTable();

		// make the data table follow the treeviewer's selection
		treeViewer.addPostSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				updateDataTable();
			}
		});
		// update whenever files get loaded/unloaded
		//XXX unregister listener when we get disposed
		scaveEditor.getResultFileManager().addListener(resultFilesChangeListener = 
			new IResultFilesChangeListener() {
			public void resultFileManagerChanged(ResultFileManager manager) {
				updateDataTable();
			}
		});
		// update on model changes
		//TODO try to limit notifications to relevant ones
		IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
		notifier.addListener(modelChangeListener = new INotifyChangedListener() {
			public void notifyChanged(Notification notification) {
				updateDataTable();
			}
		});
		
		// set up actions
		configureViewerButton(
				datasetPanel.getAddButton(),
				datasetPanel.getTreeViewer(),
				new NewAction()); //XXX "Add new item"?
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

	protected void updateDataTable() {
		TreeViewer treeViewer = getDatasetTreeViewer();
		IStructuredSelection selection = (IStructuredSelection)treeViewer.getSelection(); // tree selection is always IStructuredSelection
		Object selected = selection.getFirstElement();
		if (selected instanceof EObject) {
			Dataset dataset = ScaveModelUtil.findEnclosingObject((EObject)selected, Dataset.class);
			DatasetItem item = ScaveModelUtil.findEnclosingObject((EObject)selected, DatasetItem.class);
			if (dataset != null) {
				ResultFileManagerEx manager = scaveEditor.getResultFileManager();
				IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, item);
				filterPanel.setIDList(idlist);
			}
		}
	}

	private void createFormContents() {
		SashForm sashform = new SashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));

		// create dataset treeviewer with buttons
		datasetPanel = new DatasetPanel(sashform, SWT.NONE);

		// create data panel
		int datasetType = dataset.getType().getValue();
		int type = datasetType==DatasetType.SCALAR ? DataTable.TYPE_SCALAR
				 : datasetType==DatasetType.VECTOR ? DataTable.TYPE_VECTOR
				 : datasetType==DatasetType.HISTOGRAM ? DataTable.TYPE_HISTOGRAM 
				 : -1;
		filterPanel = new FilteredDataPanel(sashform, SWT.NONE, type);
	}
}
