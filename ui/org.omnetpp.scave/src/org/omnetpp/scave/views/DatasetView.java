package org.omnetpp.scave.views;

import static org.omnetpp.scave.TestSupport.DATASET_VIEW_HISTOGRAMS_PANEL_ID;
import static org.omnetpp.scave.TestSupport.DATASET_VIEW_MAIN_PANEL_ID;
import static org.omnetpp.scave.TestSupport.DATASET_VIEW_SCALARS_PANEL_ID;
import static org.omnetpp.scave.TestSupport.DATASET_VIEW_VECTORS_PANEL_ID;
import static org.omnetpp.scave.TestSupport.WIDGET_ID;
import static org.omnetpp.scave.TestSupport.enableGuiTest;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.scave.actions.SetFilterAction2;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.ChooseTableColumnsAction;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelProvider;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ChartLine;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * View that shows the content of a dataset.
 * 
 * @author tomi
 */
// work in progress ...
// TODO add icon
public class DatasetView extends ViewWithMessagePart implements ISelectionProvider	{

	public static final String ID = "org.omnetpp.scave.DatasetView";
	
	private Composite panel;
	private StackLayout layout;
	private FilteredDataPanel vectorsPanel;
	private FilteredDataPanel scalarsPanel;
	private FilteredDataPanel histogramsPanel;
	
	private boolean viewControlCreated;
	
	private ScaveEditor activeScaveEditor;
	private Dataset selectedDataset;
	private DatasetItem selectedItem;
	
	private IAction showScalarsAction;
	private IAction showVectorsAction;
	private IAction showHistogramsAction;
	private IAction selectAllAction;
	private IAction toggleFilterAction;
	private SetFilterAction2 setFilterAction;
	
	private ISelectionListener selectionChangedListener;
	private IPartListener partListener;
	private IResultFilesChangeListener resultFilesChangeListener;
	private INotifyChangedListener modelChangeListener;
	
	private ILabelProvider labelProvider;
	
	private ISelection selection;
	private ListenerList selectionChangeListeners = new ListenerList();
	
	private static final IDList EMPTY = new IDList();
	
	
	
	@Override
	public void init(IViewSite site) throws PartInitException {
		super.init(site);
		site.setSelectionProvider(this);
	}


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
		vectorsPanel = configurePanel(new FilteredDataPanel(panel, SWT.NONE, ResultType.VECTOR_LITERAL));
		scalarsPanel = configurePanel(new FilteredDataPanel(panel, SWT.NONE, ResultType.SCALAR_LITERAL));
		histogramsPanel = configurePanel(new FilteredDataPanel(panel, SWT.NONE, ResultType.HISTOGRAM_LITERAL));
		
		setVisibleDataPanel(vectorsPanel);
		showFilter(false);

		viewControlCreated = true;
		
		if (enableGuiTest) {
			panel.setData(WIDGET_ID, DATASET_VIEW_MAIN_PANEL_ID);
			scalarsPanel.setData(WIDGET_ID, DATASET_VIEW_SCALARS_PANEL_ID);
			vectorsPanel.setData(WIDGET_ID, DATASET_VIEW_VECTORS_PANEL_ID);
			histogramsPanel.setData(WIDGET_ID, DATASET_VIEW_HISTOGRAMS_PANEL_ID);
		}
		
		return panel;
	}
	
	private FilteredDataPanel configurePanel(FilteredDataPanel panel) {
		panel.getTable().addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				DataTable table = (DataTable)e.widget;
				if (table.getSelectionCount() == 0)
					setSelection(StructuredSelection.EMPTY);
				else {
					IDListSelection selection = new IDListSelection(table.getSelectedIDs(), table.getResultFileManager());
					setSelection(selection);
				}
			}
		});
		return panel;
	}
	
	private void initActions() {
		showScalarsAction = new ShowTableAction(ResultType.SCALAR_LITERAL);
		showVectorsAction = new ShowTableAction(ResultType.VECTOR_LITERAL);
		showHistogramsAction = new ShowTableAction(ResultType.HISTOGRAM_LITERAL);
		toggleFilterAction = new ToggleFilterAction();
		toggleFilterAction.setChecked(isFilterVisible());
		setFilterAction = new SetFilterAction2();
		selectAllAction = new SelectAllAction();
		IActionBars actionBars = getViewSite().getActionBars();
		actionBars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
	}
	
	private void createToolbarButtons() {
		IToolBarManager manager = getViewSite().getActionBars().getToolBarManager();
		manager.add(toggleFilterAction);
		manager.add(new Separator());
		manager.add(showScalarsAction);
		manager.add(showVectorsAction);
		manager.add(showHistogramsAction);
		
		showVectorsAction.setChecked(true);
	}
	
	private void createContextMenu(final FilteredDataPanel panel) {
		DataTable table = panel.getTable();
		IMenuManager menuManager = table.getContextMenuManager();
		menuManager.add(new ChooseTableColumnsAction(table));
		menuManager.add(setFilterAction);
		table.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				setFilterAction.update(panel);
			}
		});
		// XXX call getSite().registerContexMenu() ?
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

			public void partClosed(IWorkbenchPart part) {
				if (part == activeScaveEditor)
					activeEditorChanged(null);
			}

			public void partBroughtToTop(IWorkbenchPart part) {}
			public void partDeactivated(IWorkbenchPart part) {}
			public void partOpened(IWorkbenchPart part) {}
		};
		getSite().getPage().addPartListener(partListener);
		
		activeEditorChanged(getActiveEditor());
	}
	
	private void hookEditorListeners(IEditorPart editor) {
		if (editor instanceof ScaveEditor) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			scaveEditor.getResultFileManager().addChangeListener(resultFilesChangeListener =
				new IResultFilesChangeListener() {
				public void resultFileManagerChanged(ResultFileManager manager) {
					Display.getDefault().asyncExec(new Runnable() {
						public void run() {
							updateDataTable();
						}
					});
				}
			});
			
			IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
			notifier.addListener(modelChangeListener = new INotifyChangedListener() {
				public void notifyChanged(Notification notification) {
					Display.getDefault().asyncExec(new Runnable() {
						public void run() {
							updateDataTable();
						}
					});
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
				scaveEditor.getResultFileManager().removeChangeListener(resultFilesChangeListener);
	
			if (modelChangeListener != null) {
				IChangeNotifier notifier = (IChangeNotifier)scaveEditor.getAdapterFactory();
				notifier.removeListener(modelChangeListener);
			}
		}
	}
	
	private FilteredDataPanel getVisibleDataPanel() {
		return layout == null ? null : (FilteredDataPanel)layout.topControl;
	}
	
	private ResultType getVisibleResultType() {
		FilteredDataPanel panel = getVisibleDataPanel();
		return (panel == scalarsPanel ? ResultType.SCALAR_LITERAL :
				panel == vectorsPanel ? ResultType.VECTOR_LITERAL :
				panel == histogramsPanel ? ResultType.HISTOGRAM_LITERAL :
				null);
	}
	
	private FilteredDataPanel getFilteredDataPanel(ResultItem item) {
		if (item instanceof ScalarResult)
			return scalarsPanel;
		else if (item instanceof VectorResult)
			return vectorsPanel;
		else if (item instanceof HistogramResult)
			return histogramsPanel;
		else
			return null;
	}
	
	private void setVisibleDataPanel(FilteredDataPanel table) {
		if (layout.topControl != table) {
			if (setFilterAction != null)
				setFilterAction.update(table);
			layout.topControl = table;
			panel.layout();
			updateContentDescription();
		}
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
		Dataset dataset = null;
		DatasetItem item = null;
		long id = -1L;
		ResultFileManager manager = null;
		
		if (selection == this.selection)
			return;
		
		if (selection instanceof IStructuredSelection &&
				((IStructuredSelection)selection).getFirstElement() instanceof EObject) {
			EObject selected = (EObject)((IStructuredSelection)selection).getFirstElement();
			dataset = ScaveModelUtil.findEnclosingOrSelf(selected, Dataset.class);
			item = ScaveModelUtil.findEnclosingOrSelf(selected, DatasetItem.class);
			setInput(dataset, item);
		}
		else if (selection instanceof IStructuredSelection) {
			Object selectedObject = ((IStructuredSelection)selection).getFirstElement();
			if (selectedObject instanceof ChartLine) {
				ChartLine selectedLine = (ChartLine)selectedObject;
				item = selectedLine.getChart();
				dataset = ScaveModelUtil.findEnclosingDataset(item);
				id = selectedLine.getResultItemID();
				manager = selectedLine.getResultFileManager();
			}
		}
		
		if (dataset != null) {
			setInput(dataset, item);
			if (id != -1L && manager != null)
				selectResultItem(id, manager);
		}
		else {
			setInput(null, null);
			showMessage("No dataset item selected.");
		}
	}
	
	private void selectResultItem(long id, ResultFileManager manager) {
		ResultItem item = manager.getItem(id);
		FilteredDataPanel panel = getFilteredDataPanel(item);
		if (panel != null && !panel.isDisposed()) {
			DataTable table = panel.getTable();
			if (!table.isDisposed()) {
				table.setSelectionByID(id);
			}
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
				labelProvider = new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(activeScaveEditor.getAdapterFactory()));
				hideMessage();
			}
			else {
				scalarsPanel.setResultFileManager(null);
				vectorsPanel.setResultFileManager(null);
				histogramsPanel.setResultFileManager(null);
				labelProvider = null;
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
			updateContentDescription();
		}
	}
	
	private void updateContentDescription() {
		if (labelProvider != null && selectedDataset != null) {
			ResultType visibleType = getVisibleResultType();
			String type = (visibleType == ResultType.SCALAR_LITERAL ? "scalars" :
				 		   visibleType == ResultType.VECTOR_LITERAL ? "vectors" :
				 		   visibleType == ResultType.HISTOGRAM_LITERAL ? "histograms" : null);

			String desc = "";
			if (type != null)
				desc += type;
			if (selectedItem != null) {
				if (desc.length() > 0) desc += " at ";
				desc += "'" + labelProvider.getText(selectedItem) + "'";
			}
			if (desc.length() > 0)
				desc += " from ";
			else
				desc += "content of";
			desc += labelProvider.getText(selectedDataset);
			hideMessage();
			setContentDescription(desc);
		}
		else
			showMessage("No dataset item selected.");
	}
	
	/**
	 * Prevent calling showMessage until the view fully created.
	 */
	@Override
	protected void showMessage(String text) {
		if (viewControlCreated)
			super.showMessage(text);
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
	
	private void showFilter(boolean show) {
		vectorsPanel.showFilterPanel(show);
		scalarsPanel.showFilterPanel(show);
		histogramsPanel.showFilterPanel(show);
	}
	
	private boolean isFilterVisible() {
		return vectorsPanel.isFilterPanelVisible();
	}
	
	/*
	 * Actions
	 */
	
	class ShowTableAction extends Action
	{
		FilteredDataPanel table;
		
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
			setVisibleDataPanel(table);
		}
	}
	
	class ToggleFilterAction extends Action
	{
		public ToggleFilterAction() {
			super(isFilterVisible() ? "Hide filter" : "Show filter", IAction.AS_CHECK_BOX);
			setDescription("Toggles the filtering panel on/off.");
			setImageDescriptor(ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_FILTER));
		}

		@Override
		public void run() {
			boolean visible = isFilterVisible();
			showFilter(!visible);
			setText(visible ? "Show filter" : "Hide filter");
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
	
	/*
	 * ISelectionProvider
	 */

	public void addSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangeListeners.add(listener);
	}

	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		selectionChangeListeners.remove(listener);
	}
	
	protected void fireSelectionChangeEvent(ISelection selection) {
		SelectionChangedEvent event = new SelectionChangedEvent(this, selection);
		for (Object listener : selectionChangeListeners.getListeners())
			((ISelectionChangedListener)listener).selectionChanged(event);
	}

	public ISelection getSelection() {
		return selection;
	}

	public void setSelection(ISelection selection) {
		if (selection != this.selection) {
			this.selection = selection;
			fireSelectionChangeEvent(selection);
		}
	}
}
