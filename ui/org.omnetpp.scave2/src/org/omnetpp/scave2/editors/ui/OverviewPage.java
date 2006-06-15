package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.actions.OpenChartSheetActionDelegate;
import org.omnetpp.scave2.actions.OpenDatasetActionDelegate;
import org.omnetpp.scave2.editors.ScaveEditor;

public class OverviewPage extends ScrolledForm {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section inputFilesSection = null;
	private Section dataSection = null;
	private Section datasetsSection = null;
	private Section chartSheetsSection = null;
	private ScaveEditor scaveEditor = null;  // the containing editor

	public OverviewPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style | SWT.V_SCROLL);
		this.scaveEditor = scaveEditor;
		initialize();
	}
	
	public TreeViewer getInputFilesTreeViewer() {
		InputFilesPanel panel = (InputFilesPanel)inputFilesSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getPhysicalDataTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getPhysicalTreeViewer();
	}
	
	public TreeViewer getLogicalDataTreeViewer() {
		DataPanel panel = (DataPanel)dataSection.getClient();
		return panel.getLogicalTreeViewer();
	}
	
	public TreeViewer getDatasetsTreeViewer() {
		DatasetsPanel panel = (DatasetsPanel)datasetsSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getChartSheetsTreeViewer() {
		ChartSheetsPanel panel = (ChartSheetsPanel)chartSheetsSection.getClient();
		return panel.getTreeViewer();
	}
	
	

	private void initialize() {
		setExpandHorizontal(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createInputFilesSection();
		createDataSection();
		createDatasetsSection();
		createChartSheetsSection();
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null) {
			formToolkit = new FormToolkit(Display.getCurrent());
		}
		return formToolkit;
	}

	/**
	 * This method initializes inputFilesSection	
	 *
	 */
	private void createInputFilesSection() {
		GridData gridData = new GridData();
		gridData.grabExcessHorizontalSpace = true;
		gridData.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		inputFilesSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		inputFilesSection.setExpanded(true);
		inputFilesSection.setLayoutData(gridData);
		inputFilesSection.setText("Input files");
		inputFilesSection.setDescription("Add or drag&drop output files that should by used in this analysis.");
		InputFilesPanel inputFilesPanel = new InputFilesPanel(inputFilesSection, SWT.NONE);
		inputFilesSection.setClient(inputFilesPanel);

		final TreeViewer treeViewer = inputFilesPanel.getTreeViewer();

		// configure Add button
		//TODO

		// configure Add Wildcard button
		//TODO

		// configure Edit button
//		final Button editButton = inputFilesPanel.getEditButton();
//		scaveEditor.configureEditButton(editButton, treeViewer);

		// configure Remove button
		final Button removeButton = inputFilesPanel.getRemoveFileButton();
		scaveEditor.configureRemoveButton(removeButton, treeViewer);
	}

	/**
	 * This method initializes dataSection	
	 *
	 */
	private void createDataSection() {
		GridData gridData1 = new GridData();
		gridData1.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		dataSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		dataSection.setExpanded(true);
		dataSection.setLayoutData(gridData1);
		dataSection.setText("Data");
		dataSection.setDescription("Here you can browse all data (vectors, scalars and histograms) that come from the input files.");
		dataSection.setClient(new DataPanel(dataSection, SWT.NONE));
	}

	/**
	 * This method initializes datasetsSection	
	 */
	private void createDatasetsSection() {
		GridData gridData2 = new GridData();
		gridData2.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		datasetsSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		datasetsSection.setExpanded(true);
		datasetsSection.setLayoutData(gridData2);
		datasetsSection.setText("Datasets");
		datasetsSection.setDescription("Here you can browse the datasets you have created from the input.");
		final DatasetsPanel datasetsPanel = new DatasetsPanel(datasetsSection, SWT.NONE);
		datasetsSection.setClient(datasetsPanel);

		final TreeViewer treeViewer = datasetsPanel.getTreeViewer();

		// configure Open dataset button
		Button openButton = datasetsPanel.getOpenDatasetButton();
		scaveEditor.configureButton(openButton, new OpenDatasetActionDelegate());
		
		// configure New button
		//TODO
		
		// configure Edit button
		final Button editButton = datasetsPanel.getEditNodeButton();
		scaveEditor.configureEditButton(editButton, treeViewer);
		
		// configure Remove button
		final Button removeButton = datasetsPanel.getRemoveNodeButton();
		scaveEditor.configureRemoveButton(removeButton, treeViewer);
	}

	/**
	 * This method initializes chartSheetsSection	
	 */
	private void createChartSheetsSection() {
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = org.eclipse.swt.layout.GridData.FILL;
		chartSheetsSection = getFormToolkit().createSection(
				getBody(),
				ExpandableComposite.TWISTIE | Section.DESCRIPTION
						| ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setExpanded(true);
		chartSheetsSection.setLayoutData(gridData3);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setDescription("Here you can browse the charts you have created for the datasets.");
		ChartSheetsPanel chartSheetsPanel = new ChartSheetsPanel(chartSheetsSection, SWT.NONE); 
		chartSheetsSection.setClient(chartSheetsPanel);

		final TreeViewer treeViewer = chartSheetsPanel.getTreeViewer();

		// configure Open button
		final Button openButton = chartSheetsPanel.getOpenChartSheetButton();
		ScaveEditor.disableButtonOnSelectionContent(openButton, treeViewer, ScaveModelFactory.eINSTANCE.createChartSheet().eClass());
		scaveEditor.configureButton(openButton, new OpenChartSheetActionDelegate());
		// configure Edit button
		final Button editButton = chartSheetsPanel.getEditChartSheetButton();
		scaveEditor.configureEditButton(editButton, treeViewer);

		// configure Remove button
		final Button removeButton = chartSheetsPanel.getRemoveChartSheetButton();
		scaveEditor.configureRemoveButton(removeButton, treeViewer);
	}
}
