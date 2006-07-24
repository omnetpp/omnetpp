package org.omnetpp.scave2.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.CustomSashForm;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave2.actions.EditAction;
import org.omnetpp.scave2.actions.NewAction;
import org.omnetpp.scave2.actions.OpenAction;
import org.omnetpp.scave2.actions.RemoveAction;
import org.omnetpp.scave2.editors.ScaveEditor;

public class DatasetsAndChartsPage extends ScaveEditorPage {

	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	private Section datasetsSection = null;
	private Section chartSheetsSection = null;
	private SashForm sashform = null;

	public DatasetsAndChartsPage(Composite parent, ScaveEditor scaveEditor) {
		super(parent, SWT.V_SCROLL, scaveEditor);
		initialize();
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
		// set up UI
		setPageTitle("Datasets");
		setFormTitle("Datasets and Charts");
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout());
		createSashForm();
		createDatasetsSection();
		createChartSheetsSection();
		
		// configure viewers
		scaveEditor.configureTreeViewer(getDatasetsTreeViewer());
        scaveEditor.configureTreeViewer(getChartSheetsTreeViewer());
        
		// set contents
		Analysis analysis = scaveEditor.getAnalysis();
		getDatasetsTreeViewer().setInput(analysis.getDatasets());
        getChartSheetsTreeViewer().setInput(analysis.getChartSheets());
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

	private void createSashForm() {
		sashform = new CustomSashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(GridData.GRAB_HORIZONTAL |
											GridData.GRAB_VERTICAL |
											GridData.FILL_BOTH));
	}
	
	/**
	 * This method initializes datasetsSection	
	 */
	private void createDatasetsSection() {
		GridData gridData2 = new GridData();
		gridData2.horizontalAlignment = GridData.FILL;
		gridData2.verticalAlignment = GridData.FILL;
		gridData2.grabExcessHorizontalSpace = true;
		gridData2.grabExcessVerticalSpace = true;
		datasetsSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		datasetsSection.setExpanded(true);
		datasetsSection.setLayoutData(gridData2);
		datasetsSection.setText("Datasets");
		datasetsSection.setDescription("Here you can browse the datasets you have created from the input.");
		final DatasetsPanel datasetsPanel = new DatasetsPanel(datasetsSection, SWT.NONE);
		datasetsSection.setClient(datasetsPanel);

		configureViewerButton(
				datasetsPanel.getEditNodeButton(), 
				datasetsPanel.getTreeViewer(), 
				new EditAction());
		configureViewerButton(
				datasetsPanel.getNewNodeButton(),
				datasetsPanel.getTreeViewer(),
				new NewAction());
		configureViewerButton(
				datasetsPanel.getRemoveNodeButton(), 
				datasetsPanel.getTreeViewer(),
				new RemoveAction());
		configureViewerDefaultButton(
				datasetsPanel.getOpenDatasetButton(), 
				datasetsPanel.getTreeViewer(),
				new OpenAction());
	}

	/**
	 * This method initializes chartSheetsSection	
	 */
	private void createChartSheetsSection() {
		// set up UI
		GridData gridData3 = new GridData();
		gridData3.horizontalAlignment = GridData.FILL;
		gridData3.verticalAlignment = GridData.FILL;
		gridData3.grabExcessHorizontalSpace = true;
		chartSheetsSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setExpanded(true);
		chartSheetsSection.setLayoutData(gridData3);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setDescription("Here you can browse the charts you have created for the datasets.");
		ChartSheetsPanel chartSheetsPanel = new ChartSheetsPanel(chartSheetsSection, SWT.NONE); 
		chartSheetsSection.setClient(chartSheetsPanel);

		// configure actions
		configureViewerButton(
				chartSheetsPanel.getEditChartSheetButton(),
				chartSheetsPanel.getTreeViewer(), 
				new EditAction());
		configureViewerButton(
				chartSheetsPanel.getNewChartSheetButton(),
				chartSheetsPanel.getTreeViewer(),
				new NewAction()); //XXX "New chart sheet"?
		configureViewerButton(
				chartSheetsPanel.getRemoveChartSheetButton(), 
				chartSheetsPanel.getTreeViewer(),
				new RemoveAction());
		configureViewerDefaultButton(
				chartSheetsPanel.getOpenChartSheetButton(), 
				chartSheetsPanel.getTreeViewer(),
				new OpenAction());
	}
}