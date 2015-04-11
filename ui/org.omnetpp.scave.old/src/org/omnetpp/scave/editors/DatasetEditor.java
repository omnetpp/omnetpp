/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;


import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.dnd.DND;
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
import org.omnetpp.common.xml.CombinedSAXHandler;
import org.omnetpp.common.xml.XMLWriter;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDListTransfer;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.panel.DatasetContentOutlinePage;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.plugin.ScavePlugin;
import org.omnetpp.scave.wizards.NonExistingFileEditorInput;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
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
public abstract class DatasetEditor extends MultiPageEditorPart
    implements IResourceChangeListener {

    /** The dataset being edited (the MVC model) */
    private IDListModel dataset = new IDListModel();

    private IDatasetStrategy strategy;

    /** Set to true by the pages */
    private boolean isDirty = false;

    /** Page for the Outline view */
    private DatasetContentOutlinePage outlinePage;

    private IDatasetEditorPage[] pages;

    private FilterPanelPage filterPanelPage;

    private IDatasetEditorPage activePage;

    /**
     * Default constructor.
     */
    public DatasetEditor(IDatasetStrategy strategy, IDatasetEditorPage[] pages) {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);

        this.pages = pages;
        this.strategy = strategy;

        filterPanelPage = null;
        for (IDatasetEditorPage page : pages) {
            page.setEditor(this);
            if (page instanceof FilterPanelPage)
                filterPanelPage = (FilterPanelPage)page;
        }
    }

    public IDListModel getDataset() {
        return dataset;
    }

    /**
     * The <code>MultiPageEditorExample</code> implementation of this method
     * checks that the input is an instance of <code>IFileEditorInput</code>.
     */
    public void init(IEditorSite site, IEditorInput editorInput)
    throws PartInitException {
        System.out.println("Editor.init() with "+editorInput.toString());

        if (!(editorInput instanceof NonExistingFileEditorInput) &&
            !(editorInput instanceof IFileEditorInput))
            throw new PartInitException("Invalid Input: Must be IFileEditorInput");

        super.init(site, editorInput);
        setPartName(editorInput.getName());

        for (IDatasetEditorPage page : pages) {
            page.init();
        }
    }

    /**
     * The <code>MultiPageEditorPart</code> implementation of this
     * <code>IWorkbenchPart</code> method disposes all nested editors.
     * Subclasses may extend.
     */
    public void dispose() {
        for (IDatasetEditorPage page : pages) {
            page.dispose();
        }

        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        super.dispose();
    }


    /**
     * Creates the pages of the multi-page editor.
     */
    protected void createPages() {
        for (IDatasetEditorPage page : pages) {
            int index = addPage(page.createPageControl(getContainer()));
            setPageText(index, page.getPageTitle());
        }

        IEditorInput editorInput = getEditorInput();
        if (editorInput != null &&
            !(editorInput instanceof NonExistingFileEditorInput))
            openDoc(editorInput);

        for (IDatasetEditorPage page : pages)
            page.finalizePage();

        int lastPageIndex = getPageCount() - 1;
        if (lastPageIndex >= 0)
            setActivePage(lastPageIndex);
    }


    protected void openDoc(IEditorInput editorInput) {
        // if it's an "Untitled" document, don't try to load it
        if (editorInput instanceof IFileEditorInput) {
            IFileEditorInput fileInput = (IFileEditorInput)editorInput;
            final java.io.File file = fileInput.getFile().getLocation().toFile();

            SafeRunnable.run(new SafeRunnable("Error opening file "+file.getName()) {
                public void run() {
                    loadFromXml(file, getProgressMonitor());
                }
            });
        }
    }

    public void pageChange(int newPageIndex) {
        if (activePage != null)
            activePage.deactivate();
        activePage = pages[newPageIndex];
        activePage.activate();
    }

    /**
     * Override MultiPageEditor's version
     */
    public boolean isDirty() {
        return isDirty;
    }

    /*
     * Persistency interface
     */

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
        boolean success = saveToXml(fileName, progressMonitor);
        if (success) {
            isDirty = false;
            firePropertyChange(PROP_DIRTY);
        }
    }

    /**
     * Saves the multi-page editor's document as another file.
     * Also updates the text for page 0's tab, and updates this multi-page editor's input
     * to correspond to the nested editor's.
     *
     * Asks the user for the workspace path of a file resource and saves the
     * document there. (Method source code copied from <code>TextEditor</code>,
     * and simplified significantly. Look it up for more sophistication.)
     */
    public void doSaveAs() {
        IProgressMonitor progressMonitor = getProgressMonitor();

        Shell shell= getSite().getShell();
        IEditorInput input = getEditorInput();

        SaveAsDialog dialog= new SaveAsDialog(shell);

        IFile original= (input instanceof IFileEditorInput) ? ((IFileEditorInput) input).getFile() : null;
        if (original != null)
            dialog.setOriginalFile(original);

        dialog.create();

//      IDocumentProvider provider= getDocumentProvider();
//      if (provider == null) {
//      // editor has programmatically been  closed while the dialog was open
//      return;
//      }
//
//      if (provider.isDeleted(input) && original != null) {
//      // TODO use resources: MessageFormat.format(TextEditorMessages.Editor_warning_save_delete, new Object[] { original.getName() });
//      String message= "File "+original.getName()+" has been deleted on the disk. Still save it?";
//      dialog.setErrorMessage(null);
//      dialog.setMessage(message, IMessageProvider.WARNING);
//      }

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
        boolean success = saveToXml(fileName, progressMonitor);
        if (success) {
            setInput(newInput);
            setPartName(newInput.getName());
            isDirty = false;
            firePropertyChange(PROP_DIRTY);
            firePropertyChange(PROP_INPUT); // FIXME navigator doesn't get notified that a new file has been created
        }

        if (progressMonitor != null)
            progressMonitor.setCanceled(!success);
    }

    /*
     * Persistency implementation
     */

    private boolean saveToXml(String fileName, IProgressMonitor progressMonitor) {
        // FIXME use progressmonitor AND implement as "long running task"!
        Exception exception = null;
        String errorMsg = null;
        try {
            OutputStream os = new FileOutputStream(fileName);
            XMLWriter writer = new XMLWriter(os, "UTF-8", 2);
            writer.writeXMLHeader();
            writer.writeStartElement(EL_SCAVE);
            writer.writeAttribute(ATT_VERSION, FILEFORMAT_VERSION);

            for (IDatasetEditorPage page : pages)
                page.save(writer, progressMonitor);

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

    private void loadFromXml(java.io.File file, IProgressMonitor progressMonitor) {
        try {
            // TODO use progressMonitor
            long t0 = System.currentTimeMillis();
            SAXParserFactory spfactory = SAXParserFactory.newInstance();
            SAXParser parser = spfactory.newSAXParser();
            CombinedSAXHandler handler = new CombinedSAXHandler();

            Map<String,ContentHandler> handlers = new HashMap<String,ContentHandler>(2 * pages.length);
            for (IDatasetEditorPage page : pages)
                handlers.putAll(page.getLoader(progressMonitor));
            handler.addContentHandlers(handlers);

            parser.parse(file, handler);
            System.out.println("loading "+file.getName()+": " + (System.currentTimeMillis()-t0));
        } catch (ParserConfigurationException e) {
            throw new RuntimeException("internal error: "+e.getMessage(), e);
        } catch (SAXException e) {
            throw new RuntimeException("error parsing XML: "+e.getMessage(), e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /* (non-Javadoc)
     * Method declared on IEditorPart.
     */
    public boolean isSaveAsAllowed() {
        return isDirty();
    }

    /**
     * Marks the editor contents as dirty (if it's not already so).
     */
    public void markDirty() {
        if (isDirty)
            return;

        isDirty = true;
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
        //  Display.getDefault().asyncExec(new Runnable(){
        //      public void run(){
        //          IWorkbenchPage[] pages = getSite().getWorkbenchWindow().getPages();
        //          for (int i = 0; i<pages.length; i++){
        //              if(((FileEditorInput)editor.getEditorInput()).getFile().getProject().equals(event.getResource())){
        //                  IEditorPart editorPart = pages[i].findEditor(editor.getEditorInput());
        //                  pages[i].closeEditor(editorPart,true);
        //              }
        //          }
        //      }
        //  });
        //}
    }

    /**
     * Method comes from WorkbenchPart, eventually from IAdaptable.
     */
    public Object getAdapter(Class required) {
        if (IContentOutlinePage.class.equals(required)) {
            if (outlinePage == null) {
                outlinePage = new DatasetContentOutlinePage(dataset);
            }
            return outlinePage;
        }
        else {
            for (IDatasetEditorPage page : pages) {
                if (page instanceof IAdaptable) {
                    Object adapter = ((IAdaptable)page).getAdapter(required);
                    if (adapter != null)
                        return adapter;
                }
            }
        }


        return super.getAdapter(required);
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
    public IStatusLineManager getStatusLineManager() {

        IEditorActionBarContributor contributor = getEditorSite().getActionBarContributor();
        if (!(contributor instanceof EditorActionBarContributor))
            return null;

        IActionBars actionBars = ((EditorActionBarContributor) contributor).getActionBars();
        if (actionBars == null)
            return null;

        return actionBars.getStatusLineManager();
    }

    public FilterPanel getFilterPanel() {
        if (filterPanelPage != null)
            return filterPanelPage.getFilterPanel();
        return null;
    }

    public void setupDropTarget(Control dropTargetControl) {
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
                }
                else if (event.data instanceof IDList) {
                    IDList data = (IDList)event.data;
                    if (strategy.isCompatible(data)) {
                        getDataset().merge(data);
                    }
                    else {
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
    public void loadFileAndAddContentsToDataset(final String fileName) {
        SafeRunnable.run(new SafeRunnable("Error loading " + fileName) {
            public void run() {
                ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
                File file = strategy.loadResultFile(fileName);
                IDList newData = resultFileManager.getDataInFile(file);
                dataset.merge(newData);
            }
        });
    }
}
