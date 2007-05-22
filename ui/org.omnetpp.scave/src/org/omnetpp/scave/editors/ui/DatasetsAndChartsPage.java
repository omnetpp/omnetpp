package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.CustomSashForm;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.NewAction;
import org.omnetpp.scave.actions.OpenAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;

/**
 * Scave page which displays datasets and chart sheets.
 * @author Andras
 */
public class DatasetsAndChartsPage extends ScaveEditorPage {
	private FormToolkit formToolkit = null;  
	private Section datasetsSection = null;
	private Section chartSheetsSection = null;
	private SashForm sashform = null;

	/**
	 * Constructor
	 */
	public DatasetsAndChartsPage(Composite parent, ScaveEditor scaveEditor) {
		super(parent, SWT.V_SCROLL, scaveEditor);
		initialize();
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Datasets");
		setFormTitle("Datasets and Charts");
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.WHITE);
		getBody().setLayout(new GridLayout());
		
		//XXX experimental: toolbar for easy creation of Scave objects
		ToolBar toolbar = new ToolBar(getBody(), SWT.HORIZONTAL);
		toolbar.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		toolbar.setBackground(ColorFactory.WHITE);
		toolbar.setFont(new Font(null, "Arial", 7, SWT.NORMAL));
		new ModelObjectPalette(toolbar, scaveEditor, true);
		
		createSashForm();
		createDatasetsSection();
		createChartSheetsSection();
		sashform.setWeights(new int[] {3,2});
		
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
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit(Display.getCurrent());
		return formToolkit;
	}

	public TreeViewer getDatasetsTreeViewer() {
		DatasetsPanel panel = (DatasetsPanel)datasetsSection.getClient();
		return panel.getTreeViewer();
	}
	
	public TreeViewer getChartSheetsTreeViewer() {
		ChartSheetsPanel panel = (ChartSheetsPanel)chartSheetsSection.getClient();
		return panel.getTreeViewer();
	}
	
	private void createSashForm() {
		sashform = new CustomSashForm(getBody(), SWT.VERTICAL | SWT.SMOOTH);
		sashform.setBackground(this.getBackground());
		sashform.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	}
	
	/**
	 * This method initializes datasetsSection	
	 */
	private void createDatasetsSection() {
		datasetsSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		datasetsSection.setText("Datasets");
		datasetsSection.setDescription("Here you can browse the datasets you have created from the input.");
		//datasetsSection.setExpanded(true); XXX SWT bug: must be after setText() if present, otherwise text won't appear!
		final DatasetsPanel datasetsPanel = new DatasetsPanel(datasetsSection, SWT.NONE);
		datasetsSection.setClient(datasetsPanel);

		configureViewerButton(
				datasetsPanel.getEditNodeButton(), 
				datasetsPanel.getTreeViewer(), 
				new EditAction());
		configureViewerButton(
				datasetsPanel.getNewChildButton(),
				datasetsPanel.getTreeViewer(),
				new NewAction(scaveEditor.getAnalysis().getDatasets(), true));
		configureViewerButton(
				datasetsPanel.getNewSiblingButton(),
				datasetsPanel.getTreeViewer(),
				new NewAction(null, false));
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
		chartSheetsSection = getFormToolkit().createSection(sashform, Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setDescription("Here you can browse the charts you have created for the datasets.");
		//chartSheetsSection.setExpanded(true); XXX SWT bug: must be after setText() if present, otherwise text won't appear!
		ChartSheetsPanel chartSheetsPanel = new ChartSheetsPanel(chartSheetsSection, SWT.NONE); 
		chartSheetsSection.setClient(chartSheetsPanel);

		// configure actions
		configureViewerButton(
				chartSheetsPanel.getEditChartSheetButton(),
				chartSheetsPanel.getTreeViewer(), 
				new EditAction());
		configureViewerButton(
				chartSheetsPanel.getNewChildButton(),
				chartSheetsPanel.getTreeViewer(),
				new NewAction(scaveEditor.getAnalysis().getChartSheets(), true)); //XXX "New chart sheet"?
		configureViewerButton(
				chartSheetsPanel.getNewSiblingButton(),
				chartSheetsPanel.getTreeViewer(),
				new NewAction(null, false));
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