package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;

/**
 * This is the "Browse data" page of Scave Editor
 */
//XXX tables and filter should include the experiment, measurement, replication fields as well
//XXX filter should include expressions ("where load>10")
//XXX make filter panel foldable?
public class BrowseDataPage extends ScrolledForm {

	Label label;
	TabFolder tabfolder;
	Composite buttonPanel;
	Button createDatasetButton;
	Button addToDatasetButton;
	Button createChartButton;
	Button copyToClipboardButton;
	
	VectorsPanel vectorsPanel;
	ScalarsPanel scalarsPanel;
	
	public BrowseDataPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);
		initialize();
	}
	
	public ScalarsPanel getScalarsPanel() {
		return scalarsPanel;
	}
	
	public VectorsPanel getVectorsPanel() {
		return vectorsPanel;
	}
	
	public FilterPanel getActivePanel() {
		int index = tabfolder.getSelectionIndex();
		if (index >= 0)
			return (FilterPanel)tabfolder.getItem(index).getControl();
		else
			return null;
	}
	
//	public Button getCreateDatasetButton() {
//		return createDatasetButton;
//	}
//	
//	public Button getAddToDatasetButton() {
//		return addToDatasetButton;
//	}
//	
//	public Button getCreateChartButton() {
//		return createChartButton;
//	}
//	
//	public Button getCopyToClipboardButton() {
//		return copyToClipboardButton;
//	}
	
	public TableViewer getScalarsTableViewer() {
		return scalarsPanel.getTableViewer();
	}
	
	public TableViewer getVectorsTableViewer() {
		return vectorsPanel.getTableViewer();
	}
	
	private void initialize() {
		//setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		label = new Label(getBody(), SWT.WRAP);
		label.setText("Here you can see all data that come from the input files specified in the Inputs page.");
		label.setBackground(this.getBackground());
		createTabFolder();
		createButtonsPanel();
		
		// add actions
	}
	
	private void createTabFolder() {
		tabfolder = new TabFolder(getBody(), SWT.TOP);
		//tabfolder.setBackground(new Color(null,255,255,255));
		tabfolder.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											  GridData.GRAB_VERTICAL |
											  GridData.FILL_BOTH));
		createPages();
		tabfolder.setSelection(0);
	}
	
	private void createPages() {
		createVectorsPage();
		createScalarsPage();
		createHistogramsPage();
	}
	
	private void createVectorsPage() {
		vectorsPanel = new VectorsPanel(tabfolder, SWT.NONE);
		addItem("Vectors", vectorsPanel);
	}
	
	private void createScalarsPage() {
		scalarsPanel = new ScalarsPanel(tabfolder, SWT.NONE);
		addItem("Scalars", scalarsPanel);
	}
	
	private void createHistogramsPage() {
		addItem("Histograms", new ScalarsPanel(tabfolder, SWT.NONE));
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
