package org.omnetpp.scave.editors;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.TreeIterator;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.ChartSheetPage;
import org.omnetpp.scave.editors.ui.DatasetPage;
import org.omnetpp.scave.editors.ui.DatasetsAndChartsPage;
import org.omnetpp.scave.editors.ui.InputsPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * OMNeT++/OMNEST Analysis tool.
 *
 * @author andras, tomi
 */
//TODO copy/paste doesn't work on the model
//TODO Chart sheet: should modify order of charts in the ChartSheet#getCharts collection
//TODO chart page: "view numbers" feature
//TODO label provider: print attributes in "quotes"
public class ScaveEditor extends AbstractEMFModelEditor implements INavigationLocationProvider {

	private InputsPage inputsPage;
	private BrowseDataPage browseDataPage;
	private DatasetsAndChartsPage datasetsPage;
	private Map<EObject,ScaveEditorPage> closablePages = new LinkedHashMap<EObject,ScaveEditorPage>();
	
	/**
	 *  ResultFileManager containing all files of the analysis.
	 */
	private ResultFileManagerEx manager = new ResultFileManagerEx();

	/**
	 * Loads/unloads result files in manager, according to changes in the model and in the workspace.
	 */
	private ResultFilesTracker tracker;
	
	/**
	 * Updates pages when the model changed.
	 */
	private INotifyChangedListener pageUpdater = new INotifyChangedListener() {
		public void notifyChanged(Notification notification) {
			updatePages(notification);
		}
	};

	/**
	 * Temporary datasets and charts are added to this resource.
	 * The resource is not saved.
	 */
	private Resource tempResource;

	/**
	 * Factory of Scave objects.
	 */
	private static final ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;

	/**
	 * Scave model package.
	 */
	private static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;

	/**
	 * The constructor.
	 */
	public ScaveEditor() {
	}

	public ResultFileManagerEx getResultFileManager() {
		return manager;
	}

	public InputsPage getInputsPage() {
		return inputsPage;
	}

	public BrowseDataPage getBrowseDataPage() {
		return browseDataPage;
	}

	public DatasetsAndChartsPage getDatasetsPage() {
		return datasetsPage;
	}

	@Override
	public void init(IEditorSite site, IEditorInput editorInput) 
		throws PartInitException {
		
		if (!(editorInput instanceof IFileEditorInput))
			throw new PartInitException("Invalid Input: Must be IFileEditorInput");
		IFile fileInput = ((IFileEditorInput)editorInput).getFile();
		if (!editorInput.exists())
			throw new PartInitException("Missing Input: Resource '" + fileInput.getFullPath().toString() + "' does not exists");
		File javaFile = fileInput.getLocation().toFile();
		if (!javaFile.exists())
			throw new PartInitException("Missing Input: Scave file '" + javaFile.toString() + "' does not exists");
		
		// add part listener to save the editor state *before* it is disposed
		final IWorkbenchPage page = site.getPage();
		page.addPartListener(new IPartListener() {
			public void partActivated(IWorkbenchPart part) {}
			public void partBroughtToTop(IWorkbenchPart part) {}
			public void partDeactivated(IWorkbenchPart part) {}
			public void partOpened(IWorkbenchPart part) {}
			public void partClosed(IWorkbenchPart part) {
				if (part == ScaveEditor.this) {
					page.removePartListener(this);
					saveState();
				}
			}
		});
		
		// init super. Note that this does not load the model yet -- it's done in createModel() called from createPages().
		super.init(site, editorInput);
	}

	@Override
	public void dispose() {
		if (tracker!=null) {
			ResourcesPlugin.getWorkspace().removeResourceChangeListener(tracker);
		}
		if (adapterFactory instanceof IChangeNotifier) {
			IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
			if (tracker != null) notifier.removeListener(tracker);
			notifier.removeListener(pageUpdater);
		}
		if (manager != null) {
			manager.delete(); // it would get garbage-collected anyway, but the sooner the better because it may have allocated large amounts of data
			if (tracker != null)      // deactivate the tracker explicitly, because it might receive a notification
				tracker.deactivate(); // in case of the ScaveEditor.dispose() was called from a notification.
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
			analysis.setInputs(factory.createInputs());
		if (analysis.getDatasets()==null)
			analysis.setDatasets(factory.createDatasets());
		if (analysis.getChartSheets()==null)
			analysis.setChartSheets(factory.createChartSheets());

		tempResource = createTempResource();

		IFile inputFile = ((IFileEditorInput)getEditorInput()).getFile();
		tracker = new ResultFilesTracker(manager, analysis.getInputs(), inputFile.getParent().getFullPath()); //XXX must ensure that Inputs never gets deleted or replaced!!!

		// listen to model changes
		if (adapterFactory instanceof IChangeNotifier) {
			IChangeNotifier notifier = (IChangeNotifier)adapterFactory;
			notifier.addListener(tracker);
			notifier.addListener(pageUpdater);
		}

		// listen to resource changes: create, delete, modify
		ResourcesPlugin.getWorkspace().addResourceChangeListener(tracker);
	}

	protected Resource createTempResource() {
		IFileEditorInput modelFile = (IFileEditorInput)getEditorInput();
		IPath tempResourcePath = modelFile.getFile().getFullPath().addFileExtension("temp");
		URI resourceURI = URI.createPlatformResourceURI(tempResourcePath.toString(), true);;
		Resource resource = editingDomain.getResourceSet().createResource(resourceURI);
		Analysis analysis = factory.createAnalysis();
		analysis.setInputs(factory.createInputs());
		analysis.setDatasets(factory.createDatasets());
		analysis.setChartSheets(factory.createChartSheets());
		resource.getContents().add(analysis);
		return resource;
	}

	/**
	 * Prevent saving the temporary resource.
	 */
	@Override
	protected boolean isSaveable(Resource resource) {
		return resource != tempResource;
	}

	@Override
	protected void doCreatePages() {
		// add fixed pages: Inputs, Browse Data, Datasets
		FillLayout layout = new FillLayout();
        getContainer().setLayout(layout);

        getTabFolder().setMRUVisible(true);
        
		// we can load the result files now
        //XXX we should probably move this after creating the pages, but then we'll need something like browseDataPage.refresh()
        tracker.synchronize();

        createInputsPage();
        createBrowseDataPage();
        createDatasetsPage();
        
        restoreState();
        
        final CTabFolder tabfolder = getTabFolder();
		tabfolder.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				int newPageIndex = tabfolder.indexOf((CTabItem) e.item);
				pageChangedByUser(newPageIndex);
			}
		});
	}
	
	protected CTabFolder getTabFolder() {
		return (CTabFolder)getContainer();
	}
	
	@Override
	protected void initializeContentOutlineViewer(Viewer contentOutlineViewer) {
		contentOutlineViewer.setInput(getAnalysis());
	}

	/** Override base method to set the property source provider. */
	@Override
	public IPropertySheetPage getPropertySheetPage() {
		if (propertySheetPage == null) {
			// this will initialize propertySheetPage
			super.getPropertySheetPage();
			propertySheetPage.setPropertySourceProvider(
				new ScavePropertySourceProvider(adapterFactory, manager));
			return propertySheetPage;
		}
		else
			return super.getPropertySheetPage();
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

	public ScaveEditorPage getActiveEditorPage() {
		int i = getActivePage();
		if (i >= 0)
			return getEditorPage(i);
		else
			return null;
	}
	
	public ScaveEditorPage getEditorPage(int pageIndex) {
		Control page = getControl(pageIndex);
		if (page instanceof ScaveEditorPage)
			return (ScaveEditorPage)page;
		else
			return null;
	}
	
	public ChartCanvas getActiveChartCanvas() {
		ScaveEditorPage activePage = getActiveEditorPage();
		return activePage != null ? activePage.getActiveChartCanvas() : null;
	}
	
	/**
	 * Returns the edited resource.
	 */
	public Resource getResource() {
    	return (Resource)editingDomain.getResourceSet().getResources().get(0);
	}

	/**
     * Utility method: Returns the Analysis object from the resource.
     */
	public Analysis getAnalysis() {
    	Resource resource = getResource();
    	Analysis analysis = (Analysis)resource.getContents().get(0);
    	return analysis;
    }

	/**
	 * Returns the temporary analysis object. 
	 */
	public Analysis getTempAnalysis() {
		return (Analysis)tempResource.getContents().get(0);
	}
	
	/**
	 * Returns true if the object is a temporary object, i.e. it is not saved in
	 * the analysis file. 
	 */
	public boolean isTemporaryObject(EObject object) {
		return object.eResource() == tempResource;
	}
	
	/**
	 * Opens a new editor page for the {@code object} (Dataset, Chart or ChartSheet),
	 * or switches to it if already opened.
	 */
	public ScaveEditorPage open(Object object) {
		if (object instanceof Dataset)
			return openDataset((Dataset)object);
		else if (object instanceof Chart)
			return openChart((Chart)object);
		else if (object instanceof ChartSheet)
			return openChartSheet((ChartSheet)object);
		else
			return null;
	}
	
	/**
	 * Opens the given chart on a new editor page, or switches to it
	 * if already opened.
	 */
	public ScaveEditorPage openChart(Chart chart) {
		return openClosablePage(chart);
	}
	
	/**
	 * Opens the given dataset on a new editor page, or switches to it
	 * if already opened.
	 */
	public ScaveEditorPage openDataset(Dataset dataset) {
		return openClosablePage(dataset);
	}
	
	/**
	 * Opens the given chart sheet on a new editor page, or switches to it
	 * if already opened.
	 */
	public ScaveEditorPage openChartSheet(ChartSheet chartSheet) {
		return openClosablePage(chartSheet);
	}

	/**
	 * Opens the given <code>object</code> (dataset/chart/chartsheet), or
	 * switches to it if already opened.
	 */
	private ScaveEditorPage openClosablePage(EObject object) {
		int pageIndex = getOrCreateClosablePage(object);
		setActivePage(pageIndex);
		return getEditorPage(pageIndex);
	}
	
	/**
	 * Closes the page displaying the given <code>object</code>.
	 * If no such page, nothing happens. 
	 */
	public void closePage(EObject object) {
		Control page = closablePages.get(object);
		if (page != null) {
			removePage(page);
		}
	}
	
	public void showInputsPage() {
		showPage(getInputsPage());
	}

	public void showBrowseDataPage() {
		showPage(getBrowseDataPage());
	}

	public void showDatasetsPage() {
		showPage(getDatasetsPage());
	}
	
	public void showPage(ScaveEditorPage page) {
		int pageIndex = findPage(page);
		if (pageIndex >= 0)
			setActivePage(pageIndex);
	}

	public void gotoObject(Object object) {
		if (object instanceof EObject) {
			EObject eobject = (EObject)object;
			if (getAnalysis() == null || eobject.eResource() != getAnalysis().eResource())
				return;
		}
		
		ScaveEditorPage activePage = getActiveEditorPage();
		if (activePage != null) {
			if (activePage.gotoObject(object))
				return;
		}
		int activePageIndex = -1;
		for (int pageIndex = getPageCount()-1; pageIndex >= 0; --pageIndex) {
			ScaveEditorPage page = getEditorPage(pageIndex);
			if (page != null && page.gotoObject(object)) {
				activePageIndex = pageIndex;
				break;
			}
		}
		if (activePageIndex >= 0) {
			setActivePage(activePageIndex);
		}
	}
	

	public void setPageTitle(ScaveEditorPage page, String title) {
		int pageIndex = findPage(page);
		if (pageIndex >= 0)
			setPageText(pageIndex, title);
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
	
	/**
	 * Creates a closable page. These pages are closed automatically when the
	 * displayed object (chart/dataset/chart sheet) is removed from the model.
	 * Their tabs contain a small (x), so the user can also close them. 
	 */
	private int createClosablePage(EObject object) {
		ScaveEditorPage page;
		if (object instanceof Dataset)
			page = new DatasetPage(getContainer(), this, (Dataset)object);
		else if (object instanceof Chart)
			page = new ChartPage(getContainer(), this, (Chart)object);
		else if (object instanceof ChartSheet)
			page = new ChartSheetPage(getContainer(), this, (ChartSheet)object);
		else
			throw new IllegalArgumentException("Cannot create editor page for " + object);

		int pageIndex = addClosableScaveEditorPage(page);
		closablePages.put(object, page);
		return pageIndex;
	}
	
	@Override
	protected void pageClosed(Control control) {
		Assert.isTrue(closablePages.containsValue(control));
		
		// remove it from the map
		Iterator<Map.Entry<EObject,ScaveEditorPage>> entries = closablePages.entrySet().iterator();
		while (entries.hasNext()) {
			Map.Entry<EObject, ScaveEditorPage> entry = entries.next();
			if (control.equals(entry.getValue()))
				entries.remove();
		}
	}
	
	/**
	 * Returns the page displaying {@code object}.
	 * The {@code object} expected to be a Dataset, Chart or ChartSheet.
	 */
	protected ScaveEditorPage getClosableEditorPage(EObject object) {
		return closablePages.get(object);
	}
	
	/**
	 * Returns the page displaying <code>object</code>. If the object already has a page
	 * it is returned, otherwise a new page created.
	 */
	private int getOrCreateClosablePage(EObject object) {
		Control page = closablePages.get(object);
		int pageIndex = page != null ? findPage(page) : createClosablePage(object);
		Assert.isTrue(pageIndex >= 0);
		return pageIndex;
	}
	
	@Override
	public void handleSelectionChange(ISelection selection) {
		super.handleSelectionChange(selection);

		//FIXME all pages should implement a selectionChanged() method, and move all the following stuff in there!
		// propagate selection to the following viewers
		setViewerSelectionNoNotify(inputsPage.getInputFilesTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getDatasetsTreeViewer(), selection);
		setViewerSelectionNoNotify(datasetsPage.getChartSheetsTreeViewer(), selection);
		for (Control page : closablePages.values()) {
			if (page instanceof DatasetPage) {
				setViewerSelectionNoNotify(((DatasetPage)page).getDatasetTreeViewer(), selection);
			}
		}
		
	}
	
	class ScaveEditorContentOutlinePage extends MyContentOutlinePage
	{
		@Override
		public void createControl(Composite parent) {
			super.createControl(parent);
			TreeViewer viewer = getTreeViewer();
			Tree tree = viewer.getTree();
			if (tree != null) {
				tree.addSelectionListener(new SelectionAdapter () {
					public void widgetDefaultSelected(SelectionEvent e) {
						if (e.item instanceof TreeItem) {
							TreeItem item = (TreeItem)e.item;
							open(item.getData());
						}
					}
				});
			}
		}
	}

	
	@Override
	public IContentOutlinePage getContentOutlinePage() {
		if (contentOutlinePage == null) {
			contentOutlinePage = new ScaveEditorContentOutlinePage();
			contentOutlinePage.addSelectionChangedListener(selectionChangedListener);
			contentOutlinePage.addSelectionChangedListener(new ISelectionChangedListener() {
				public void selectionChanged(SelectionChangedEvent event) {
					contentOutlineSelectionChanged(event.getSelection());
				}
			});
		}

		return contentOutlinePage;
	}

	protected void contentOutlineSelectionChanged(ISelection selection) {
		if (selection instanceof IStructuredSelection) {
			Object object = ((IStructuredSelection)selection).getFirstElement();
			//System.out.println("Selected: "+object);
			if (object != null)
				gotoObject(object);
		}
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
			Command command = AddCommand.create(getEditingDomain(), inputs, pkg.getInputs_Inputs(), inputFile);
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
	
	/**
	 * Utility function to access the active editor in the workbench.
	 */
	public static ScaveEditor getActiveScaveEditor(IWorkbench workbench) {
		if (workbench.getActiveWorkbenchWindow() != null) {
			IWorkbenchPage page = workbench.getActiveWorkbenchWindow().getActivePage();
			if (page != null) {
				IEditorPart part = page.getActiveEditor();
				if (part instanceof ScaveEditor)
					return (ScaveEditor)part;
			}
		}
		return null;
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

	/**
	 * Updates the pages.
	 * Registered as a listener on model changes.
	 */
 	@SuppressWarnings("unchecked")
	private void updatePages(Notification notification) {
		if (notification.isTouch())
			return;
		
		// close pages whose content was deleted, except temporary datasets/charts
		// (temporary objects are not deleted, but they can be moved into the persistent analysis)
		if (notification.getNotifier() instanceof EObject && !isTemporaryObject((EObject)notification.getNotifier())) {
			List<Object> deletedObjects = null;
			switch (notification.getEventType()) {
			case Notification.REMOVE: 
				deletedObjects = new ArrayList<Object>();
				deletedObjects.add(notification.getOldValue());
				break;
			case Notification.REMOVE_MANY:
				deletedObjects = (List<Object>)notification.getOldValue();
				break;
			}
		
			if (deletedObjects != null) {
				for (Object object : deletedObjects) {
					if (object instanceof EObject) {
						TreeIterator<EObject> contents = ((EObject)object).eAllContents();
						// iterate on contents including object
						for (Object next = object; next != null; next = contents.hasNext() ? contents.next() : null) {
							if (next instanceof Dataset) {
								closePage((Dataset)next);
							}
							else if (next instanceof Chart) {
								closePage((Chart)next);
								contents.prune();
							}
							else if (next instanceof ChartSheet) {
								closePage((ChartSheet)next);
								contents.prune();
							}
						}
					}
				}
			}
		}

		// update contents of pages
		int pageCount = getPageCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
			Control control = getControl(pageIndex);
			if (control instanceof ScaveEditorPage) {
				ScaveEditorPage page = (ScaveEditorPage)control;
				page.updatePage(notification);
			}
		}
	}
	
	@Override
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		Control page = getControl(newPageIndex);
		if (page instanceof ScaveEditorPage) {
			((ScaveEditorPage)page).pageSelected();
		}
		fakeSelectionChange();
	}

	/**
	 * Pretends that a selection change has taken place. This is e.g. useful for updating
	 * the enabled/disabled/pushed etc state of actions (AbstractScaveAction) whose 
	 * isApplicable() method is hooked on selection changes.
	 */
	public void fakeSelectionChange() {
		setSelection(getSelection());
	}
	
	/*
	 * PageId
	 */
	private static final String TEMPORARY = "t:";
	private static final String PERSISTENT = "p:";
	
	String getPageId(ScaveEditorPage page) {
		if (page == null)
			return null;
		else if (page.equals(inputsPage))
			return "Inputs";
		else if (page.equals(browseDataPage))
			return "BrowseData";
		else if (page.equals(datasetsPage))
			return "Datasets";
		else {
			for (Map.Entry<EObject, ScaveEditorPage> entry : closablePages.entrySet()) {
				EObject object = entry.getKey();
				ScaveEditorPage editorPage = entry.getValue();
				if (page.equals(editorPage)) {
					Resource resource = object.eResource();
					String prefix = resource == tempResource ? TEMPORARY : PERSISTENT;
					String uri = resource != null ? resource.getURIFragment(object) : null;
					return uri != null ? prefix + uri : null;
				}
			}
		}
		return null;
	}
	
	ScaveEditorPage restorePage(String pageId) {
		if (pageId == null)
			return null;
		if (pageId.equals("Inputs")) {
			setActivePage(findPage(inputsPage));
			return inputsPage;
		}
		else if (pageId.equals("BrowseData")) {
			setActivePage(findPage(browseDataPage));
			return browseDataPage;
		}
		else if (pageId.equals("Datasets")) {
			setActivePage(findPage(datasetsPage));
			return datasetsPage;
		}
		else {
			EObject object = null;
			String uri = null;
			Resource resource = null;
			if (pageId.startsWith(TEMPORARY)) {
				uri = pageId.substring(TEMPORARY.length());
				resource = tempResource;
			}
			else if (pageId.startsWith(PERSISTENT)) {
				uri = pageId.substring(PERSISTENT.length());
				resource = getResource();
			}
			
			try {
				if (resource != null && uri != null)
					object = resource.getEObject(uri);
			} catch (Exception e) {}

			if (object != null)
				return open(object);
		}
		return null;
	}
	
	/*
	 * Per input persistent state.
	 */
	private IFile getInputFile() {
		IEditorInput input = getEditorInput();
		if (input instanceof IFileEditorInput)
			return ((IFileEditorInput)input).getFile();
		else
			return null;
	}
	
	private static final String
		ACTIVE_PAGE = "ActivePage",
		PAGE = "Page",
		PAGE_ID = "PageId";
	
	private void saveState(IMemento memento) {
		memento.putInteger(ACTIVE_PAGE, getActivePage());
		for (EObject openedObject : closablePages.keySet()) {
			ScaveEditorPage page = closablePages.get(openedObject);
			IMemento pageMemento = memento.createChild(PAGE);
			pageMemento.putString(PAGE_ID, getPageId(page));
			page.saveState(pageMemento);
		}
	}
	
	private void restoreState(IMemento memento) {
		for (IMemento pageMemento : memento.getChildren(PAGE)) {
			String pageId = pageMemento.getString(PAGE_ID);
			if (pageId != null) {
				ScaveEditorPage page = restorePage(pageId);
					if (page != null)
						page.restoreState(pageMemento);
			}
		}
		int activePage = memento.getInteger(ACTIVE_PAGE);
		if (activePage >= 0 && activePage < getPageCount())
			setActivePage(activePage);
	}
	
	private void saveState() {
		try {
			IFile file = getInputFile();
			if (file != null) {
				ScaveEditorMemento memento = new ScaveEditorMemento();
				saveState(memento);
				memento.save(file);
			}
		} catch (Exception e) {
//			MessageDialog.openError(getSite().getShell(),
//									"Saving editor state",
//									"Error occured while saving editor state: "+e.getMessage());
			ScavePlugin.logError(e);
		}
	}
	
	private void restoreState() {
		try {
			IFile file = getInputFile();
			if (file != null) {
				ScaveEditorMemento memento = new ScaveEditorMemento(file);
				restoreState(memento);
			}
		}
		catch (CoreException e) {
			ScavePlugin.log(e.getStatus());
		}
		catch (Exception e) {
//			MessageDialog.openError(getSite().getShell(),
//					"Restoring editor state",
//					"Error occured while restoring editor state: "+e.getMessage());
			ScavePlugin.logError(e);
		}
	}

	/*
	 * Navigation
	 */
	public INavigationLocation createEmptyNavigationLocation() {
		return new ScaveNavigationLocation(this, true);
	}

	public INavigationLocation createNavigationLocation() {
		return new ScaveNavigationLocation(this, false);
	}
	
	public void markNavigationLocation() {
		getSite().getPage().getNavigationHistory().markLocation(this);
	}
	
	public void pageChangedByUser(int newPageIndex) {
		Control page = getControl(newPageIndex);
		if (page instanceof ScaveEditorPage) {
			markNavigationLocation();
		}
	}
}


