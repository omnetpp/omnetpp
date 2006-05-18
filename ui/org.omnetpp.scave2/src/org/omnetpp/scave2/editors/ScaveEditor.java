package org.omnetpp.scave2.editors;

import java.util.ArrayList;

import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave.model.Analysis;

public class ScaveEditor extends AbstractEMFModelEditor {

	private OverviewPage overviewPage;
	private BrowseDataPage browseDataPage;
	private ArrayList<DatasetPage> datasetPages = new ArrayList<DatasetPage>(3);
	private ArrayList<ChartSheetPage> chartSheetPages = new ArrayList<ChartSheetPage>(3);
	
	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}
	
	@Override
	protected void doCreatePages() {
		FillLayout layout = new FillLayout();
        getContainer().setLayout(layout);

        createOverviewPage();
        createBrowseDataPage();

        configureTreeViewer(overviewPage.getInputFilesTreeViewer());
        configureTreeViewer(overviewPage.getDatasetsTreeViewer());
        configureTreeViewer(overviewPage.getChartSheetsTreeViewer());

        //XXX catch potential nullpointer- and classcast exceptions during resource magic 
        XMIResource resource = (XMIResource)editingDomain.getResourceSet().getResources().get(0);
        Analysis analysis = (Analysis)resource.getContents().get(0);
        
        overviewPage.getInputFilesTreeViewer().setInput(analysis.getInputs());
        overviewPage.getDatasetsTreeViewer().setInput(analysis.getDatasets());
        overviewPage.getChartSheetsTreeViewer().setInput(analysis.getChartSheets()); //XXX for now...

        addSelectionChangedListenerTo(overviewPage.getInputFilesTreeViewer());
        addSelectionChangedListenerTo(overviewPage.getDatasetsTreeViewer());
        addSelectionChangedListenerTo(overviewPage.getChartSheetsTreeViewer());
        
        //createDatasetPage("queue lengths");
        //createDatasetPage("average EED");
        //createDatasetPage("frame counts");
        //createChartPage("packet loss");
        //createChartPage("delay");
	}

	private void createOverviewPage() {
		overviewPage = new OverviewPage(getContainer(), SWT.NONE);
		setFormTitle(overviewPage, "Overview");
		int index = addPage(overviewPage);
		setPageText(index, "Overview");
	}
	
	private void createBrowseDataPage() {
		browseDataPage = new BrowseDataPage(getContainer(), SWT.NONE);
		setFormTitle(browseDataPage, "Browse data");
		int index = addPage(browseDataPage);
		setPageText(index, "Browse data");
	}
	
	private void createDatasetPage(String name) {
		DatasetPage page = new DatasetPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Dataset: " + name);
		addDatasetPage("Dataset: " + name, page);
	}
	
	private void addDatasetPage(String pageText, DatasetPage page) {
		datasetPages.add(page);
		int index = addPage(page);
		setPageText(index, pageText);
	}
	
	private void createChartPage(String name) {
		ChartSheetPage page = new ChartSheetPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Charts: " + name);
		addChartSheetPage("Charts: " + name, page);
	}
	
	private void addChartSheetPage(String pageText, ChartSheetPage page) {
		chartSheetPages.add(page);
		int index = addPage(page);
		setPageText(index, pageText);
	}
	
	private void setFormTitle(ScrolledForm form, String title) {
		form.setFont(new Font(null, "Arial", 12, SWT.BOLD));
		form.setForeground(new Color(null, 0, 128, 255));
		form.setText(title);
	}

	@Override
	public void handleSelectionChange(ISelection selection, Object source) {
		super.handleSelectionChange(selection, source);

		setSelectionToViewer(overviewPage.getInputFilesTreeViewer(), selection, source);
		setSelectionToViewer(overviewPage.getDatasetsTreeViewer(), selection, source);
		setSelectionToViewer(overviewPage.getChartSheetsTreeViewer(), selection, source);
	}
}


