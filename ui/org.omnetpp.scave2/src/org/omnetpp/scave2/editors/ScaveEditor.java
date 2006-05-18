package org.omnetpp.scave2.editors;

import java.util.ArrayList;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.ui.forms.widgets.ScrolledForm;

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

        overviewPage.getInputFilesTreeViewer().setInput(editingDomain.getResourceSet()); //XXX for now...
        overviewPage.getDatasetsTreeViewer().setInput(editingDomain.getResourceSet()); //XXX for now...
        overviewPage.getChartSheetsTreeViewer().setInput(editingDomain.getResourceSet()); //XXX for now...

        addSelectionChangedListenerTo(overviewPage.getInputFilesTreeViewer());
        addSelectionChangedListenerTo(overviewPage.getDatasetsTreeViewer());
        addSelectionChangedListenerTo(overviewPage.getChartSheetsTreeViewer());
        
        //createDatasetPage("queue lengths");
        //createDatasetPage("average EED");
        //createDatasetPage("frame counts");
        //createChartPage("packet loss");
        //createChartPage("delay");
	}

	private void addSelectionChangedListenerTo(TreeViewer modelViewer)	{
		modelViewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent selectionChangedEvent) {
				handleSelectionChange(selectionChangedEvent.getSelection(), (Viewer)selectionChangedEvent.getSource());
			}
		});
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

	public void handleSelectionChange(ISelection selection, Viewer source) {
		setSelectionToViewer(selection, overviewPage.getInputFilesTreeViewer(), source);
		setSelectionToViewer(selection, overviewPage.getDatasetsTreeViewer(), source);
		setSelectionToViewer(selection, overviewPage.getChartSheetsTreeViewer(), source);
		setSelectionToViewer(selection, contentOutlineViewer, source);
	}

	public void setSelectionToViewer(ISelection selection, Viewer target, Viewer source) {
		if (target!=source && !selection.equals(target.getSelection()))
			target.setSelection(selection,true);
	}
	
	@Override
	public void handleContentOutlineSelection(ISelection selection) {
		handleSelectionChange(selection, contentOutlineViewer);
	}
}


