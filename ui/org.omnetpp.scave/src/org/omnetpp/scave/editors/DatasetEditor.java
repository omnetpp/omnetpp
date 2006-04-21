package org.omnetpp.scave.editors;


import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceAdapter;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorActionBarContributor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.EditorActionBarContributor;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.properties.PropertySourceIO;
import org.omnetpp.common.xml.XMLWriter;
import org.omnetpp.scave.charting.InteractiveChart;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.IDListIO;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDListTransfer;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.ModelChangeListener;
import org.omnetpp.scave.panel.DatasetContentOutlinePage;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.plugin.ScavePlugin;
import org.omnetpp.scave.wizards.NonExistingFileEditorInput;
import static org.omnetpp.scave.model.IDListXMLConsts.*;


/**
 * Base class for all dataset editors.
 *
 * TODO consider whether we really need MultiPageEditorPart as base class.
 * It assumes that it manages a set of "real" editors, and basically delegates
 * all work there. This isn't useful here, since our pages are not IEditorParts.
 *
 * FIXME better mapping between workspace files and absolute OS paths.
 * e.g use IContainer[] findContainersForLocation(IPath location) on IWorkspaceRoot.
 *
 * TODO file references inside .sdataset etc files should be relative to the file itself
 * In ResultFileManager, use paths relative to the workspace root?
 */
public abstract class DatasetEditor extends MultiPageEditorPart implements IResourceChangeListener {

	/** Allows us common handling of scalars and vectors */
	private IDatasetStrategy strategy;

	/** The dataset being edited (the MVC model) */
	private IDListModel dataset = new IDListModel();

	/** Set to true if the dataset changed since the last save */
	private boolean isModelDirty = false;

	/** The (optional) chart page only gets updated if (when) visible */
	private boolean chartNeedsUpdate = false;

	/** The Data panel that appears on the first page */
	protected FilterPanel filterPanel;

	/** The Chart which appears on the second page, or null if this is just a dataset editor */
	protected InteractiveChart chartWrapper;

	/** Page for the Outline view */
	private DatasetContentOutlinePage outlinePage;

	/**
	 * Default constructor.
	 */
	public DatasetEditor(IDatasetStrategy strategy) {
		super();
		this.strategy = strategy;
		ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
	}

	/**
	 * The <code>MultiPageEditorExample</code> implementation of this method
	 * checks that the input is an instance of <code>IFileEditorInput</code>.
	 */
	public void init(IEditorSite site, IEditorInput editorInput)
	throws PartInitException {
		System.out.println("Editor.init() with "+editorInput.toString());
		super.init(site, editorInput);
		setPartName(editorInput.getName());

		// if it's an "Untitled" document, don't try to load it
		if (! (editorInput instanceof NonExistingFileEditorInput)) {

			// verify it's a file, then load it
			if (!(editorInput instanceof IFileEditorInput))
				throw new PartInitException("Invalid Input: Must be IFileEditorInput");

			// actually load the file
			// TODO this should be a "long-running task"
			IDList result = openDoc(editorInput);
			if (result!=null)
				getDataset().set(result);
		}

		// add change listener to dataset
		dataset.addListener(new ModelChangeListener() {
			public void handleChange() {
				markDirty();
				updateChart();
			}});
		
		getSite().setSelectionProvider(new SelectionProvider());
	}

	private IDList openDoc(IEditorInput editorInput) {
		IFileEditorInput fileInput = (IFileEditorInput)editorInput;
		java.io.File file = fileInput.getFile().getLocation().toFile();
		IDList result = null;
		try {
			result = IDListIO.load(file, getProgressMonitor());
		} catch (Exception e) {
			MessageDialog.openError(new Shell(), "Error opening file",
					"Error opening file "+file.getName()+": "+e.getMessage());
		}
		return result;
	}

	/**
	 * Creates the scalars/vectors table page of the multi-page editor.
	 */
	protected void createDatasetPage() {
		try {
			filterPanel = strategy.createFilterPanel(getContainer(), SWT.NONE);
			filterPanel.setStatusLineManager(getStatusLineManager());
			filterPanel.setDataset(getDataset());
			int index = addPage(filterPanel.getPanel());
			setPageText(index, "Data");
		} catch (Exception e) {
			MessageDialog.openError(getSite().getShell(),
					"Error", "Error creating dataset page");
		}
	}

	/**
	 * The <code>MultiPageEditorPart</code> implementation of this
	 * <code>IWorkbenchPart</code> method disposes all nested editors.
	 * Subclasses may extend.
	 */
	public void dispose() {
		ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
		filterPanel.dispose();
		getSite().setSelectionProvider(null);
		super.dispose();
	}

	/**
	 * Returns the dataset being edited in this editor.
	 */
	public IDListModel getDataset() {
		return dataset;
	}

	/**
	 * Replace the contents of the current dataset.
	 */
	public void setDataset(IDList dataset) {
		dataset.set(dataset);
	}

	/**
	 * Returns the filter panel on the "Data" page of the multi-page editor
	 */
	public FilterPanel getFilterPanel() {
		return filterPanel;
	}

	/**
	 * Regenerate chart contents if chart is visible, otherwise
	 * just remember that it'll have to be done sometime.
	 */
	public void updateChart() {
		if (chartWrapper!=null) {
			if (getActivePage()!=1) {
				chartNeedsUpdate = true;
			} else {
				doUpdateChart();
				chartNeedsUpdate = false;
			}
		}
	}

	/**
	 * Regenerates chart contents based on the current dataset.
	 * This method has to be overridden in subclasses to do the
	 * actual work.
	 */
	public void doUpdateChart() {
	}

	/**
	 * Calculates the contents of page 2 when the it is activated.
	 */
	protected void pageChange(int newPageIndex) {
		super.pageChange(newPageIndex);
		if (chartNeedsUpdate)
			updateChart();
	}

	/**
	 * Override MultiPageEditor's version
	 */
	public boolean isDirty() {
		return isModelDirty;
	}

	/**
	 * Saves the multi-page editor's document.
	 */
	public void doSave(IProgressMonitor progressMonitor) {
		//it it's "Untitled", perform "Save as"
		IFileEditorInput fileInput = (IFileEditorInput)getEditorInput();
		if (fileInput instanceof NonExistingFileEditorInput) {
			doSaveAs();
			return;
		}

		// figure out file name
		String fileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();
		boolean success = performSave(fileName, progressMonitor);
		if (success) {
			isModelDirty = false;
			firePropertyChange(PROP_DIRTY);
		}
	}

	/**
	 * Saves the multi-page editor's document as another file.
	 * Also updates the text for page 0's tab, and updates this multi-page editor's input
	 * to correspond to the nested editor's.
	 */
	public void doSaveAs() {
		performSaveAs(getProgressMonitor());
	}
	
	protected boolean performSave(String fileName, IProgressMonitor progressMonitor) {
		// FIXME use progressmonitor AND implement as "long running task"!
		Exception exception = null;
		String errorMsg = null;
		try {
			OutputStream os = new FileOutputStream(fileName);
			XMLWriter writer = new XMLWriter(os, "UTF-8", 2);
			writer.writeXMLHeader();
			writer.writeStartElement(EL_SCAVE);
	        writer.writeAttribute(ATT_VERSION, FILEFORMAT_VERSION);
			IDListIO.save(dataset.get(), writer, progressMonitor);
			saveChart(writer);
			writer.writeEndElement(EL_SCAVE);
			writer.close();
			return true;
		} catch (FileNotFoundException e) {
			errorMsg = "Cannot open file: ";
			exception = e;
			return false;
		} catch (IOException e) {
			errorMsg = "Error when saving file: ";
			exception = e;
			return false;
		} finally {
			if (exception != null)
				MessageDialog.openError(new Shell(), "Error saving file",
						errorMsg + fileName + " (" + exception.getMessage() + ")");
				
				
		}
		
	}
	
	public void saveChart(XMLWriter writer) throws IOException {
		if (chartWrapper != null) {
			writer.writeStartElement("chart");
			PropertySource propertySource = new ChartProperties(chartWrapper);
			PropertySourceIO.save(propertySource, writer);
			writer.writeEndElement("chart");
		}
	}
	

	/**
	 * Asks the user for the workspace path of a file resource and saves the
	 * document there. (Method source code copied from <code>TextEditor</code>,
	 * and simplified significantly. Look it up for more sophistication.)
	 */
	protected void performSaveAs(IProgressMonitor progressMonitor) {
		Shell shell= getSite().getShell();
		IEditorInput input = getEditorInput();

		SaveAsDialog dialog= new SaveAsDialog(shell);

		IFile original= (input instanceof IFileEditorInput) ? ((IFileEditorInput) input).getFile() : null;
		if (original != null)
			dialog.setOriginalFile(original);

		dialog.create();

//		IDocumentProvider provider= getDocumentProvider();
//		if (provider == null) {
//		// editor has programmatically been  closed while the dialog was open
//		return;
//		}
//
//		if (provider.isDeleted(input) && original != null) {
//		// TODO use resources: MessageFormat.format(TextEditorMessages.Editor_warning_save_delete, new Object[] { original.getName() });
//		String message= "File "+original.getName()+" has been deleted on the disk. Still save it?";
//		dialog.setErrorMessage(null);
//		dialog.setMessage(message, IMessageProvider.WARNING);
//		}

		if (dialog.open() == Window.CANCEL) {
			if (progressMonitor != null)
				progressMonitor.setCanceled(true);
			return;
		}

		IPath filePath= dialog.getResult();
		if (filePath == null) {
			if (progressMonitor != null)
				progressMonitor.setCanceled(true);
			return;
		}

		IWorkspace workspace= ResourcesPlugin.getWorkspace();
		IFile file= workspace.getRoot().getFile(filePath);
		final IFileEditorInput newInput = new FileEditorInput(file);

		String fileName = newInput.getFile().getLocation().toFile().getAbsolutePath();
		boolean success = performSave(fileName, progressMonitor);
		if (success) {
			setInput(newInput);
			setPartName(newInput.getName());
			isModelDirty = false;
			firePropertyChange(PROP_DIRTY);
			firePropertyChange(PROP_INPUT); // FIXME navigator doesn't get notified that a new file has been created
		}

		if (progressMonitor != null)
			progressMonitor.setCanceled(!success);
	}

	/* (non-Javadoc)
	 * Method declared on IEditorPart.
	 */
	public boolean isSaveAsAllowed() {
		return true;
	}

	/**
	 * Marks the editor contents as dirty (if it's not already so).
	 */
	private void markDirty() {
		if (isModelDirty)
			return;

		isModelDirty = true;
		firePropertyChange(PROP_DIRTY);
	}

	/* (non-Javadoc)
	 * Method declared on IEditorPart
	 */
	public void gotoMarker(IMarker marker) {
		setActivePage(0);
		IDE.gotoMarker(getEditor(0), marker);
	}

	/**
	 * Called on resource change events.
	 */
	public void resourceChanged(final IResourceChangeEvent event){
		//TODO Closes all project files on project close.
		//if(event.getType() == IResourceChangeEvent.PRE_CLOSE){
		//	Display.getDefault().asyncExec(new Runnable(){
		//		public void run(){
		//			IWorkbenchPage[] pages = getSite().getWorkbenchWindow().getPages();
		//			for (int i = 0; i<pages.length; i++){
		//				if(((FileEditorInput)editor.getEditorInput()).getFile().getProject().equals(event.getResource())){
		//					IEditorPart editorPart = pages[i].findEditor(editor.getEditorInput());
		//					pages[i].closeEditor(editorPart,true);
		//				}
		//			}
		//		}
		//	});
		//}
	}

	/**
	 * Method comes from WorkbenchPart, eventually from IAdaptable.
	 */
	public Object getAdapter(Class required) {
		if (IContentOutlinePage.class.equals(required)) {
			if (outlinePage == null) {
				outlinePage = new DatasetContentOutlinePage(getDataset());
			}
			return outlinePage;
		}
		else if (required == IPropertySheetPage.class)
		{
			PropertySheetPage page =  new PropertySheetPage();
			page.setPropertySourceProvider(new IPropertySourceProvider() {
				public IPropertySource getPropertySource(Object object) {
					if (object instanceof IPropertySource)
						return (IPropertySource)object;
					else if (object == chartWrapper)
						return new ChartProperties(chartWrapper);
					else
						return null;
				}
			});
			return page;
		}

		return super.getAdapter(required);
	}

	protected void setupDragSource(Control dragSourceControl) {
		// make table d&d source for IDLists
		DragSource dragSrc = new DragSource(dragSourceControl, DND.DROP_DEFAULT | DND.DROP_COPY);
		dragSrc.setTransfer(new Transfer[] { IDListTransfer.getInstance() });
		dragSrc.addDragListener(new DragSourceAdapter() {
			public void dragSetData(DragSourceEvent event) {
				event.data = filterPanel.getSelection();
			}
		});
	}

	protected void setupDropTarget(Control dropTargetControl) {
		// make panel d&d target for IDLists and files (FIXME find better place for this code)
		DropTarget dropTarget = new DropTarget(dropTargetControl, DND.DROP_DEFAULT | DND.DROP_COPY);
		dropTarget.setTransfer(new Transfer[] {
				FileTransfer.getInstance(),
				IDListTransfer.getInstance()
		});
		dropTarget.addDropListener(new DropTargetAdapter() {
			public void dragEnter(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT) {
					event.detail = DND.DROP_COPY;
				}
			}
			public void dragOperationChanged(DropTargetEvent event) {
				if (event.detail == DND.DROP_DEFAULT) {
					event.detail = DND.DROP_COPY;
				}
			}
			public void drop(DropTargetEvent event) {
				if (event.data instanceof String[]) {
					String [] fileNames = (String[])event.data;
					for (int i=0; i<fileNames.length; i++)
						loadFileAndAddContentsToDataset(fileNames[i]);
				} else if (event.data instanceof IDList) {
					IDList data = (IDList)event.data;
					if (strategy.isCompatible(data)) {
						getDataset().merge(data);
					} else {
						MessageDialog.openError(new Shell(), "Error",
						"Cannot drop or paste scalars into a vector dataset, or vica versa.");
					}
				}
			}
		});
	}

	/**
	 * Load a result file (.vec or .sca), and add its contents to the current panel.
	 * (The contents of the loaded file will be available for all other
	 * editors too, for insertion via the ExtendDatasetDialog.)
	 */
	public void loadFileAndAddContentsToDataset(String fileName) {
		try {
			ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
			File file = strategy.loadResultFile(fileName);
			IDList newData = resultFileManager.getDataInFile(file);
			dataset.merge(newData);
		} catch (RuntimeException e) {
			MessageDialog.openError(getSite().getShell(), "Error",
					"Error loading " + fileName + ": " + e.getMessage());
		}
	}

	/**
	 * Returns the progress monitor related to this editor.
	 * (Method source code copied from AbstractTextEditor.)
	 */
	protected IProgressMonitor getProgressMonitor() {

		IProgressMonitor pm = null;

		IStatusLineManager manager = getStatusLineManager();
		if (manager != null)
			pm = manager.getProgressMonitor();

		return pm != null ? pm : new NullProgressMonitor();
	}

	/**
	 * Returns the status line manager of this editor.
	 * (Method source code copied from AbstractTextEditor.)
	 */
	private IStatusLineManager getStatusLineManager() {

		IEditorActionBarContributor contributor = getEditorSite().getActionBarContributor();
		if (!(contributor instanceof EditorActionBarContributor))
			return null;

		IActionBars actionBars = ((EditorActionBarContributor) contributor).getActionBars();
		if (actionBars == null)
			return null;

		return actionBars.getStatusLineManager();
	}
	
	class SelectionProvider implements ISelectionProvider
	{
		public SelectionProvider() {
		}
		
		public void addSelectionChangedListener(ISelectionChangedListener listener) {
			// TODO Auto-generated method stub
		}
	
		public void removeSelectionChangedListener(ISelectionChangedListener listener) {
			// TODO Auto-generated method stub
		}
	
		public ISelection getSelection() {
			if (chartWrapper != null)
				return new StructuredSelection(chartWrapper);
			else
				return null;
		}
	
		public void setSelection(ISelection selection) {
		}
	}
}
