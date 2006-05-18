package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;

public class BrowseDataPage extends ScrolledForm {

	Label label;
	CTabFolder ctabfolder;
	Composite buttonPanel;
	Button createDatasetButton;
	Button addToDatasetButton;
	Button createChartButton;
	Button copyToClipboardButton;
	
	public BrowseDataPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL);
		initialize();
	}
	
	private void initialize() {
		setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		label = new Label(getBody(), SWT.WRAP);
		label.setText("Here you can see all data that come from the input files specified in the overview page.");
		label.setBackground(this.getBackground());
		createTabFolder();
		createButtonsPanel();
	}
	
	private void createTabFolder() {
		ctabfolder = new CTabFolder(getBody(), SWT.TOP | SWT.FLAT);
		//ctabfolder.setBackground(this.getBackground());
		ctabfolder.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											  GridData.GRAB_VERTICAL |
											  GridData.FILL_BOTH));
		createPages();
		ctabfolder.setSelection(0);
	}
	
	private void createPages() {
		createVectorsPage();
		createScalarsPage();
		createHistogramsPage();
	}
	
	private void createVectorsPage() {
		addItem("Vectors", new VectorsPanel(ctabfolder, SWT.NONE));
	}
	
	private void createScalarsPage() {
		addItem("Scalars", new ScalarsPanel(ctabfolder, SWT.NONE));
	}
	
	private void createHistogramsPage() {
		addItem("Histograms", new ScalarsPanel(ctabfolder, SWT.NONE));
	}
	
	private void addItem(String text, Control control) {
		CTabItem item = new CTabItem(ctabfolder, SWT.NONE);
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
