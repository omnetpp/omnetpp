package org.omnetpp.scave2.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.emf.edit.command.CreateChildCommand;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.editors.ui.BrowseDataPage;
import org.omnetpp.scave2.editors.ui.ChartPage;
import org.omnetpp.scave2.editors.ui.ChartSheetPage;
import org.omnetpp.scave2.editors.ui.DatasetPage;
import org.omnetpp.scave2.editors.ui.DatasetsAndChartsPage;
import org.omnetpp.scave2.editors.ui.InputsPage;
import org.omnetpp.scave2.editors.ui.ScaveEditorPage;

/**
 * OMNeT++/OMNEST Analysis tool.  
 * 
 * @author andras, tomi
 */
//TODO add flag into InputFile: "name" is OS path or workspace-relative path
//TODO Outline view doesn't use our labelprovider
//TODO copy/paste doesn't work on the model
//TODO Browse page, Create Dataset button: assemble default name from filter params
//TODO Browse page, Add To Dataset button: offer tree to select insertion point
//TODO Browse page: add a "Preview selected items on a chart" button
//TODO Chart sheet: should modify order of charts in the ChartSheet#getCharts collection
//TODO chart page: "view numbers" feature
//TODO "view numbers in a vector" feature (ie is this the same feature as "view numbers in chart"?)
//TODO label provider: print attributes in "quotes"
public class ScaveEditor extends AbstractEMFModelEditor {

	private InputsPage inputsPage;
	private BrowseDataPage browseDataPage;
	private DatasetsAndChartsPage datasetsPage;
	
	/**
	 *  ResultFileManager containing all files of the analysis. 
	 */
	private ResultFileManager manager = new ResultFileManager();

	/**
	 * Loads/unloads result files in manager, according to changes in the model and in the workspace.
	 */
	private ResultFilesTracker tracker;
	
	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}
	
	public ResultFileManager getResultFileManager() {
		return manager;
	}
	
	public List<File> getInputFiles() {
		return tracker.getInputFiles();
	}
	
	public BrowseDataPage getBrowseDataPage() {
		return browseDataPage;
	}
	
	@Override
	public void init(IEditorSite site, IEditorInput editorInput) {
		// init super. Note that this does not load the model yet -- it's done in createModel() called from createPages().
		super.init(site, editorInput);
	}

	@Override
	public void dispose() {
		if (tracker!=null) {
			ResourcesPlugin.getWorkspace().removeResourceChangeListener(tracker);
			if (adapterFactory instanceof IChangeNotifier) {
				IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
				notifier.removeListener(tracker);
			}
		}
		if (manager != null) {
			manager.delete(); // it would get garbage-collected anyway, but the sooner the better because it may have allocated large amounts of data
			manager = null;
		}
		super.dispose();
	}
	
	@Override
	public void createModel() {
		super.createModel();

		// ensure mandatory objects exist; XXX must also prevent them from being deleted
		Analysis analysis = getAnalysis();
		if (analysis.getInputs()==null)
			analysis.setInputs(ScaveModelFactory.eINSTANCE.createInputs());
		if (analysis.getDatasets()==null)
			analysis.setDatasets(ScaveModelFactory.eINSTANCE.createDatasets());
		if (analysis.getChartSheets()==null)
			analysis.setChartSheets(ScaveModelFactory.eINSTANCE.createChartSheets());

		tracker = new ResultFilesTracker(manager, analysis.getInputs()); //XXX must ensure that Inputs never gets deleted or replaced!!! 
		
		// listen to model changes
		if (adapterFactory instanceof IChangeNotifier) {
			IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
			notifier.addListener(tracker);
		}
		
		// listen to resource changes: create, delete, modify
		ResourcesPlugin.getWorkspace().addResourceChangeListener(tracker);
	}

	@Override
	protected void doCreatePages() {
		// add fixed pages: Inputs, Browse Data, Datasets
		FillLayout layout = new FillLayout();
        getContainer().setLayout(layout);

		// we can load the result files now
        //XXX we should probably move this after creating the pages, but then we'll need something like browseDataPage.refresh()  
        tracker.synchronize();

        createInputsPage();
        createBrowseDataPage();
        createDatasetsPage();

	}

	@Override
	protected void initializeContentOutlineViewer(Viewer contentOutlineViewer) {
		contentOutlineViewer.setInput(getAnalysis());
	}

	/**
	 * Adds a fixed (non-closable) editor page at the last position 
	 */
	public int addScaveEditorPage(ScaveEditorPage page) {
		int index = addPage(page);
		setPageText(index, page.getPageTitle());
		return index;
	}

	/**
	 * Adds a closable editor page at the last position 
	 */
	public int addClosableScaveEditorPage(ScaveEditorPage page) {
		int index = getPageCount();
		addClosablePage(index, page);
		setPageText(index, page.getPageTitle());
		return index;
	}
	
	/**
	 * Adds a closable page to the multi-page editor. This is a variant of 
	 * addPage(int index, Control control).
	 */
	public void addClosablePage(int index, Control control) {
		// hack: add it in the normal way, then replace CTabItem with one with SWT.CLOSE set
		super.addPage(index, control);
		CTabFolder ctabFolder = (CTabFolder) control.getParent();
		ctabFolder.getItem(index).dispose();
		CTabItem item = new CTabItem(ctabFolder, SWT.CLOSE, index);
		item.setControl(control);
	}

	/**
	 * Closes the page of the multi-page editor page which holds
	 * the given control. The request is ignored if the control is
	 * not found among the pages.
	 */
	public void removePage(Control control) {
		int index = findPage(control);
		if (index!=-1)
			removePage(index);
	}
	
	/**
	 * Returns the index of the multi-page editor page which holds
	 * the given control (typeically some Composite). Returns -1 if not found.
	 */
	public int findPage(Control control) {
		for (int i=0; i<getPageCount(); i++)
			if (getControl(i)==control)
				return i;
		return -1;
	}

	/**
     * Utility method: Returns the Analysis object from the resource.
     */
    //XXX catch potential nullpointer- and classcast exceptions during resource magic 
	public Analysis getAnalysis() {
    	XMIResource resource = (XMIResource)editingDomain.getResourceSet().getResources().get(0);
    	Analysis analysis = (Analysis)resource.getContents().get(0);
    	return analysis;
    }
	
	/**
	 * Opens the given dataset on a new editor page, and switches to it. 
	 */
	public void openDataset(Dataset dataset) {
		int pageIndex = createDatasetPage(dataset);
		setActivePage(pageIndex);
	}

	/**
	 * Opens the given chart sheet on a new editor page, and switches to it. 
	 */
	public void openChartSheet(ChartSheet chartSheet) {
		int pageIndex = createChartSheetPage(chartSheet);
		setActivePage(pageIndex);
	}
	
	/**
	 * Opens the given chart on a new editor page, and switches to it. 
	 */
	public void openChart(Chart chart) {
		int pageIndex = createChartPage(chart);
		setActivePage(pageIndex);
	}
	
	private void createInputsPage() {
		inputsPage = new InputsPage(getContainer(), this);
		addScaveEditorPage(inputsPage);
	}
	
	private void createBrowseDataPage() {
		browseDataPage = new BrowseDataPage(getContainer(), this);
		addScaveEditorPage(browseDataPage);
	}
	
	private void createDatasetsPage() {
		datasetsPage = new DatasetsAndChartsPage(getContainer(), this);
        addScaveEditorPage(datasetsPage);
	}
	
	private int createDatasetPage(Dataset dataset) {
		DatasetPage page = new DatasetPage(getContainer(), this, dataset);
		int index = addClosableScaveEditorPage(page);
		return index;
	}
	
	private int createChartSheetPage(ChartSheet chartsheet) {
		ChartSheetPage page = new ChartSheetPage(getContainer(), this, chartsheet);
		int index = addClosableScaveEditorPage(page);
		return index;
	}
	
	private int createChartPage(Chart chart) {
		ChartPage page = new ChartPage(getContainer(), this, chart);
		int index = addClosableScaveEditorPage(page);
		return index;
	}
	
	@Override
	public void handleSelectionChange(ISelection selection) {
		super.handleSelectionChange(selection);

		// propagate selection to the following viewers
		setViewerSelectionNoNotify(inputsPage.getInputFilesTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getDatasetsTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getChartSheetsTreeViewer(), selection);
	}
	
	/**
	 * Adds the given workspace file to Inputs.
	 */
	public void addWorkspaceFileToInputs(IFile resource) {
		String resourcePath = resource.getFullPath().toPortableString();

		// add resourcePath to Inputs if not already there
		Inputs inputs = getAnalysis().getInputs();
		boolean found = false;
		for (Object inputFileObj : inputs.getInputs()) {
			InputFile inputFile = (InputFile)inputFileObj;
			if (inputFile.getName().equals(resourcePath))
				found = true;
		}

		if (!found) {
			// use the EMF.Edit Framework's command interface to do the job (undoable)
			InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
			inputFile.setName(resourcePath);
			ArrayList selection = new ArrayList();
			selection.add(inputs);
			Command command = new CreateChildCommand(getEditingDomain(), inputs, ScaveModelFactory.eINSTANCE.getScaveModelPackage().getInputs_Inputs(), inputFile, selection);
			executeCommand(command);
		}
	}
	
	/**
	 * Utility function: finds an IFile for an existing file given with OS path. Returns null if the file was not found.
	 */
	public static IFile findFileInWorkspace(String fileName) {
		IFile[] iFiles = ResourcesPlugin.getWorkspace().getRoot().findFilesForLocation(new Path(fileName));
		IFile iFile = null;
		for (IFile f : iFiles) { 
			if (f.exists()) {
				iFile = f;
				break;
			}
		}
		return iFile;
	}

	public void reportError(String message) {
		// TODO
	}
	
	/**
	 * Utility method. 
	 */
	public void executeCommand(Command command) {
		getEditingDomain().getCommandStack().execute(command);
	}

	public ISelectionChangedListener getSelectionChangedListener() {
		return selectionChangedListener;
	}

}


