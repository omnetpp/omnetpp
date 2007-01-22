package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.scave.actions.AddToDatasetAction;
import org.omnetpp.scave.actions.CopyToClipboardAction;
import org.omnetpp.scave.actions.CreateChartAction;
import org.omnetpp.scave.actions.CreateDatasetAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * This is the "Browse data" page of Scave Editor
 */
public class BrowseDataPage extends ScaveEditorPage {

	private Label label;
	private TabFolder tabfolder;
	private Composite buttonPanel;
	private Button createDatasetButton;
	private Button addToDatasetButton;
	private Button createChartButton;
	private Button copyToClipboardButton;
	
	private TabItem vectorsTab;
	private TabItem scalarsTab;
	private TabItem histogramsTab;
	private FilteredDataPanel vectorsPanel;
	private FilteredDataPanel scalarsPanel;
	private FilteredDataPanel histogramsPanel;
	
	private IResultFilesChangeListener fileChangeListener;
	private SelectionListener selectionChangeListener;
	
	
	public BrowseDataPage(Composite parent, ScaveEditor editor) {
		super(parent, SWT.V_SCROLL, editor);
		initialize();
		hookListeners();
	}
	
	
	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
	}


	public FilteredDataPanel getScalarsPanel() {
		return scalarsPanel;
	}
	
	public FilteredDataPanel getVectorsPanel() {
		return vectorsPanel;
	}
	
	public FilteredDataPanel getHistogramsPanel() {
		return histogramsPanel;
	}
	
	public FilteredDataPanel getActivePanel() {
		int index = tabfolder.getSelectionIndex();
		if (index >= 0)
			return (FilteredDataPanel)tabfolder.getItem(index).getControl();
		else
			return null;
	}
	
	public void setActivePanel(FilteredDataPanel panel) {
		TabItem[] items = tabfolder.getItems();
		for (int i = 0; i < items.length; ++i)
			if (items[i].getControl() == panel) {
				tabfolder.setSelection(i);
				return;
			}
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Browse data");
		setFormTitle("Browse data");
		//setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		getBody().setLayout(new GridLayout());
		label = new Label(getBody(), SWT.WRAP);
		label.setText("Here you can see all data that come from the files specified in the Inputs page.");
		label.setBackground(this.getBackground());
		createTabFolder();
		createButtonsPanel();
		
		// add actions
		IWorkbenchWindow workbenchWindow = scaveEditor.getSite().getWorkbenchWindow();
		configureGlobalButton(workbenchWindow, createDatasetButton, new CreateDatasetAction());
		configureGlobalButton(workbenchWindow, addToDatasetButton, new AddToDatasetAction());
		configureGlobalButton(workbenchWindow, createChartButton, new CreateChartAction());
		configureGlobalButton(workbenchWindow, copyToClipboardButton, new CopyToClipboardAction());
		
		// set up contents
		ResultFileManagerEx manager = scaveEditor.getResultFileManager();
		scalarsPanel.setResultFileManager(manager);
		vectorsPanel.setResultFileManager(manager);
		histogramsPanel.setResultFileManager(manager);
		scalarsPanel.setIDList(manager.getAllScalars());
		vectorsPanel.setIDList(manager.getAllVectors());
		histogramsPanel.setIDList(new IDList()); // TODO
	}
	
	private void createTabFolder() {
		tabfolder = new TabFolder(getBody(), SWT.TOP);
		//tabfolder.setBackground(new Color(null,255,255,255));
		tabfolder.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											  GridData.GRAB_VERTICAL |
											  GridData.FILL_BOTH));

		// create pages
		vectorsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_VECTOR);
		configureFilteredDataPanel(vectorsPanel);
		scalarsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_SCALAR);
		configureFilteredDataPanel(scalarsPanel);
		histogramsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_HISTOGRAM);
		configureFilteredDataPanel(histogramsPanel);
		
		ResultFileManager manager = scaveEditor.getResultFileManager();
		long numVectors = manager.getAllVectors().size();
		long numScalars = manager.getAllScalars().size();
		long numHistograms = 0; // TODO
		vectorsTab = addItem("Vectors("+numVectors+")", vectorsPanel);
		scalarsTab = addItem("Scalars("+numScalars+")", scalarsPanel);
		histogramsTab = addItem("Histograms("+numHistograms+")", histogramsPanel);
		
		tabfolder.setSelection(0);
	}
	
	private TabItem addItem(String text, Control control) {
		TabItem item = new TabItem(tabfolder, SWT.NONE);
		item.setText(text);
		item.setControl(control);
		return item;
	}
	
	private void createButtonsPanel() {
		buttonPanel = new Composite(getBody(), SWT.NONE);
		buttonPanel.setBackground(this.getBackground());
		FillLayout layout = new FillLayout();
		layout.type = SWT.HORIZONTAL;
		layout.spacing = 5;
		buttonPanel.setLayout(layout);
		createButtons();
	}
	
	private void createButtons() {
		createDatasetButton = new Button(buttonPanel, SWT.NONE);
		createDatasetButton.setText("Create dataset...");
		addToDatasetButton = new Button(buttonPanel, SWT.NONE);
		addToDatasetButton.setText("Add to dataset...");
		createChartButton = new Button(buttonPanel, SWT.NONE);
		createChartButton.setText("Create chart...");
		copyToClipboardButton = new Button(buttonPanel, SWT.NONE);
		copyToClipboardButton.setText("Copy...");
	}
	
	private void hookListeners() {
		if (fileChangeListener == null) {
			fileChangeListener = new IResultFilesChangeListener() {
				public void resultFileManagerChanged(final ResultFileManager manager) {
					getDisplay().asyncExec(new Runnable() {
						public void run() {
							IDList vectors = manager.getAllVectors();
							IDList scalars = manager.getAllScalars();
							IDList histograms = new IDList(); // TODO
							vectorsTab.setText("Vectors("+vectors.size()+")");
							scalarsTab.setText("Scalars("+scalars.size()+")");
							histogramsTab.setText("Histograms("+histograms.size()+")");
							scalarsPanel.setIDList(scalars);
							vectorsPanel.setIDList(vectors);
							histogramsPanel.setIDList(histograms);
						}
					});
				}
			};
			scaveEditor.getResultFileManager().addListener(fileChangeListener);
		}
		
		if (selectionChangeListener == null) {
			selectionChangeListener = new SelectionAdapter() {
				@Override
				public void widgetSelected(SelectionEvent e) {
					TableItem item = (TableItem)e.item;
					Object data = item.getData(DataTable.ITEM_KEY);
					if (data instanceof VectorResult) {
						scaveEditor.setSelection(new StructuredSelection(data));
					}
				}
			};
			vectorsPanel.getTable().addSelectionListener(selectionChangeListener);
		}
	}
	
	private void unhookListeners() {
		if (fileChangeListener != null) {
			ResultFileManagerEx manager = scaveEditor.getResultFileManager();
			manager.removeListener(fileChangeListener);
		}
		if (selectionChangeListener != null) {
			vectorsPanel.getTable().removeSelectionListener(selectionChangeListener);
		}
	}

	@Override
	public void pageSelected() {
		FilteredDataPanel panelToActivate = null;
		if (vectorsPanel.getTable().getItemCount() > 0)
			panelToActivate = vectorsPanel;
		else if (scalarsPanel.getTable().getItemCount() > 0)
			panelToActivate = scalarsPanel;
		else if (histogramsPanel.getTable().getItemCount() > 0)
			panelToActivate = histogramsPanel;
		if (panelToActivate != null)
			this.setActivePanel(panelToActivate);
	}
}
