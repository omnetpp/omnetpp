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
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.content.IContentType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.ChartSheet;
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

/**
 * OMNeT++/OMNEST Analysis tool.  
 * 
 * @author andras, tomi
 */
//FIXME add flag into InputFile: "name" is OS path or workspace-relative path
//FIXME file drag&drop doesn't mark the document as dirty
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
        
        setupResultFileDropTarget(overviewPage.getInputFilesTreeViewer().getControl()); //XXX throws error: looks like EMF already sets up a DropTarget on this, and we cannot add another one
        setupResultFileDropTarget(overviewPage.getPhysicalDataTreeViewer().getControl());
        setupResultFileDropTarget(overviewPage.getLogicalDataTreeViewer().getControl());

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

	public void openChartSheet(ChartSheet chartSheet) {
		createChartPage(chartSheet.getName());
	}
	
	protected void initializeContentOutlineViewer(Viewer contentOutlineViewer) {
		contentOutlineViewer.setInput(getAnalysisModelObject());
	}

	private void createOverviewPage() {
		InputsTreeViewProvider physicalView = new InputsPhysicalViewProvider(this);
		InputsTreeViewProvider logicalView = new InputsLogicalViewProvider(this);
		
		overviewPage = new OverviewPage(getContainer(), SWT.NONE);
        configureTreeViewer(overviewPage.getInputFilesTreeViewer());
        configureTreeViewer(overviewPage.getDatasetsTreeViewer());
        configureTreeViewer(overviewPage.getChartSheetsTreeViewer());
        physicalView.configureTreeViewer(overviewPage.getPhysicalDataTreeViewer());
        logicalView.configureTreeViewer(overviewPage.getLogicalDataTreeViewer());
		setFormTitle(overviewPage, "Overview");
		int index = addPage(overviewPage);
		setPageText(index, "Overview");
	}
	
	private void createBrowseDataPage() {
		InputsTableViewProvider scalarsView = new InputsScalarsViewProvider(this);
		InputsTableViewProvider vectorsView = new InputsVectorsViewProvider(this);

		browseDataPage = new BrowseDataPage(getContainer(), SWT.NONE);
		scalarsView.configureFilterPanel(browseDataPage.getScalarsPanel());
		vectorsView.configureFilterPanel(browseDataPage.getVectorsPanel());
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
	
	/**
	 * Sets up the given control so that when a file is drag-dropped into it,
	 * it will be added to Inputs unless it's already in there.
	 */
	private void setupResultFileDropTarget(Control dropTargetControl) {
		// set up DropTarget, and add FileTransfer to it;
		// issue: the EMF editor already adds a drop target to the TreeViewer
		DropTarget dropTarget = null;
		Object o = dropTargetControl.getData("DropTarget");
		if (o instanceof DropTarget) {
			dropTarget = (DropTarget) o; // this is a dirty hack, but there seems to be no public API for getting an existing drop target
			// add FileTransfer to the transfer array if not already in there
			Transfer[] transfer = dropTarget.getTransfer();
			Transfer[] transfer2 = new Transfer[transfer.length+1];
			System.arraycopy(transfer, 0, transfer2, 0, transfer.length);
			transfer2[transfer2.length-1] = FileTransfer.getInstance();
			dropTarget.setTransfer(transfer2);
		}
		else {
			dropTarget = new DropTarget(dropTargetControl, DND.DROP_DEFAULT | DND.DROP_COPY);
			dropTarget.setTransfer(new Transfer[] {FileTransfer.getInstance()});
		}

		// add our listener to handle file transfer to the DropTarget
		dropTarget.addDropListener(new DropTargetAdapter() {
			public void dragEnter(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT)
					event.detail = DND.DROP_COPY;
			}
			public void dragOperationChanged(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT)
					event.detail = DND.DROP_COPY;
			}
			public void drop(DropTargetEvent event) {
				if (event.data instanceof String[]) {
					String [] fileNames = (String[])event.data;
					for (int i=0; i<fileNames.length; i++)
						fileDropped(fileNames[i]);
				}
			}
		});
	}

	/**
	 * Adds the given file (OS-relative) to Inputs unless it's already in there.
	 */
	private void fileDropped(String fileName) {
		// convert OS path to workspace-relative path
		IFile iFile = findFileInWorkspace(fileName);
		if (iFile==null) {
			System.out.println("path not in workspace: "+fileName); //XXX error dialog?
			return;
		}
		String resourcePath = iFile.getFullPath().toPortableString();
		
		// add resourcePath to Inputs if not already there
		Inputs inputs = getAnalysisModelObject().getInputs();
		boolean found = false;
		for (Object inputFileObj : inputs.getInputs()) {
			InputFile inputFile = (InputFile)inputFileObj;
			if (inputFile.getName().equals(resourcePath))
				found = true;
		}

		if (!found) {
			InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
			inputFile.setName(resourcePath);
			inputs.getInputs().add(inputFile);
		}
	}

	/**
	 * Finds an IFile for an existing file given with OS path. Returns null if the file was not found.
	 */
	private IFile findFileInWorkspace(String fileName) {
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

	private void loadFiles(Inputs inputs) {
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


