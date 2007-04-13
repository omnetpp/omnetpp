package org.omnetpp.scave.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * View that shows the content of a dataset.
 * 
 * @author tomi
 */
// work in progress ...
// TODO update tables on input file and model change
// TODO add icon
public class DatasetView extends ViewWithMessagePart {

	public static final String ID = "org.omnetpp.scave.DatasetView";
	
	private Composite panel;
	private StackLayout layout;
	private FilteredDataPanel vectorsPanel;
	private FilteredDataPanel scalarsPanel;
	private FilteredDataPanel histogramsPanel;
	
	private Dataset selectedDataset;
	private DatasetItem selectedItem;
	
	private IAction showScalarsAction;
	private IAction showVectorsAction;
	private IAction showHistogramsAction;
	
	protected ISelectionListener selectionChangedListener;
	private static final IDList EMPTY = new IDList();
	
	
	
	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		hookSelectionChangedListener();
		createToolbarButtons();
	}


	@Override
	protected Control createViewControl(Composite parent) {
		panel = new Composite(parent, SWT.NONE);
		panel.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											  GridData.GRAB_VERTICAL |
											  GridData.FILL_BOTH));
		layout = new StackLayout();
		panel.setLayout(layout);

		// create pages
		vectorsPanel = new FilteredDataPanel(panel, SWT.NONE, DataTable.TYPE_VECTOR);
		scalarsPanel = new FilteredDataPanel(panel, SWT.NONE, DataTable.TYPE_SCALAR);
		histogramsPanel = new FilteredDataPanel(panel, SWT.NONE, DataTable.TYPE_HISTOGRAM);
		
		setVisibleTable(vectorsPanel);
		
		return panel;
	}
	
	private void createToolbarButtons() {
		showScalarsAction = new ShowTableAction(ResultType.SCALAR_LITERAL);
		showVectorsAction = new ShowTableAction(ResultType.VECTOR_LITERAL);
		showHistogramsAction = new ShowTableAction(ResultType.HISTOGRAM_LITERAL);
		IToolBarManager manager = getViewSite().getActionBars().getToolBarManager();
		manager.add(showScalarsAction);	// TODO: make buttons radio-style
		manager.add(showVectorsAction);
		manager.add(showHistogramsAction);
	}
	
	@Override
	public void dispose() {
		unhookSelectionChangedListener();
		super.dispose();
	}

	private void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				setViewerInput(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	private void setVisibleTable(FilteredDataPanel table) {
		layout.topControl = table;
		panel.layout();
	}
	
	private void setViewerInput(ISelection selection) {
		if (getActiveEditor() instanceof ScaveEditor &&
				selection instanceof IStructuredSelection) {
			ScaveEditor editor = (ScaveEditor)getActiveEditor();
			Object selected = ((IStructuredSelection)selection).getFirstElement();
			if (selected instanceof EObject) {
				Dataset dataset = ScaveModelUtil.findEnclosingOrSelf((EObject)selected, Dataset.class);
				DatasetItem item = ScaveModelUtil.findEnclosingOrSelf((EObject)selected, DatasetItem.class);
				if (dataset != selectedDataset || item != selectedItem) {
					updateTables(dataset, item, editor);
					return;
				}
			}
		}
		
		updateTables(null, null, null);
	}
	
	private void updateTables(Dataset dataset, DatasetItem item, ScaveEditor editor) {
		if (dataset != null) {
			// set inputs of tables
			ResultFileManager manager = editor.getResultFileManager();
			IDList scalars = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.SCALAR_LITERAL);
			IDList vectors = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.VECTOR_LITERAL);
			IDList histograms = DatasetManager.getIDListFromDataset(manager, dataset, item, ResultType.HISTOGRAM_LITERAL);
			scalarsPanel.setResultFileManager(manager);
			vectorsPanel.setResultFileManager(manager);
			histogramsPanel.setResultFileManager(manager);
			scalarsPanel.setIDList(scalars);
			vectorsPanel.setIDList(vectors);
			histogramsPanel.setIDList(histograms);
			selectedDataset = dataset;
			selectedItem = item;
			
			// set visible table
			FilteredDataPanel control = (FilteredDataPanel)layout.topControl;
			if (control.getIDList().isEmpty()) {
				control = null;
				if (!scalars.isEmpty()) control = scalarsPanel;
				if (!vectors.isEmpty()) control = vectorsPanel;
				if (!histograms.isEmpty()) control = histogramsPanel;
				if (control != null)
					setVisibleTable(control);
			}
		}
		else {
//			scalarsPanel.setIDList(EMPTY);
//			vectorsPanel.setIDList(EMPTY);
//			histogramsPanel.setIDList(EMPTY);
			selectedDataset = null;
			selectedItem = null;
		}
	}
	
	class ShowTableAction extends Action
	{
		Control table;
		
		public ShowTableAction(ResultType type) {
			switch (type.getValue()) {
			case ResultType.SCALAR:
				setText("Show scalars");
				setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SHOWSCALARS));
				table = scalarsPanel;
				break;
			case ResultType.VECTOR:
				setText("Show vectors");
				setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SHOWVECTORS));
				table = vectorsPanel;
				break;
			case ResultType.HISTOGRAM:
				setText("Show histograms");
				setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_SHOWHISTOGRAMS));
				table = histogramsPanel;
				break;
			default:
				Assert.isLegal(false, "Unknown result type: " + type);
				break;
			}
		}
		
		public void run() {
			if (layout.topControl != table) {
				layout.topControl = table;
				panel.layout();
			}
		}
	}
}
