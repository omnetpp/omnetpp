package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave2.actions.AddToDatasetAction;
import org.omnetpp.scave2.actions.CopyToClipboardAction;
import org.omnetpp.scave2.actions.CreateChartAction;
import org.omnetpp.scave2.actions.CreateDatasetAction;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.DataTable;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;

/**
 * This is the "Browse data" page of Scave Editor
 */
//XXX tables and filter should include the experiment, measurement, replication fields as well
//XXX filter should include expressions ("where load>10")
//XXX make filter panel foldable?
public class BrowseDataPage extends ScaveEditorPage {

	private Label label;
	private TabFolder tabfolder;
	private Composite buttonPanel;
	private Button createDatasetButton;
	private Button addToDatasetButton;
	private Button createChartButton;
	private Button copyToClipboardButton;
	
	private FilteredDataPanel vectorsPanel;
	private FilteredDataPanel scalarsPanel;
	private FilteredDataPanel histogramsPanel;
	
	public BrowseDataPage(Composite parent, ScaveEditor editor) {
		super(parent, SWT.V_SCROLL, editor);
		initialize();
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
		scalarsPanel.setIDList(manager.getAllScalars());
		vectorsPanel.setIDList(manager.getAllVectors());
		
		// stay up to date
		manager.addListener(new IResultFilesChangeListener() {
			public void idlistChanged(IDList idlist) {
			}
			public void resultFileManagerChanged(ResultFileManager manager) {
				scalarsPanel.setIDList(manager.getAllScalars());
				vectorsPanel.setIDList(manager.getAllVectors());
			}
		});
	}
	
	private void createTabFolder() {
		tabfolder = new TabFolder(getBody(), SWT.TOP);
		//tabfolder.setBackground(new Color(null,255,255,255));
		tabfolder.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											  GridData.GRAB_VERTICAL |
											  GridData.FILL_BOTH));

		// create pages
		vectorsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_VECTOR);
		scalarsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_SCALAR);
		histogramsPanel = new FilteredDataPanel(tabfolder, SWT.NONE, DataTable.TYPE_HISTOGRAM);
		addItem("Vectors", vectorsPanel);
		addItem("Scalars", scalarsPanel);
		addItem("Histograms", histogramsPanel);
		
		tabfolder.setSelection(0);
	}
	
	private void addItem(String text, Control control) {
		TabItem item = new TabItem(tabfolder, SWT.NONE);
		item.setText(text);
		item.setControl(control);
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
}
