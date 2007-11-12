package org.omnetpp.scave.editors.ui;

import static org.omnetpp.scave.TestSupport.CHARTSHEETS_TREE_ID;
import static org.omnetpp.scave.TestSupport.DATASETS_PAGE_PALETTE_ID;
import static org.omnetpp.scave.TestSupport.DATASETS_TREE_ID;
import static org.omnetpp.scave.TestSupport.WIDGET_ID;
import static org.omnetpp.scave.TestSupport.enableGuiTest;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.provider.DelegatingWrapperItemProvider;
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
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Scave page which displays datasets and chart sheets.
 * @author Andras
 */
public class DatasetsAndChartsPage extends ScaveEditorPage {
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

		Composite palette = createPalette(getBody(), true);
		
		// configure viewers
		scaveEditor.configureTreeViewer(getDatasetsTreeViewer());
        scaveEditor.configureTreeViewer(getChartSheetsTreeViewer());
        
		// set contents
		Analysis analysis = scaveEditor.getAnalysis();
		getDatasetsTreeViewer().setInput(analysis.getDatasets());
        getChartSheetsTreeViewer().setInput(analysis.getChartSheets());
        
        if (enableGuiTest) {
        	getDatasetsTreeViewer().getTree().setData(WIDGET_ID, DATASETS_TREE_ID);
        	getChartSheetsTreeViewer().getTree().setData(WIDGET_ID, CHARTSHEETS_TREE_ID);
			palette.setData(WIDGET_ID, DATASETS_PAGE_PALETTE_ID);
        }
	}

	/**
	 * This method initializes formToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null)
			formToolkit = new FormToolkit2(Display.getCurrent());
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

	@Override
	public boolean gotoObject(Object object) {
		if (object instanceof EObject) {
			EObject eobject = (EObject)object;
			if (ScaveModelUtil.findEnclosingOrSelf(eobject, Datasets.class) != null) {
				getDatasetsTreeViewer().reveal(eobject);
				return true;
			}
			else if (ScaveModelUtil.findEnclosingOrSelf(eobject, ChartSheets.class) != null) {
				getChartSheetsTreeViewer().reveal(eobject);
				return true;
			}
		}
		else if (object instanceof DelegatingWrapperItemProvider) {
			DelegatingWrapperItemProvider wrapper = (DelegatingWrapperItemProvider)object;
			getChartSheetsTreeViewer().reveal(wrapper);
			return true;
		}
		return false;
	}
}


