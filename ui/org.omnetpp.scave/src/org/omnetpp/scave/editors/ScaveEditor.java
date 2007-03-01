package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.TreeIterator;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.xmi.XMIResource;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.views.properties.IPropertySheetPage;
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
//FIXME merge ChartPage and ChartPage2

//TODO copy/paste doesn't work on the model
//TODO Chart sheet: should modify order of charts in the ChartSheet#getCharts collection
//TODO chart page: "view numbers" feature
//TODO label provider: print attributes in "quotes"
public class ScaveEditor extends AbstractEMFModelEditor {

	private InputsPage inputsPage;
	private BrowseDataPage browseDataPage;
	private DatasetsAndChartsPage datasetsPage;
	private Map<EObject,ScaveEditorPage> closablePages = new HashMap<EObject,ScaveEditorPage>();

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
	public void init(IEditorSite site, IEditorInput editorInput) {
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

		tracker = new ResultFilesTracker(manager, analysis.getInputs()); //XXX must ensure that Inputs never gets deleted or replaced!!!

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
		URI resourceURI = URI.createPlatformResourceURI(tempResourcePath.toString());;
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
		if (i >= 0) {
			Control page = getControl(i);
			if (page instanceof ScaveEditorPage)
				return (ScaveEditorPage)page;
		}
		return null;
	}

	/**
     * Utility method: Returns the Analysis object from the resource.
     */
	public Analysis getAnalysis() {
    	XMIResource resource = (XMIResource)editingDomain.getResourceSet().getResources().get(0);
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
	 * Opens the given chart on a new editor page, or switches to it
	 * if already opened.
	 */
	public void openChart(Chart chart) {
		openClosablePage(chart);
	}
	
	/**
	 * Opens the given dataset on a new editor page, or switches to it
	 * if already opened.
	 */
	public void openDataset(Dataset dataset) {
		openClosablePage(dataset);
	}
	
	/**
	 * Opens the given chart sheet on a new editor page, or switches to it
	 * if already opened.
	 */
	public void openChartSheet(ChartSheet chartSheet) {
		openClosablePage(chartSheet);
	}

	/**
	 * Opens the given <code>object</code> (dataset/chart/chartsheet), or
	 * switches to it if already opened.
	 */
	private void openClosablePage(EObject object) {
		int pageIndex = getOrCreateClosablePage(object);
		setActivePage(pageIndex);
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
	 * displayed object (chart/dataset/chartsheet) is removed from the model.
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
			Map.Entry entry = entries.next();
			if (control.equals(entry.getValue()))
				entries.remove();
		}
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
 	private void updatePages(Notification notification) {
		if (notification.isTouch())
			return;
		
		// close pages whose content was deleted, except temporary datasets/charts
		// (temporary objects are not deleted, but they can be moved into the persistent analysis)
		if (notification.getNotifier() instanceof EObject && !isTemporaryObject((EObject)notification.getNotifier())) {
			List<EObject> deletedObjects = null;
			switch (notification.getEventType()) {
			case Notification.REMOVE: 
				deletedObjects = new ArrayList<EObject>();
				deletedObjects.add((EObject)notification.getOldValue());
				break;
			case Notification.REMOVE_MANY:
				deletedObjects = (List<EObject>)notification.getOldValue();
				break;
			}
		
			if (deletedObjects != null) {
				for (EObject object : deletedObjects) {
					TreeIterator contents = object.eAllContents();
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
		if (page instanceof ScaveEditorPage)
			((ScaveEditorPage)page).pageSelected();
		
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
}


