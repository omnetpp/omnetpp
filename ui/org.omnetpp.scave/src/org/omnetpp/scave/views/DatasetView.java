package org.omnetpp.scave.views;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
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
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IActionFilter;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.ChooseTableColumnsAction;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
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
// TODO add icon
public class DatasetView extends ViewWithMessagePart {

	public static final String ID = "org.omnetpp.scave.DatasetView";
	
	private Composite panel;
	private StackLayout layout;
	private FilteredDataPanel vectorsPanel;
	private FilteredDataPanel scalarsPanel;
	private FilteredDataPanel histogramsPanel;
	
	private ScaveEditor activeScaveEditor;
	private Dataset selectedDataset;
	private DatasetItem selectedItem;
	
	private IAction showScalarsAction;
	private IAction showVectorsAction;
	private IAction showHistogramsAction;
	private IAction selectAllAction;
	
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;
	private IResultFilesChangeListener resultFilesChangeListener;
	private INotifyChangedListener modelChangeListener;
	
	private static final IDList EMPTY = new IDList();
	
	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);
		initActions();
		hookPageListeners();
		createToolbarButtons();
		createContextMenu(vectorsPanel);
		createContextMenu(scalarsPanel);
		createContextMenu(histogramsPanel);
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
		vectorsPanel = new FilteredDataPanel(panel, SWT.NONE, ResultType.VECTOR_LITERAL);
		scalarsPanel = new FilteredDataPanel(panel, SWT.NONE, ResultType.SCALAR_LITERAL);
		histogramsPanel = new FilteredDataPanel(panel, SWT.NONE, ResultType.HISTOGRAM_LITERAL);
		
		setVisibleDataPanel(vectorsPanel);
		
		return panel;
	}
	
	private void initActions() {
		showScalarsAction = new ShowTableAction(ResultType.SCALAR_LITERAL);
		showVectorsAction = new ShowTableAction(ResultType.VECTOR_LITERAL);
		showHistogramsAction = new ShowTableAction(ResultType.HISTOGRAM_LITERAL);
		selectAllAction = new SelectAllAction();
		IActionBars actionBars = getViewSite().getActionBars();
		actionBars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
	}
	
	private void createToolbarButtons() {
		IToolBarManager manager = getViewSite().getActionBars().getToolBarManager();
		manager.add(showScalarsAction);
		manager.add(showVectorsAction);
		manager.add(showHistogramsAction);
		
		showVectorsAction.setChecked(true);
	}
	
	private void createContextMenu(FilteredDataPanel panel) {
		DataTable table = panel.getTable();
		table.getContextMenuManager().add(new ChooseTableColumnsAction(table));
	}
	
	@Override
	public void dispose() {
		unhookPageListeners();
		super.dispose();
	}

	private void hookPageListeners() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				workbechSelectionChanged(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	
		partListener = new IPartListener() {
			public void partActivated(IWorkbenchPart part) {
				if (part instanceof ScaveEditor)
					activeEditorChanged((ScaveEditor)part);
				else if (part instanceof IEditorPart)
					activeEditorChanged(null);
			}

			public void partBroughtToTop(IWorkbenchPart part) {
			}

			public void partClosed(IWorkbenchPart part) {
				if (part == activeScaveEditor)
					activeEditorChanged(null);
			}

			public void partDeactivated(IWorkbenchPart part) {
			}

			public void partOpened(IWorkbenchPart part) {
			}
		};
		getSite().getPage().addPartListener(partListener);
		
		activeEditorChanged(getActiveEditor());
	}
	
	private void hookEditorListeners(IEditorPart editor) {
		if (editor instanceof ScaveEditor) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			scaveEditor.getResultFileManager().addListener(resultFilesChangeListener =
				new IResultFilesChangeListener() {
				public void resultFileManagerChanged(ResultFileManager manager) {
					updateDataTable();
				}
			});
			
			IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
			notifier.addListener(modelChangeListener = new INotifyChangedListener() {
				public void notifyChanged(Notification notification) {
					updateDataTable();
				}
			});
		}
	}
	
	private void unhookPageListeners() {
		IWorkbenchPage page = getSite().getPage();
		if (selectionChangedListener != null) {
			page.removePostSelectionListener(selectionChangedListener);
			selectionChangedListener = null;
		}
		if (partListener != null) {
			page.removePartListener(partListener);
			partListener = null;
		}
	}
	
	private void unhookEditorListeners(IEditorPart editor) {
		if (editor instanceof ScaveEditor) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			if (resultFilesChangeListener != null)
				scaveEditor.getResultFileManager().removeListener(resultFilesChangeListener);
	
			if (modelChangeListener != null) {
				IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
				notifier.removeListener(modelChangeListener);
			}
		}
	}
	
	private FilteredDataPanel getVisibleDataPanel() {
		return layout == null ? null : (FilteredDataPanel)layout.topControl;
	}
	
	private void setVisibleDataPanel(FilteredDataPanel table) {
		layout.topControl = table;
		panel.layout();
	}
	
	private void setSelectedAction(IAction action) {
		action.setChecked(true);
		if (action == showScalarsAction) {
			showVectorsAction.setChecked(false);
			showHistogramsAction.setChecked(false);
		}
		else if (action == showVectorsAction) {
			showScalarsAction.setChecked(false);
			showHistogramsAction.setChecked(false);
		}
		else if (action == showHistogramsAction) {
			showScalarsAction.setChecked(false);
			showVectorsAction.setChecked(false);
		}
	}
	
	private void workbechSelectionChanged(ISelection selection) {
		if (selection instanceof IStructuredSelection &&
				((IStructuredSelection)selection).getFirstElement() instanceof EObject) {
			EObject selected = (EObject)((IStructuredSelection)selection).getFirstElement();
			Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(selected, Dataset.class);
			DatasetItem item = ScaveModelUtil.findEnclosingOrSelf(selected, DatasetItem.class);
			setInput(dataset, item);
			hideMessage();
		}
		else {
			setInput(null, null);
			showMessage("No dataset item selected.");
		}
	}
	
	private void activeEditorChanged(IEditorPart editor) {
		if (editor != activeScaveEditor) {
			unhookEditorListeners(activeScaveEditor);
			activeScaveEditor = editor instanceof ScaveEditor ? (ScaveEditor)editor : null;
			hookEditorListeners(activeScaveEditor);
			
			if (activeScaveEditor != null) {
				ResultFileManager manager = activeScaveEditor.getResultFileManager();
				scalarsPanel.setResultFileManager(manager);
				vectorsPanel.setResultFileManager(manager);
				histogramsPanel.setResultFileManager(manager);
				hideMessage();
			}
			else {
				scalarsPanel.setResultFileManager(null);
				vectorsPanel.setResultFileManager(null);
				histogramsPanel.setResultFileManager(null);
				showMessage("No active scave editor.");
			}
		}
	}
	
	/**
	 * Make the first table containing items visible.
	 */
	private void switchToNonEmptyTable() {
		FilteredDataPanel control = getVisibleDataPanel();
		IAction action = null;
		if (control == null || control.getIDList().isEmpty()) {
			control = null;
			if (!scalarsPanel.getIDList().isEmpty()) { control = scalarsPanel; action = showScalarsAction; }
			if (!vectorsPanel.getIDList().isEmpty()) { control = vectorsPanel; action = showVectorsAction; }
			if (!histogramsPanel.getIDList().isEmpty()) { control = histogramsPanel; action = showHistogramsAction; }
			if (control != null)
				setVisibleDataPanel(control);
			if (action != null)
				setSelectedAction(action);
		}
		
	}
	
	private void setInput(Dataset dataset, DatasetItem item) {
		if (selectedDataset != dataset || selectedItem != item) {
			selectedDataset = dataset;
			selectedItem = item;
			updateDataTable();
			switchToNonEmptyTable();
		}
	}
	
	private void updateDataTable() {
		if (activeScaveEditor != null && selectedDataset != null) {
			ResultFileManager manager = activeScaveEditor.getResultFileManager();
			IDList scalars = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedItem, ResultType.SCALAR_LITERAL);
			IDList vectors = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedItem, ResultType.VECTOR_LITERAL);
			IDList histograms = DatasetManager.getIDListFromDataset(manager, selectedDataset, selectedItem, ResultType.HISTOGRAM_LITERAL);
			scalarsPanel.setIDList(scalars);
			vectorsPanel.setIDList(vectors);
			histogramsPanel.setIDList(histograms);
		}
		else {
			scalarsPanel.setIDList(EMPTY);
			vectorsPanel.setIDList(EMPTY);
			histogramsPanel.setIDList(EMPTY);
		}
	}
	
	class ShowTableAction extends Action
	{
		Control table;
		
		public ShowTableAction(ResultType type) {
			super(null, IAction.AS_RADIO_BUTTON);

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
	
	class SelectAllAction extends Action
	{
		@Override
		public void run() {
			FilteredDataPanel panel = getVisibleDataPanel();
			if (panel != null && panel.getTable() != null) {
				panel.getTable().setFocus();
				panel.getTable().selectAll();
			}
		}
	}
}
