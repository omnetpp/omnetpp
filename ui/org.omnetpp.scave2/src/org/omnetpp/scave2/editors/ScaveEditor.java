package org.omnetpp.scave2.editors;

import static org.omnetpp.scave2.model.DatasetType.SCALAR;
import static org.omnetpp.scave2.model.DatasetType.VECTOR;

import java.util.ArrayList;
import java.util.Collection;
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
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.emf.edit.command.CreateChildCommand;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Datasets;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.ContentTypes;
import org.omnetpp.scave2.actions.IScaveAction;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.editors.providers.InputsLogicalViewProvider;
import org.omnetpp.scave2.editors.providers.InputsPhysicalViewProvider;
import org.omnetpp.scave2.editors.providers.InputsScalarsViewProvider;
import org.omnetpp.scave2.editors.providers.InputsTableViewProvider;
import org.omnetpp.scave2.editors.providers.InputsTreeViewProvider;
import org.omnetpp.scave2.editors.providers.InputsVectorsViewProvider;
import org.omnetpp.scave2.editors.ui.BrowseDataPage;
import org.omnetpp.scave2.editors.ui.ChartPage;
import org.omnetpp.scave2.editors.ui.ChartSheetPage;
import org.omnetpp.scave2.editors.ui.DatasetPage;
import org.omnetpp.scave2.editors.ui.DatasetsAndChartsPage;
import org.omnetpp.scave2.editors.ui.InputsPage;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.FilterParams;

/**
 * OMNeT++/OMNEST Analysis tool.  
 * 
 * @author andras, tomi
 */
//FIXME add flag into InputFile: "name" is OS path or workspace-relative path
//TODO Outline view doesn't use our labelprovider
//FIXME setting ContainingSheet for a chart doesn't update the label in the table!!!
//TODO open dataset by double-click 
public class ScaveEditor extends AbstractEMFModelEditor implements INotifyChangedListener, IResourceChangeListener {

	private InputsPage inputsPage;
	private BrowseDataPage browseDataPage;
	private DatasetsAndChartsPage datasetsPage;
	
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
	
	public BrowseDataPage getBrowseDataPage() {
		return browseDataPage;
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

	/**
	 * Variant of addPage(int index, Control control): adds a page with a 
	 * large [X] on the tab. 
	 */
	public void addClosablePage(int index, String text, Control control) {
		// add it in the normal way, then replace CTabItem with one with SWT.CLOSE set
		super.addPage(index, control);
		CTabFolder ctabFolder = (CTabFolder) control.getParent();
		ctabFolder.getItem(index).dispose();
		CTabItem item = new CTabItem(ctabFolder, SWT.CLOSE, index);
		item.setControl(control);
		setPageText(index, text);
	}

	/**
	 * Adds a closable page on the last position 
	 */
	public int addClosablePage(String text, Control control) {
		int index = getPageCount();
		addClosablePage(index, text, control);
		return index;
	}
	
	public int addPage(String text, Control control) {
		int index = addPage(control);
		setPageText(index, text);
		return index;
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

        createInputsPage();
        createBrowseDataPage();
        createDatasetsPage();


        Analysis analysis = getAnalysisModelObject();
        // create mandatory objects if not exist; XXX must also prevent them from being deleted
        if (analysis.getInputs()==null)
        	analysis.setInputs(ScaveModelFactory.eINSTANCE.createInputs());
        if (analysis.getDatasets()==null)
        	analysis.setDatasets(ScaveModelFactory.eINSTANCE.createDatasets());
        if (analysis.getChartSheets()==null)
        	analysis.setChartSheets(ScaveModelFactory.eINSTANCE.createChartSheets());

        loadFiles(analysis.getInputs());
        
        inputsPage.getInputFilesTreeViewer().setInput(analysis.getInputs());
        inputsPage.getPhysicalDataTreeViewer().setInput(analysis.getInputs());
        inputsPage.getLogicalDataTreeViewer().setInput(analysis.getInputs());
        browseDataPage.getScalarsTableViewer().setInput(analysis.getInputs());
        browseDataPage.getVectorsTableViewer().setInput(analysis.getInputs());
        datasetsPage.getDatasetsTreeViewer().setInput(analysis.getDatasets());
        datasetsPage.getChartSheetsTreeViewer().setInput(analysis.getChartSheets()); //XXX for now...
        
        setupResultFileDropTarget(inputsPage.getInputFilesTreeViewer().getControl()); //XXX throws error: looks like EMF already sets up a DropTarget on this, and we cannot add another one
        setupResultFileDropTarget(inputsPage.getPhysicalDataTreeViewer().getControl());
        setupResultFileDropTarget(inputsPage.getLogicalDataTreeViewer().getControl());
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
		int pageIndex = createDatasetPage(dataset);
		setActivePage(pageIndex);
	}

	public void openChartSheet(ChartSheet chartSheet) {
		int pageIndex = createChartSheetPage(chartSheet);
		setActivePage(pageIndex);
	}
	
	public void openChart(Chart chart) {
		int pageIndex = createChartPage(chart);
		setActivePage(pageIndex);
	}
	
	protected void initializeContentOutlineViewer(Viewer contentOutlineViewer) {
		contentOutlineViewer.setInput(getAnalysisModelObject());
	}

	private void createInputsPage() {
		//XXX move stuff into page's constructor
		inputsPage = new InputsPage(getContainer(), SWT.NONE, this);
		setFormTitle(inputsPage, "Inputs");

		InputsTreeViewProvider physicalViewProvider = new InputsPhysicalViewProvider(this);
		InputsTreeViewProvider logicalViewProvider = new InputsLogicalViewProvider(this);
        configureTreeViewer(inputsPage.getInputFilesTreeViewer());
        physicalViewProvider.configureTreeViewer(inputsPage.getPhysicalDataTreeViewer());
        logicalViewProvider.configureTreeViewer(inputsPage.getLogicalDataTreeViewer());
        inputsPage.getPhysicalDataTreeViewer().addSelectionChangedListener(selectionChangedListener);
        inputsPage.getLogicalDataTreeViewer().addSelectionChangedListener(selectionChangedListener);
        
		addPage("Inputs", inputsPage);
	}
	
	private void createBrowseDataPage() {
		//XXX move stuff into page's constructor
		browseDataPage = new BrowseDataPage(getContainer(), SWT.NONE, this);
		setFormTitle(browseDataPage, "Browse data");

		InputsTableViewProvider scalarsViewProvider = new InputsScalarsViewProvider(this);
		InputsTableViewProvider vectorsViewProvider = new InputsVectorsViewProvider(this);
		scalarsViewProvider.configureFilterPanel(browseDataPage.getScalarsPanel());
		vectorsViewProvider.configureFilterPanel(browseDataPage.getVectorsPanel());

		addPage("Browse data", browseDataPage);
	}
	
	private void createDatasetsPage() {
		//XXX move stuff into page's constructor
		datasetsPage = new DatasetsAndChartsPage(getContainer(), SWT.NONE, this);
		setFormTitle(datasetsPage, "Datasets and Charts");

		configureTreeViewer(datasetsPage.getDatasetsTreeViewer());
        configureTreeViewer(datasetsPage.getChartSheetsTreeViewer());

        addPage("Datasets", datasetsPage);
	}
	
	private int createDatasetPage(Dataset dataset) {
		//XXX move stuff into page's constructor
		DatasetPage page = new DatasetPage(getContainer(), SWT.NONE, this);
		setFormTitle(page, "Dataset: " + dataset.getName());

		configureTreeViewer(page.getDatasetTreeViewer());
		if (SCALAR.equals(dataset.getType())) {
			page.addScalarsPanel();
			InputsTableViewProvider provider = new InputsScalarsViewProvider(this);
			provider.configureFilterPanel(page.getFilterPanel());
		} else if (VECTOR.equals(dataset.getType())) {
			page.addVectorsPanel();
			InputsTableViewProvider provider = new InputsVectorsViewProvider(this);
			provider.configureFilterPanel(page.getFilterPanel());
		} else {
			throw new RuntimeException("invalid or unset dataset 'type' attribute: "+dataset.getType()); //XXX proper error handling
		}
		page.getDatasetTreeViewer().setInput(dataset);
		int index = addClosablePage("Dataset: " + dataset.getName(), page);
		return index;
	}
	
	private int createChartSheetPage(ChartSheet chartsheet) {
		//XXX move stuff into page's constructor
		ChartSheetPage page = new ChartSheetPage(getContainer(), SWT.NONE);
		setFormTitle(page, "Charts: " + chartsheet.getName());

		Collection<Chart> charts = findCharts(chartsheet);
		Composite parent = page.getChartSheetComposite();
		for (Chart chart : charts) {
			Dataset dataset = findEnclosingDataset(chart);
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart);
			String type = dataset.getType();
			page.addChart(ChartFactory.createChart(parent, type, idlist, manager));
		}
		
		int index = addClosablePage("Charts: " + chartsheet.getName(), page);
		return index;
	}
	
	private int createChartPage(Chart chart) {
		//XXX move stuff into page's constructor
		ChartPage page = new ChartPage(getContainer(), SWT.NONE);
		Composite parent = page.getChartComposite();
		Dataset dataset = findEnclosingDataset(chart);
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart);
		String type = dataset.getType();
		page.setChart(ChartFactory.createChart(parent, type, idlist, manager));
		setFormTitle(page, "Chart: " + chart.getName());
		int index = addClosablePage("Chart: " + chart.getName(), page);
		return index;
	}
	
	private void setFormTitle(ScrolledForm form, String title) {
		form.setFont(new Font(null, "Arial", 12, SWT.BOLD));
		form.setForeground(new Color(null, 0, 128, 255));
		form.setText(title);
	}
	
	public Dataset createDataset(String name, String type, FilterParams params) {
		Dataset dataset = ScaveModelFactory.eINSTANCE.createDataset();
		dataset.setName(name);
		dataset.setType(type);
		dataset.getItems().add(createAdd(params));
		return dataset;
	}
	
	public Chart createChart(String name) {
		Chart chart = ScaveModelFactory.eINSTANCE.createChart();
		chart.setName(name);
		return chart;
	}
	
	public Add createAdd(FilterParams params) {
		Add add = ScaveModelFactory.eINSTANCE.createAdd();
		add.setFilenamePattern(params.getRunNamePattern());
		add.setModuleNamePattern(params.getModuleNamePattern());
		add.setNamePattern(params.getDataNamePattern());
		return add;
	}
	
	private Collection<Chart> findCharts(ChartSheet chartsheet) {
		List<Chart> charts = new ArrayList<Chart>();
		Datasets datasets = getAnalysisModelObject().getDatasets();
		findCharts(chartsheet, datasets, charts);
		return charts;
	}
	
	private void findCharts(ChartSheet chartsheet, Object container, List<Chart> result) {
		if (container instanceof Datasets) {
			for (Object dataset : ((Datasets)container).getDatasets())
				findCharts(chartsheet, dataset, result);
		}
		else if (container instanceof Dataset) {
			for (Object item : ((Dataset)container).getItems()) {
				findCharts(chartsheet, item, result);
			}
		}
		else if (container instanceof Group) {
			for (Object item : ((Group)container).getItems())
				findCharts(chartsheet, item, result);
		}
		else if (container instanceof Chart) {
			Chart chart = (Chart)container;
			if (chart.getContainingSheet() == null ||
				chart.getContainingSheet().equals(chartsheet)) {
				result.add(chart);
			}
		}
	}
	
	private Dataset findEnclosingDataset(Chart chart) {
		EObject parent = chart.eContainer();
		while (parent != null && !(parent instanceof Dataset))
			parent = parent.eContainer();
		return (Dataset)parent;
	}

	@Override
	public void handleSelectionChange(ISelection selection) {
		super.handleSelectionChange(selection);

		setViewerSelectionNoNotify(inputsPage.getInputFilesTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getDatasetsTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getChartSheetsTreeViewer(), selection);
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
	 * Finds an IFile for an existing file given with OS path. Returns null if the file was not found.
	 */
	public IFile findFileInWorkspace(String fileName) {
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
				else 
					throw new RuntimeException("wrong file type:"+file.getFullPath()); //XXX proper error handling (e.g. remove file from Inputs?)
			}
		} catch (CoreException e) {
			System.err.println("Cannot open resource: " + file.getFullPath()); //XXX proper error message
		}
	}
	
	private void unloadFile(IFile file) {
		File resultFile = manager.getFile(file.getLocation().toOSString());
		inputFiles.remove(resultFile);
		// TODO: ResultFileManager.unloadFile() not yet implemented
		//if (resultFile != null)
		//	manager.unloadFile(resultFile);
	}
	
//	/**
//	 * Utility function: Adds dynamic behaviour to a control (typically a Button): 
//	 * it gets disabled whenever the viewer's selection is empty.
//	 */
//	public static void disableButtonWhenSelectionEmpty(final Control button, final Viewer viewer) {
//		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
//			public void selectionChanged(SelectionChangedEvent event) {
//				button.setEnabled(!event.getSelection().isEmpty());
//			}
//		});
//	}
//
//	/**
//	 * Utility function: Adds dynamic behaviour to a control (typically a Button): 
//	 * it gets enabled only whenever the viewer's selection is not empty, and 
//	 * only contains instances of the given EClass (or subclasses).
//	 */
//	public static void disableButtonOnSelectionContent(final Control button, final Viewer viewer, final EClass eClass) {
//		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
//			public void selectionChanged(SelectionChangedEvent event) {
//				IStructuredSelection sel = (IStructuredSelection)event.getSelection();
//				boolean ok = !sel.isEmpty();  // selection is not empty, and only contains instances of eClass
//				for (Object o : sel.toArray()) {
//					if (!(o instanceof EObject) || !((EObject)o).eClass().isSuperTypeOf(eClass)) {
//						ok = false;
//						break;
//					}
//				}
//				button.setEnabled(ok);
//			}
//		});
//	}
//	
//	/**
//	 * Utility function: configures a Remove button which is associated with a viewer.
//	 */
//	public void configureRemoveButton(final Button removeButton, final Viewer viewer) {
//		ScaveEditor.disableButtonWhenSelectionEmpty(removeButton, viewer);
//		removeButton.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				// use EMF.Edit Framework do to the removal (this makes it undoable)
//				IStructuredSelection sel = (IStructuredSelection)viewer.getSelection();
//				Command command = RemoveCommand.create(getEditingDomain(), sel.toList());
//				getEditingDomain().getCommandStack().execute(command);
//			}
//		});
//	}
//	
//	/**
//	 * Utility function: configures an Edit button which is associated with a viewer.
//	 */
//	public void configureEditButton(final Button editButton, final Viewer viewer) {
//		ScaveEditor.disableButtonWhenSelectionEmpty(editButton, viewer);
//		editButton.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				IStructuredSelection sel = (IStructuredSelection)viewer.getSelection();
//				openEditSelectedElemementsDialog(sel);
//			}
//		});
//	}
//
//	/**
//	 * Connects the button with an action, so that
//	 * the action is executed, when the button is pressed and
//	 * the button is enabled/disabled when the action becomes enabled/disabled.
//	 */
//	public void configureButton(final Button button, final IEditorActionDelegate delegate) {
//		final Action action = new Action() {
//			public void run() {
//				delegate.setActiveEditor(this, ScaveEditor.this);
//				delegate.run(this);
//			}
//		};
//		delegate.setActiveEditor(action, this);
//		getSite().getSelectionProvider().addSelectionChangedListener(new ISelectionChangedListener() {  <<<<<<<<< this wasn't good
//			public void selectionChanged(SelectionChangedEvent event) {
//				delegate.selectionChanged(action, event.getSelection());
//			}
//		});
//		button.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				action.run();
//			}
//		});
//		final IPropertyChangeListener propertyChangeListener = new IPropertyChangeListener() {
//			public void propertyChange(PropertyChangeEvent event) {
//				if (event.getProperty().equals(IAction.ENABLED)) {
//					if (!button.isDisposed())
//						button.setEnabled(action.isEnabled());
//				}
//			}
//		};
//		action.addPropertyChangeListener(propertyChangeListener);
//		button.addDisposeListener(new DisposeListener() {
//			public void widgetDisposed(DisposeEvent e) {
//				action.removePropertyChangeListener(propertyChangeListener);
//			}
//		});
//	}
	
	/**
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled.
	 * 
	 * Note: ActionContributionItem is not good here because:
	 *  (1) it wants to create the button itself, and thus not compatible with FormToolkit
	 *  (2) the button it creates has wrong background color, and there's no way to access the button to fix it
	 *  (3) it will make the button listen to global selection changes, and cannot be tied to a viewer 
	 */
	private void doConfigureButton(final Button button, final IScaveAction action) {
		//button.setText(action.getText());
		button.setText(action.getText()+" / "+button.getText());
		button.setToolTipText(action.getToolTipText());
		//XXX button.setImage(action.getImageDescriptor());
		
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				action.run();
			}
		});
		final IPropertyChangeListener propertyChangeListener = new IPropertyChangeListener() {
			public void propertyChange(PropertyChangeEvent event) {
				if (event.getProperty().equals(IAction.ENABLED)) {
					if (!button.isDisposed())
						button.setEnabled(action.isEnabled());
				}
			}
		};
		action.addPropertyChangeListener(propertyChangeListener);
		button.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				action.removePropertyChangeListener(propertyChangeListener);
			}
		});
	}

	/* 
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled. 
	 * 
	 * The action will be enabled/disabled based on the selection service's
	 * selection.
	 */
	public void configureGlobalButton(final Button button, final IScaveAction action) {
		doConfigureButton(button, action);
		getSite().getWorkbenchWindow().getSelectionService().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				action.selectionChanged(selection);
			}
		});
	}
	
	/* 
	 * Connects the button with an action, so that the action is executed 
	 * when the button is pressed, and the button is enabled/disabled when 
	 * the action becomes enabled/disabled. 
	 * 
	 * The action will be enabled/disabled based on a viewer's selection.
	 */
	public void configureViewerButton(final Button button, final Viewer viewer, final IScaveAction action) {
		doConfigureButton(button, action);
		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				action.selectionChanged(event.getSelection());
			}
		});
	}

	/**
	 * Like <code>configureViewerButton</code>, but action will also run
	 * on double-clicking in the viewer.
	 */
	public void configureViewerDefaultButton(final Button button, final TreeViewer viewer, final IScaveAction action) {
		viewer.getTree().addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				action.run();
			}
		});
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
}


