package org.omnetpp.scave2.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.content.IContentType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.ContentTypes;
import org.omnetpp.scave2.editors.providers.InputsLogicalViewProvider;
import org.omnetpp.scave2.editors.providers.InputsPhysicalViewProvider;
import org.omnetpp.scave2.editors.providers.InputsScalarsViewProvider;
import org.omnetpp.scave2.editors.providers.InputsTableViewProvider;
import org.omnetpp.scave2.editors.providers.InputsTreeViewProvider;
import org.omnetpp.scave2.editors.providers.InputsVectorsViewProvider;
import org.omnetpp.scave2.editors.ui.BrowseDataPage;
import org.omnetpp.scave2.editors.ui.ChartSheetPage;
import org.omnetpp.scave2.editors.ui.DatasetPage;
import org.omnetpp.scave2.editors.ui.OverviewPage;

public class ScaveEditor extends AbstractEMFModelEditor implements INotifyChangedListener, IResourceChangeListener {

	private OverviewPage overviewPage;
	private BrowseDataPage browseDataPage;
	private ArrayList<DatasetPage> datasetPages = new ArrayList<DatasetPage>(3);
	private ArrayList<ChartSheetPage> chartSheetPages = new ArrayList<ChartSheetPage>(3);
	
	/**
	 *  ResultFileManager containing all files of the analysis. 
	 */
	private ResultFileManager manager = new ResultFileManager();
	
	/**
	 * List of input files.
	 * It is synchronized with the files specified by the Inputs node.
	 * When ResultFileManager.unloadFile() is implemented, then this
	 * field will be replaced by ResultFileManager.getFiles().
	 */
	private List<File> inputFiles = new ArrayList<File>();
	
	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}
	
	public ResultFileManager getResultFileManager() {
		return manager;
	}
	
	public List<File> getInputFiles() {
		return inputFiles;
	}
	
	
	
	@Override
	public void init(IEditorSite site, IEditorInput editorInput) {
		super.init(site, editorInput);
		if (adapterFactory instanceof IChangeNotifier) {
			IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
			notifier.addListener(this);
		}
		ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
	}

	@Override
	public void dispose() {
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
		if (adapterFactory instanceof IChangeNotifier) {
			IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
			notifier.removeListener(this);
		}
		if (manager != null) {
			manager.delete();
			manager = null;
		}
		super.dispose();
	}

	public void notifyChanged(Notification notification) {
		if (notification.isTouch())
			return;
		
		Analysis analysis = getAnalysisModelObject();
		if (analysis == null || analysis.getInputs() == null)
			return;

		switch (notification.getEventType()) {
		case Notification.ADD:
		case Notification.ADD_MANY:
		case Notification.REMOVE:
		case Notification.REMOVE_MANY:
		case Notification.MOVE:
		case Notification.SET:
		case Notification.UNSET:
			loadFiles(analysis.getInputs());
		}
	}
	
	public void resourceChanged(IResourceChangeEvent event) {
		Analysis analysis = getAnalysisModelObject();
		if (analysis == null || analysis.getInputs() == null)
			return;
		
		try {
			IResourceDelta delta = event.getDelta();
			delta.accept(new ResourceDeltaVisitor());
		} catch (CoreException e) {
			e.printStackTrace();
		}
	}
	
	class ResourceDeltaVisitor implements IResourceDeltaVisitor {
		public boolean visit(IResourceDelta delta) throws CoreException {
			IResource resource = delta.getResource();
			if (!(resource instanceof IFile))
				return true;
			
			IFile file = (IFile)resource;
			switch (delta.getKind()) {
			case IResourceDelta.ADDED:
					loadFile(file);
					break;
			case IResourceDelta.REMOVED:
					unloadFile(file);
					break;
			case IResourceDelta.CHANGED:
					unloadFile(file);
					loadFile(file);
					break;
			}
			return false;
		}
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
        configureTreeViewer(overviewPage.getPhysicalDataTreeViewer(), new InputsPhysicalViewProvider(this));
        configureTreeViewer(overviewPage.getLogicalDataTreeViewer(), new InputsLogicalViewProvider(this));
        configureTableViewer(browseDataPage.getScalarsTableViewer(), new InputsScalarsViewProvider(this));
        configureTableViewer(browseDataPage.getVectorsTableViewer(), new InputsVectorsViewProvider(this));

        Analysis analysis = getAnalysisModelObject();
        // create mandatory objects if not exist; XXX must also prevent them from being deleted
        if (analysis.getInputs()==null)
        	analysis.setInputs(ScaveModelFactory.eINSTANCE.createInputs());
        if (analysis.getDatasets()==null)
        	analysis.setDatasets(ScaveModelFactory.eINSTANCE.createDatasets());
        if (analysis.getChartSheets()==null)
        	analysis.setChartSheets(ScaveModelFactory.eINSTANCE.createChartSheets());

        loadFiles(analysis.getInputs());
        overviewPage.getInputFilesTreeViewer().setInput(analysis.getInputs());
        overviewPage.getDatasetsTreeViewer().setInput(analysis.getDatasets());
        overviewPage.getChartSheetsTreeViewer().setInput(analysis.getChartSheets()); //XXX for now...
        overviewPage.getPhysicalDataTreeViewer().setInput(analysis.getInputs());
        overviewPage.getLogicalDataTreeViewer().setInput(analysis.getInputs());
        browseDataPage.getScalarsTableViewer().setInput(analysis.getInputs());
        browseDataPage.getVectorsTableViewer().setInput(analysis.getInputs());
        
        //createDatasetPage("queue lengths");
        //createDatasetPage("average EED");
        //createDatasetPage("frame counts");
        //createChartPage("packet loss");
        //createChartPage("delay");
	}

    /**
     * Utility method: Returns the Analysis object from the resource.
     */
    //XXX catch potential nullpointer- and classcast exceptions during resource magic 
	public Analysis getAnalysisModelObject() {
    	XMIResource resource = (XMIResource)editingDomain.getResourceSet().getResources().get(0);
    	Analysis analysis = (Analysis)resource.getContents().get(0);
    	return analysis;
    }
	
	public void openDataset(Dataset dataset) {
		createDatasetPage(dataset.getName());
	}
	
	protected void initializeContentOutlineViewer(Viewer contentOutlineViewer) {
		contentOutlineViewer.setInput(getAnalysisModelObject());
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
	public void handleSelectionChange(ISelection selection) {
		super.handleSelectionChange(selection);

		setViewerSelectionNoNotify(overviewPage.getInputFilesTreeViewer(), selection);
		setViewerSelectionNoNotify(overviewPage.getDatasetsTreeViewer(), selection);
		setViewerSelectionNoNotify(overviewPage.getChartSheetsTreeViewer(), selection);
	}
	
	private void configureTreeViewer(TreeViewer viewer, InputsTreeViewProvider view) {
		viewer.setContentProvider(view.getContentProvider());
		viewer.setLabelProvider(view.getLabelProvider());
	}
	
	private void configureTableViewer(TableViewer viewer, InputsTableViewProvider view) {
		viewer.setContentProvider(view.getContentProvider());
		viewer.setLabelProvider(view.getLabelProvider());
	}
	
	private void  loadFiles(Inputs inputs) {
		// TODO: handle wildcards
		inputFiles.clear();
		for (Object inputFileObj : inputs.getInputs()) {
			String resourcePath = ((InputFile)inputFileObj).getName();
			loadFile(resourcePath);
		}
	}
	
	private void loadFile(String resourcePath) {
		if (resourcePath != null) {
			IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
			IResource resource = workspaceRoot.findMember(resourcePath);
			if (resource instanceof IFile) {
				IFile file = (IFile)resource;
				loadFile(file);
			}
		}
	}
	
	private void loadFile(IFile file) {
		try {
			if (file.getContentDescription() != null &&
					file.getContentDescription().getContentType() != null) {

				IContentType contentType = file.getContentDescription().getContentType();
				String path = file.getLocation().toOSString();
				if (ContentTypes.SCALAR.equals(contentType.getId()))
					inputFiles.add(manager.loadScalarFile(path));
				else if (ContentTypes.VECTOR.equals(contentType.getId()))
					inputFiles.add(manager.loadVectorFile(path));
			}
		} catch (CoreException e) {
			System.err.println("Cannot open resource: " + file.getFullPath());
		}
	}
	
	private void unloadFile(IFile file) {
		File resultFile = manager.getFile(file.getLocation().toOSString());
		inputFiles.remove(resultFile);
		// TODO: ResultFileManager.unloadFile() not yet implemented
		//if (resultFile != null)
		//	manager.unloadFile(resultFile);
	}
}


