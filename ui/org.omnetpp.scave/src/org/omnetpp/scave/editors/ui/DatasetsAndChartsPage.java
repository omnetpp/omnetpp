package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.ExpandableComposite;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.Section;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.CustomSashForm;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;

/**
 * Scave page which displays datasets and chart sheets.
 * @author Andras
 */
public class DatasetsAndChartsPage extends ScaveEditorPage {
	private static final Color PALETTE_BG_COLOR = new Color(null, 234, 240, 252); // very light blue
	private static final Color BUTTONS_BG_COLOR = new Color(null, 239, 244, 253); // very light blue
	
	private FormToolkit formToolkit = null;  
	private TreeViewer datasetsTreeViewer;
	private TreeViewer chartSheetsTreeViewer;
	private SashForm sashform;

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
		getBody().setLayout(new GridLayout(2, false));

		createSashForm();
		createDatasetsSection();
		createChartSheetsSection();
		sashform.setWeights(new int[] {2,1});

		Composite toolbar = new Composite(getBody(), SWT.BORDER);
		toolbar.setLayout(new RowLayout(SWT.VERTICAL));
		toolbar.setLayoutData(new GridData(SWT.END, SWT.FILL, false, true));
		toolbar.setBackground(PALETTE_BG_COLOR);
		new ModelObjectPalette2(toolbar, scaveEditor, BUTTONS_BG_COLOR, true);
		
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
		return datasetsTreeViewer;
	}
	
	public TreeViewer getChartSheetsTreeViewer() {
		return chartSheetsTreeViewer;
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
		Section datasetsSection = getFormToolkit().createSection(sashform,
				Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		datasetsSection.setText("Datasets");
		datasetsSection.setDescription("Here you can browse the datasets you have created from the input.");
		//datasetsSection.setExpanded(true); XXX SWT bug: must be after setText() if present, otherwise text won't appear!
		datasetsTreeViewer = new TreeViewer(formToolkit.createTree(datasetsSection, SWT.BORDER | SWT.MULTI));
		datasetsSection.setClient(datasetsTreeViewer.getTree());
	}

	/**
	 * This method initializes chartSheetsSection	
	 */
	private void createChartSheetsSection() {
		// set up UI
		Section chartSheetsSection = getFormToolkit().createSection(sashform, Section.DESCRIPTION | ExpandableComposite.TITLE_BAR);
		chartSheetsSection.setText("Chart sheets and charts");
		chartSheetsSection.setDescription("Here you can browse the charts you have created for the datasets.");
		//chartSheetsSection.setExpanded(true); XXX SWT bug: must be after setText() if present, otherwise text won't appear!
		chartSheetsTreeViewer = new TreeViewer(formToolkit.createTree(chartSheetsSection, SWT.BORDER | SWT.MULTI));
		chartSheetsSection.setClient(chartSheetsTreeViewer.getTree());
	}
}


