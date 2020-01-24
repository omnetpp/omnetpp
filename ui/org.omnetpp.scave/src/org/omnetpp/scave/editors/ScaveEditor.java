/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Callable;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.util.LocalSelectionTransfer;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.StructuredViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabFolder2Adapter;
import org.eclipse.swt.custom.CTabFolderEvent;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.IActionBars;
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
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.PageSite;
import org.eclipse.ui.views.contentoutline.ContentOutline;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySheetEntry;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheet;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheetSorter;
import org.omnetpp.common.Debug;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.common.ui.MultiPageEditorPartExt;
import org.omnetpp.common.ui.ViewerDragAdapter;
import org.omnetpp.common.util.DetailedPartInitException;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.AnalysisLoader;
import org.omnetpp.scave.AnalysisSaver;
import org.omnetpp.scave.LegacyAnalysisLoader;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.editors.ui.BrowseDataPage;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.ChartsPage;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.editors.ui.InputsPage;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.AddInputFileCommand;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.CommandStackListener;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.SetChartContentsCommand;
import org.omnetpp.scave.model2.ResultItemRef;
import org.omnetpp.scave.pychart.PythonProcessPool;
import org.omnetpp.scave.python.MatplotlibChartViewer;
import org.omnetpp.scave.python.NativeChartViewer;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * OMNeT++ Analysis tool.
 *
 * @author andras, tomi
 */
public class ScaveEditor extends MultiPageEditorPartExt
        implements ISelectionProvider, IGotoMarker, INavigationLocationProvider {
    public static final String ACTIVE_PAGE = "ActivePage", PAGE = "Page", PAGE_ID = "PageId";

    private InputsPage inputsPage;
    private BrowseDataPage browseDataPage;
    private ChartsPage chartsPage;

    private Map<AnalysisItem, Control> closablePages = new LinkedHashMap<AnalysisItem, Control>();

    PythonProcessPool processPool = new PythonProcessPool(2);

    ChartTemplateRegistry chartTemplateRegistry = new ChartTemplateRegistry();

    /**
     * This is the content outline page.
     */
    protected ScaveEditorContentOutlinePage contentOutlinePage;

    /**
     * This is a kludge...
     */
    protected IStatusLineManager contentOutlineStatusLineManager;

    /**
     * This is the content outline page's viewer.
     */
    protected TreeViewer contentOutlineViewer;

    /**
     * This is the property sheet page.
     */
    protected List<PropertySheetPage> propertySheetPages = new ArrayList<PropertySheetPage>();

    /**
     * The selection change listener added to all viewers
     */
    protected ISelectionChangedListener selectionChangedListener = new ISelectionChangedListener() {
        public void selectionChanged(SelectionChangedEvent selectionChangedEvent) {
            handleSelectionChange(selectionChangedEvent.getSelection());
        }
    };

    protected boolean selectionChangeInProgress = false; // to prevent recursive notifications

    /**
     * This keeps track of all the
     * {@link org.eclipse.jface.viewers.ISelectionChangedListener}s that are
     * listening to this editor. We need this because we implement
     * ISelectionProvider which includes having to manage a listener list.
     */
    protected Collection<ISelectionChangedListener> selectionChangedListeners = new ArrayList<ISelectionChangedListener>();

    /**
     * This keeps track of the selection of the editor as a whole.
     */
    protected ISelection editorSelection = StructuredSelection.EMPTY;

    Analysis analysis;

    ScaveEditorActions actions;

    /**
     * This listens for when the outline becomes active
     */
    protected IPartListener partListener = new IPartListener() {
        public void partActivated(IWorkbenchPart p) {
            if (p == ScaveEditor.this) {
                ChartScriptEditor activeChartScriptEditor = getActiveChartScriptEditor();
                if (activeChartScriptEditor != null)
                    activeChartScriptEditor.pageActivated();
            }
            if (p instanceof ContentOutline) {
                if (((ContentOutline) p).getCurrentPage() == contentOutlinePage) {
                    //getActionBarContributor().setActiveEditor(ScaveEditor.this);
                }
            } else if (p instanceof PropertySheet) {
                if (propertySheetPages.contains(((PropertySheet) p).getCurrentPage())) {
                    //getActionBarContributor().setActiveEditor(ScaveEditor.this);
                }
            }
        }

        public void partBroughtToTop(IWorkbenchPart p) {
        }

        public void partClosed(IWorkbenchPart p) {
            if (p == ScaveEditor.this) {
                getSite().getPage().removePartListener(this);
                saveState();
            }
        }

        public void partDeactivated(IWorkbenchPart p) {
        }

        public void partOpened(IWorkbenchPart p) {
        }
    };

    // TODO: do this locally in each page
    CommandStackListener commandStackListener = new CommandStackListener() {
        @Override
        public void commandStackChanged(CommandStack commandStack) {
            getContainer().getDisplay().asyncExec(new Runnable() {
                @Override
                public void run() {
                    firePropertyChange(IEditorPart.PROP_DIRTY);

                    actions.updateActions();

                    // Try to select the affected objects.
                    //
                    ICommand mostRecentCommand = commandStack.getMostRecentCommand();
                    if (mostRecentCommand != null) {
                        Collection<ModelObject> affectedObjects = mostRecentCommand.getAffectedObjects();

                        Set<ModelObject> selection = new HashSet<ModelObject>();

                        for (ModelObject obj : affectedObjects) {
                            if (obj instanceof Property)
                                selection.add(obj.getParent());
                            else if (obj instanceof Charts || obj instanceof Inputs)
                                ; // skip
                            else
                                selection.add(obj);
                        }

                        setSelectionToViewer(selection);
                    }
                    for (Iterator<PropertySheetPage> i = propertySheetPages.iterator(); i.hasNext();) {
                        PropertySheetPage propertySheetPage = i.next();
                        if (propertySheetPage.getControl().isDisposed()) {
                            i.remove();
                        } else {
                            propertySheetPage.refresh();
                        }
                    }
                }
            });
        }
    };

    /**
     * ResultFileManager containing all files of the analysis.
     */
    private ResultFileManagerEx manager = new ResultFileManagerEx();

    /**
     * Loads/unloads result files in manager, according to changes in the model and
     * in the workspace.
     */
    private ResultFilesTracker tracker;

    /**
     * Updates pages when the model changed.
     */
    private IModelChangeListener modelChangeListener = new IModelChangeListener() {

        @Override
        public void modelChanged(ModelChangeEvent event) {
            firePropertyChange(ScaveEditor.PROP_DIRTY);

            updatePages(event);
            // strictly speaking, some actions (undo, redo) should be updated when the
            // command stack changes, but there are more of those.
            actions.updateActions();

            if (contentOutlinePage != null)
                contentOutlinePage.refresh();
        }
    };

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

    public ChartsPage getChartsPage() {
        return chartsPage;
    }

    @Override
    public void init(IEditorSite site, IEditorInput editorInput) throws PartInitException {

        // if (true)
        // throw new PartInitException("NOT IMPL");

        if (!(editorInput instanceof IFileEditorInput))
            throw new DetailedPartInitException(
                    "Invalid input, it must be a file in the workspace: " + editorInput.getName(),
                    "Please make sure the project is open before trying to open a file in it.");
        IFile fileInput = ((IFileEditorInput) editorInput).getFile();
        if (!editorInput.exists())
            throw new PartInitException("File '" + fileInput.getFullPath().toString() + "' does not exist");

        // init super. Note that this does not load the model yet -- it's done in
        // createModel() called from createPages().
        setSite(site);
        setInputWithNotify(editorInput);
        setPartName(editorInput.getName());
        site.setSelectionProvider(this);
        site.getPage().addPartListener(partListener);

        actions = new ScaveEditorActions(this);
        chartTemplateRegistry.setProject(fileInput.getProject());
    }

    @Override
    public void dispose() {

        processPool.dispose();

        if (tracker != null) {
            ResourcesPlugin.getWorkspace().removeResourceChangeListener(tracker);
            analysis.removeListener(tracker);
        }

        if (manager != null) {
            // deactivate the tracker explicitly, because it might receive a notification
            // in case of the ScaveEditor.dispose() was called from a notification.
            boolean trackerInactive = true;
            if (tracker != null) {
                trackerInactive = tracker.deactivate();
                tracker = null;
            }
            // it would get garbage-collected anyway, but the sooner the better because it
            // may have allocated large amounts of data
            if (trackerInactive)
                manager.dispose();
            manager = null;
        }

        if (getSite() != null && getSite().getPage() != null)
            getSite().getPage().removePartListener(partListener);

        for (PropertySheetPage propertySheetPage : propertySheetPages)
            propertySheetPage.dispose();

        if (contentOutlinePage != null)
            contentOutlinePage.dispose();

        super.dispose();

    }

    // Modified DropAdapter to convert drop events.
    // The original EditingDomainViewerDropAdapter tries to add
    // files to the ResourceSet as XMI documents (what probably
    // causes a parse error). Here we convert the URIs of the
    // drop source to InputFiles and modify the drop target.
    // class DropAdapter
    // {
    // List<InputFile> inputFilesInSource = null;
    //
    // public DropAdapter(EditingDomain domain, Viewer viewer) {
    // super(domain, viewer);
    // }
    //
    // @Override
    // protected Collection<?> extractDragSource(Object object) {
    // Collection<?> collection = super.extractDragSource(object);
    //
    // // find URIs in source and convert them InputFiles
    // inputFilesInSource = null;
    // for (Object element : collection) {
    // if (element instanceof URI) {
    // String workspacePath = getWorkspacePathFromURI((URI)element);
    // if (workspacePath != null) {
    // if (inputFilesInSource == null)
    // inputFilesInSource = new ArrayList<InputFile>();
    // if (workspacePath.endsWith(".sca") || workspacePath.endsWith(".vec")) {
    // InputFile file = new InputFile(workspacePath);
    // inputFilesInSource.add(file);
    // }
    // }
    // }
    // }
    //
    // return inputFilesInSource != null ? inputFilesInSource : collection;
    // }
    //
    // @Override
    // protected Object extractDropTarget(Widget item) {
    // Object target = super.extractDropTarget(item);
    // if (inputFilesInSource != null) {
    // if (target instanceof InputFile || target == null)
    // target = getAnalysis().getInputs();
    // }
    // return target;
    // }
    // }

    // private String getWorkspacePathFromURI(URI uri) {
    // if (uri.isFile()) {
    // IPath path = new Path(uri.toFileString());
    // IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
    // IFile file = root.getFileForLocation(path);
    // return file != null ? file.getFullPath().toString() : null;
    // }
    // else if (uri.isPlatformResource())
    // return uri.toPlatformString(true);
    // else
    // return null;
    // }

    protected void setupDragAndDropSupportFor(StructuredViewer viewer) {
        int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
        // XXX FileTransfer causes an exception
        Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance(),
                LocalSelectionTransfer.getTransfer()/* , FileTransfer.getInstance() */ };
        viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));

        // TODO
        // viewer.addDropSupport(dndOperations, transfers, new
        // DropAdapter(editingDomain, viewer));
    }

    public void createModel() {
        // Assumes that the input is a file object.
        IFileEditorInput modelFile = (IFileEditorInput) getEditorInput();

        try {
            DocumentBuilder db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            modelFile.getFile().refreshLocal(1, null);
            Document d = db.parse(modelFile.getFile().getContents());

            NodeList nl = d.getChildNodes();

            Node rootNode = nl.item(0);

            if ("scave:Analysis".equals(rootNode.getNodeName())) {
                MessageBox messageBox = new MessageBox(getSite().getShell(), SWT.OK | SWT.CANCEL | SWT.ICON_QUESTION);
                messageBox.setText("Convert Analysis File?");
                messageBox.setMessage("File " + modelFile.getFile().getFullPath() + " is in an older file format.\n"
                        + "Do you want to open and convert it?\n");
                int result = messageBox.open();
                switch (result) {
                case SWT.CANCEL:
                    analysis = null;
                    break;
                case SWT.OK:
                    ArrayList<String> errors = new ArrayList<>();
                    analysis = new LegacyAnalysisLoader(getChartTemplateRegistry(), errors).loadLegacyAnalysis(rootNode);

                    if (!errors.isEmpty()) {
                        ListDialog errorDialog = new ListDialog(Display.getCurrent().getActiveShell());
                        errorDialog.setInput(errors);
                        errorDialog.setContentProvider(new ArrayContentProvider());
                        errorDialog.setLabelProvider(new LabelProvider());
                        errorDialog.setMessage("Conversion errors:");
                        errorDialog.open();
                    }

                    break;
                }
            }
            else {
                analysis = AnalysisLoader.loadNewAnalysis(rootNode, getChartTemplateRegistry());
            }
        }
        catch (SAXException | IOException | CoreException | ParserConfigurationException | RuntimeException e) {
            MessageDialog.openError(getSite().getShell(), "Error",
                    "Could not open resource " + modelFile.getFile().getFullPath() + "\n\n" + e.getMessage());
            ScavePlugin.logError("could not load resource", e);
            analysis = null;
        }

        if (analysis == null)
            return;

        IFile inputFile = ((IFileEditorInput) getEditorInput()).getFile();
        tracker = new ResultFilesTracker(manager, analysis.getInputs(), inputFile.getParent().getFullPath());
        analysis.addListener(modelChangeListener);
        analysis.addListener(tracker);

        // listen to resource changes: create, delete, modify
        ResourcesPlugin.getWorkspace().addResourceChangeListener(tracker);
    }

    protected void doCreatePages() {
        // add fixed pages: Inputs, Browse Data, Datasets
        FillLayout layout = new FillLayout();
        getContainer().setLayout(layout);

        getTabFolder().setMRUVisible(true);

        createInputsPage();
        createBrowseDataPage();
        createChartsPage();

        // We can load the result files now.
        // The chart pages are not refreshed automatically when the result files change,
        // so we have to load the files synchronously
        // Note that tracker.updaterJob.join() can not be used here, because the
        // JobManager is suspended during initalization of the UI.
        tracker.synchronize(true);

        // now we can restore chart pages (and other closable pages)
        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
            @Override
            public Object call() throws Exception {
                restoreState();
                return null;
            }
        });

        final CTabFolder tabfolder = getTabFolder();
        tabfolder.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                int newPageIndex = tabfolder.indexOf((CTabItem) e.item);
                pageChangedByUser(newPageIndex);
            }
        });

        tabfolder.addCTabFolder2Listener(new CTabFolder2Adapter() {
            @Override
            public void close(CTabFolderEvent event) {
                saveState();
                ChartScriptEditor editor = (ChartScriptEditor)event.item.getData();
                boolean allowClose = askToKeepEditedTemporaryChart(editor);
                event.doit = allowClose;
                if (allowClose)
                    applyChartEdits(editor);
            }
        });
    }

    public IPropertySheetPage getPropertySheetPage() {

        PropertySheetPage propertySheetPage = new PropertySheetPage() {
//            @Override
//            public void setSelectionToViewer(List<?> selection) {
//                ScaveEditor.this.setSelectionToViewer(selection);
//                ScaveEditor.this.setFocus();
//            }

            // this is a constructor fragment --Andras
            {
                // turn off sorting for our INonsortablePropertyDescriptors
                setSorter(new PropertySheetSorter() {
                    public int compare(IPropertySheetEntry entryA, IPropertySheetEntry entryB) {
                        IPropertyDescriptor descriptorA = (IPropertyDescriptor) ReflectionUtils.getFieldValue(entryA,
                                "descriptor"); // it's f***ing private --Andras
                        if (descriptorA instanceof INonsortablePropertyDescriptor)
                            return 0;
                        else
                            return super.compare(entryA, entryB);
                    }
                });
            }

            @Override
            public void setActionBars(IActionBars actionBars) {
                super.setActionBars(actionBars);
                //getActionBarContributor().shareGlobalActions(this, actionBars);
            }

            @Override
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (selection instanceof IDListSelection) {
                    // re-package element into ResultItemRef, otherwise property sheet
                    // only gets a Long due to sel.toArray() in base class. We only want
                    // to show properties if there's only one item in the selection.
                    IDListSelection idListSelection = (IDListSelection) selection;
                    if (idListSelection.size() == 1) {
                        long id = idListSelection.getIDList().get(0);
                        ResultItemRef resultItemRef = new ResultItemRef(id, idListSelection.getResultFileManager());
                        selection = new StructuredSelection(resultItemRef);
                    }
                }
                else if (selection instanceof StructuredSelection) {
                    StructuredSelection structured = (StructuredSelection)selection;
                    structured.toList();
                }
                super.selectionChanged(part, selection);
            }
        };

        propertySheetPages.add(propertySheetPage);

        if (propertySheetPage instanceof PropertySheetPage) {
            propertySheetPage.setPropertySourceProvider(new ScavePropertySourceProvider(manager));
        }
        return propertySheetPage;

    }

    /**
     * Adds a fixed (non-closable) editor page at the last position
     */
    public int addFixedPage(FormEditorPage page) {
        int index = addPage(page);
        setPageText(index, page.getPageTitle());
        return index;
    }

    /**
     * Adds a closable editor page at the last position
     */
    public int addClosablePage(Control page, String pageTitle) {
        int index = getPageCount();
        addClosablePage(index, page);
        setPageText(index, pageTitle);
        return index;
    }

    public int addClosablePage(FormEditorPage page) {
        return addClosablePage(page, page.getPageTitle());
    }

    public int addClosablePage(IEditorPart editor, IEditorInput input, String pageTitle) throws PartInitException {
        int index = getPageCount();
        addClosablePage(index, editor, input);
        setPageText(index, pageTitle);
        return index;
    }

    public FormEditorPage getActiveEditorPage() {
        int i = getActivePage();
        if (i >= 0)
            return getEditorPage(i);
        else
            return null;
    }

    public ChartScriptEditor getActiveChartScriptEditor() {
        int i = getActivePage();
        if (i >= 0) {
            IEditorPart editor = getEditor(i);
            if (editor instanceof ChartScriptEditor)
                return (ChartScriptEditor) editor;
        }

        return null;
    }

    public FormEditorPage getEditorPage(int pageIndex) {
        Control control = getControl(pageIndex);
        if (control instanceof FormEditorPage)
            return (FormEditorPage)control;
        return null;
    }

    public ChartViewer getActiveChartViewer() {
        FormEditorPage activePage = getActiveEditorPage();
        if (activePage != null)
            return activePage.getActiveChartViewer();
        ChartScriptEditor activeEditor = getActiveChartScriptEditor();

        if (activeEditor != null) {
            NativeChartViewer nativeChartViewer = activeEditor.getNativeChartViewer();
            return (nativeChartViewer != null) ? nativeChartViewer.getChartViewer() : null;
        }

        return null;
    }

    public MatplotlibChartViewer getActiveMatplotlibChartViewer() {
        ChartScriptEditor activeEditor = getActiveChartScriptEditor();

        if (activeEditor != null)
            return activeEditor.getMatplotlibChartViewer();

        return null;
    }

    public void applyChartEdits(ChartScriptEditor editor) {

        Chart orig = editor.getOriginalChart();
        Chart chart = editor.getChart();

        chart.setScript(editor.getDocument().get());

        SetChartContentsCommand command = new SetChartContentsCommand(chart, orig, (Chart)chart.dup());
        if (!command.isEmpty())
            getChartsPage().getCommandStack().addExecuted(command);
    }

    public boolean askToKeepEditedTemporaryChart(ChartScriptEditor editor) {
        Chart chart = editor.getChart();

        if (!editor.getCommandStack().wasObjectAffected(chart) && !editor.isDirty())
            return true;

        if (chart.isTemporary()) {
            int result = MessageDialog.open(MessageDialog.QUESTION_WITH_CANCEL, Display.getCurrent().getActiveShell(),
                    "Keep Temporary Chart?",
                    "Keep chart '" + chart.getName() + "' as part of the analysis? If you choose 'No', your edits will be lost.",
                    SWT.NONE, "Yes", "No", "Cancel");

            switch (result) {
            case 0:
                chart.setTemporary(false);
                ICommand command = new AddChartCommand(getAnalysis(), chart);
                chartsPage.getCommandStack().execute(command);
                showAnalysisItem(chart);
                return true;
            case 1:
                // no-op
                return true;
            case 2:
                return false;
            }
        }

        return true;
    }

    public Analysis getAnalysis() {
        return analysis;
    }

    public File getAnfFileDirectory() {
        IEditorInput input = getEditorInput();

        IFile file = ((FileEditorInput) input).getFile();

        return file.getLocation().removeLastSegments(1).toFile();
    }

    /**
     * Opens the given <code>item</code> (chart), or switches
     * to it if already opened.
     */
    public FormEditorPage openPage(AnalysisItem item) {
        int pageIndex = getOrCreateClosablePage(item);
        setActivePage(pageIndex);
        return getEditorPage(pageIndex);
    }

    /**
     * Closes the page displaying the given <code>object</code>. If no such page,
     * nothing happens.
     */
    public void closePage(AnalysisItem object) {
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

    public void showChartsPage() {
        showPage(getChartsPage());
    }

    public void showPage(FormEditorPage page) {
        int pageIndex = findPage(page);
        if (pageIndex >= 0)
            setActivePage(pageIndex);
    }

    public void showAnalysisItem(AnalysisItem item) {
        showChartsPage();
        getChartsPage().getViewer().setSelection(new StructuredSelection(item));
    }

    public void gotoObject(Object object) { // XXX useful?

        if (getAnalysis() == null)
            return;

        if (object instanceof AnalysisItem) {
            FormEditorPage editorPage = getEditorPage((AnalysisItem)object);
            if (editorPage != null) {
                showPage(editorPage);
                return;
            }
        }

        FormEditorPage activePage = getActiveEditorPage();
        if (activePage != null) {
            if (activePage.gotoObject(object))
                return;
        }

        int activePageIndex = -1;
        for (int pageIndex = getPageCount() - 1; pageIndex >= 0; --pageIndex) {
            FormEditorPage page = getEditorPage(pageIndex);
            if (page != null && page.gotoObject(object)) {
                activePageIndex = pageIndex;
                break;
            }
        }
        if (activePageIndex >= 0) {
            setActivePage(activePageIndex);
        }
    }

    public void setPageTitle(FormEditorPage page, String title) {
        int pageIndex = findPage(page);
        if (pageIndex >= 0)
            setPageText(pageIndex, title);
    }

    private void createInputsPage() {
        inputsPage = new InputsPage(getContainer(), this);
        addFixedPage(inputsPage);
    }

    private void createBrowseDataPage() {
        browseDataPage = new BrowseDataPage(getContainer(), this);
        addFixedPage(browseDataPage);
    }

    private void createChartsPage() {
        chartsPage = new ChartsPage(getContainer(), this);
        addFixedPage(chartsPage);
    }

    /**
     * Creates a closable page. These pages are closed automatically when the
     * displayed object (chart) is removed from the model. Their tabs contain a
     * small (x), so the user can also close them.
     */
    private int createClosablePage(AnalysisItem item) {
        if (item instanceof Chart)
            try {
                int index = openChartScriptEditor((Chart) item);
                closablePages.put(item, getControl(index));
                return index;
            } catch (PartInitException e) {
                ScavePlugin.logError(e);
                return -1;
            }
        else
            throw new IllegalArgumentException("Cannot create editor page for " + item);
    }

    @Override
    protected void pageClosed(Control control) {
        // Assert.isTrue(closablePages.containsValue(control));

        // remove it from the map
        Iterator<Map.Entry<AnalysisItem, Control>> entries = closablePages.entrySet().iterator();
        while (entries.hasNext()) {
            Map.Entry<AnalysisItem, Control> entry = entries.next();
            if (control.equals(entry.getValue()))
                entries.remove();
        }
    }

    /**
     * Returns the page displaying {@code item}.
     */
    public FormEditorPage getEditorPage(AnalysisItem item) {
        Control control = closablePages.get(item);
        if (control instanceof FormEditorPage)
            return (FormEditorPage)control;
        return null;
    }

    /**
     * Returns the page displaying <code>object</code>. If the object already has a
     * page it is returned, otherwise a new page created.
     */
    public int getOrCreateClosablePage(AnalysisItem item) {
        Control page = closablePages.get(item);
        int pageIndex = page != null ? findPage(page) : createClosablePage(item);
        Assert.isTrue(pageIndex >= 0);
        return pageIndex;
    }

    public void handleSelectionChange(ISelection selection) {
        // FIXME merge this method into fireSelectionChangedEvent()!!! that's where the
        // guard should be as well!
        boolean selectionReallyChanged = selection != editorSelection && !selection.equals(editorSelection);
        if (!selectionChangeInProgress || selectionReallyChanged) {
            try {
                selectionChangeInProgress = true; // "finally" ensures this gets reset in any case
                editorSelection = selection;
                // FIXME notifying the view about IDListSelections would remove the selection
                // from the editor!
                if (!(selection instanceof IDListSelection)) {
                    // setViewerSelectionNoNotify(contentOutlineViewer, selection);
                    updateStatusLineManager(contentOutlineStatusLineManager, selection);
                }
                updateStatusLineManager(getEditorSite().getActionBars().getStatusLineManager(), selection);
                actions.updateActions();
                Debug.println("selection changed: " + selection);
                fireSelectionChangedEvent(selection);
            } finally {
                selectionChangeInProgress = false;
            }
        }
    }

    class ScaveEditorContentOutlinePage extends ContentOutlinePage {
        public void createControl(Composite parent) {
            super.createControl(parent);
            TreeViewer viewer = getTreeViewer();
            Tree tree = viewer.getTree();
            if (tree != null) {
                tree.addSelectionListener(new SelectionAdapter() {
                    @Override
                    public void widgetDefaultSelected(SelectionEvent e) {
                        if (e.item instanceof TreeItem) {
                            Object object = ((TreeItem) e.item).getData();
                            if (object instanceof AnalysisItem)
                                openPage((AnalysisItem)object);
                        }
                    }
                });
            }

            contentOutlineViewer = getTreeViewer();
            contentOutlineViewer.addSelectionChangedListener(this);

            // Set up the tree viewer.

            contentOutlineViewer.setContentProvider(new ITreeContentProvider() {

                @Override
                public boolean hasChildren(Object element) {
                    return getChildren(element).length > 0;
                }

                @Override
                public Object getParent(Object element) {
                    if (element instanceof ModelObject)
                        return ((ModelObject) element).getParent();
                    return null;
                }

                @Override
                public Object[] getElements(Object inputElement) {
                    return getChildren(inputElement);
                }

                @Override
                public Object[] getChildren(Object element) {
                    if (element instanceof Analysis)
                        return new Object[] { ((Analysis)element).getInputs(), ((Analysis)element).getCharts() };
                    else if (element instanceof Inputs)
                        return ((Inputs) element).getInputs().toArray();
                    else if (element instanceof Charts)
                        return ((Charts) element).getCharts().toArray();
                    else
                        return new Object[0];
                }
            });
            contentOutlineViewer.setLabelProvider(new LabelProvider() {
                @Override
                public String getText(Object element) {
                    if (element instanceof Analysis)
                        return "Analysis";
                    else if (element instanceof Inputs)
                        return "Inputs";
                    else if (element instanceof Charts)
                        return "Charts";
                    else if (element instanceof InputFile)
                        return ((InputFile) element).getName();
                    else if (element instanceof AnalysisItem)
                        return ((AnalysisItem) element).getName();
                    else
                        return element.toString();
                }

                @Override
                public Image getImage(Object element) {
                    if (element instanceof Analysis)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_FOLDER);
                    else if (element instanceof Inputs)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_FOLDER);
                    else if (element instanceof Charts)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_FOLDER);
                    else if (element instanceof InputFile)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_INPUTFILE);
                    else if (element instanceof Chart)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_CHART);
                    else if (element instanceof AnalysisItem)
                        return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_FOLDER);
                    else
                        return null;
                }
            });
            contentOutlineViewer.setInput(getAnalysis());
            contentOutlineViewer.expandToLevel(3);

            // Make sure our popups work.
            UIUtils.createContextMenuFor(contentOutlineViewer.getControl(), true, new IMenuListener() {
                @Override
                public void menuAboutToShow(IMenuManager menuManager) {
                    actions.populateContextMenu(menuManager);
                }
            });
        }

        public void makeContributions(IMenuManager menuManager, IToolBarManager toolBarManager,
                IStatusLineManager statusLineManager) {
            super.makeContributions(menuManager, toolBarManager, statusLineManager);
            contentOutlineStatusLineManager = statusLineManager;
        }

        public void setActionBars(IActionBars actionBars) {
            super.setActionBars(actionBars);
            actions.shareGlobalActions(this, actionBars);
        }

        public void refresh() {
            getTreeViewer().refresh();
        }
    }

    public IContentOutlinePage getContentOutlinePage() {
        if (contentOutlinePage == null) {
            contentOutlinePage = new ScaveEditorContentOutlinePage();
            contentOutlinePage.addSelectionChangedListener(selectionChangedListener);
            contentOutlinePage.addSelectionChangedListener(new ISelectionChangedListener() {
                @Override
                public void selectionChanged(SelectionChangedEvent event) {
                    contentOutlineSelectionChanged(event.getSelection());
                }
            });
        }

        return contentOutlinePage;
    }

    protected void contentOutlineSelectionChanged(ISelection selection) {
        if (selection instanceof IStructuredSelection) {
            Object object = ((IStructuredSelection) selection).getFirstElement();
            // Debug.println("Selected: "+object);
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
        List<InputFile> inputs = getAnalysis().getInputs().getInputs();
        boolean found = false;
        for (Object inputFileObj : inputs) {
            InputFile inputFile = (InputFile) inputFileObj;
            if (inputFile.getName().equals(resourcePath))
                found = true;
        }

        if (!found) {
            InputFile inputFile = new InputFile(resourcePath);
            inputsPage.getCommandStack().execute(new AddInputFileCommand(analysis, inputFile));
        }
    }

    /**
     * Utility function: finds an IFile for an existing file given with OS path.
     * Returns null if the file was not found.
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
    public static ScaveEditor getActiveScaveEditor() {
        IWorkbench workbench = PlatformUI.getWorkbench();
        if (workbench.getActiveWorkbenchWindow() != null) {
            IWorkbenchPage page = workbench.getActiveWorkbenchWindow().getActivePage();
            if (page != null) {
                IEditorPart part = page.getActiveEditor();
                if (part instanceof ScaveEditor)
                    return (ScaveEditor) part;
            }
        }
        return null;
    }

    public static CommandStack getActiveScaveCommandStack() {
        ScaveEditor activeEditor = getActiveScaveEditor();
        return activeEditor == null ? null : activeEditor.getActiveCommandStack();
    }

    public ISelectionChangedListener getSelectionChangedListener() {
        return selectionChangedListener;
    }

    //TODO temp chart name changes currently do not propagate to the tabitem text, as we do not receive model change notification about their change!
    private void updatePages(ModelChangeEvent event) {

        // close pages whose content was deleted, except temporary charts
        for (AnalysisItem item : closablePages.keySet().toArray(new AnalysisItem[0])) { // toArray() is for preventing ConcurrentModificationException
            // TODO: when folders are introduced, revise this
            if (!analysis.getCharts().getCharts().contains(item) && !(item instanceof Chart && ((Chart)item).isTemporary()))
                removePage(closablePages.get(item));
        }

        // update contents of pages
        int pageCount = getPageCount();
        for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
            FormEditorPage editorPage = getEditorPage(pageIndex);
            editorPage.updatePage(event);
        }
    }

    @Override
    protected void pageChange(int newPageIndex) {
        super.pageChange(newPageIndex);

        IEditorPart editor = getEditor(newPageIndex);
        if (editor != null) {
            if (editor instanceof ChartScriptEditor)
                ((ChartScriptEditor) editor).pageActivated();
        }
        else {
            Control page = getControl(newPageIndex);

            if (page instanceof FormEditorPage)
                ((FormEditorPage) page).pageActivated();
        }

        actions.updateActions();
    }

    String getPageId(FormEditorPage page) {
        if (page == null)
            return null;
        else if (page.equals(inputsPage))
            return "Inputs";
        else if (page.equals(browseDataPage))
            return "BrowseData";
        else if (page.equals(chartsPage))
            return "Charts";
        else {
            for (Map.Entry<AnalysisItem, Control> entry : closablePages.entrySet()) {
                AnalysisItem item = entry.getKey();
                Control editorPage = entry.getValue();
                if (page.equals(editorPage))
                    return Integer.toString(item.getId());
            }
        }

        return null;
    }

    private String getPageId(ChartScriptEditor chartScriptEditor) {
        return Integer.toString(analysis.getCharts().getCharts().indexOf(chartScriptEditor.chart));
    }

    // TODO merge the one below into this one?
    FormEditorPage restoreFixedPage(String pageId) {
        if (pageId == null)
            return null;
        if (pageId.equals("Inputs")) {
            setActivePage(findPage(inputsPage));
            return inputsPage;
        } else if (pageId.equals("BrowseData")) {
            setActivePage(findPage(browseDataPage));
            return browseDataPage;
        } else if (pageId.equals("Charts")) {
            setActivePage(findPage(chartsPage));
            return chartsPage;
        }

        return null;
    }

    // TODO merge with the one above, as a last else clause?
    ChartScriptEditor restoreEditor(String pageId) {

        if (pageId == null || pageId.equals("Inputs") || pageId.equals("BrowseData") || pageId.equals("Charts"))
            return null;

        int id = Integer.parseInt(pageId);
        List<AnalysisItem> charts = analysis.getCharts().getCharts();

        AnalysisItem item = null;
        for (AnalysisItem chart : charts)
            if (chart.getId() == id) {
                item = chart;
                break;
            }

        if (item != null) {
            createClosablePage(item);
            Control p = closablePages.get(item);
            int pageIndex = findPage(p);
            IEditorPart editor = getEditor(pageIndex);
            if (editor instanceof ChartScriptEditor) {
                return (ChartScriptEditor) editor;
            }
        }
        return null;
    }

    /*
     * Per input persistent state.
     */
    public IFile getInputFile() {
        IEditorInput input = getEditorInput();
        if (input instanceof IFileEditorInput)
            return ((IFileEditorInput) input).getFile();
        else
            return null;
    }

    IMemento getMementoFor(ChartScriptEditor editor) {
        IFile file = getInputFile();
        if (file != null) {
            try {
                ScaveEditorMemento memento = new ScaveEditorMemento(file);
                String editorPageId = getPageId(editor);
                for (IMemento pageMemento : memento.getChildren(PAGE)) {
                    String pageId = pageMemento.getString(PAGE_ID);
                    if (pageId != null && pageId.equals(editorPageId))
                        return pageMemento;
                }
            } catch (CoreException e) {
            }
        }
        return null;
    }

    private void saveState(IMemento memento) {
        memento.putInteger(ACTIVE_PAGE, getActivePage());
        for (AnalysisItem openedObject : closablePages.keySet()) {
            Control p = closablePages.get(openedObject);
            if (p instanceof FormEditorPage) {
                FormEditorPage page = (FormEditorPage) p;
                IMemento pageMemento = memento.createChild(PAGE);
                String pageId = getPageId(page);
                pageMemento.putString(PAGE_ID, pageId);
                page.saveState(pageMemento);
                if (page instanceof ChartPage) {
                    ChartPage chartPage = (ChartPage)page;
                    chartPage.getChartScriptEditor().saveState(pageMemento);
                }
            }
        }
    }

    private void restoreState(IMemento memento) {
        for (IMemento pageMemento : memento.getChildren(PAGE)) {
            String pageId = pageMemento.getString(PAGE_ID);
            if (pageId != null) {
                FormEditorPage page = restoreFixedPage(pageId);
                if (page != null)
                    page.restoreState(pageMemento);

                ChartScriptEditor editor = restoreEditor(pageId);
                if (editor != null)
                    editor.restoreState(pageMemento);
            }
        }
        int activePage = memento.getInteger(ACTIVE_PAGE);
        if (activePage >= 0 && activePage < getPageCount())
            setActivePage(activePage);
    }

    public void saveState() {
        try {
            IFile file = getInputFile();
            if (file != null) {
                ScaveEditorMemento memento = new ScaveEditorMemento();
                saveState(memento);
                memento.save(file);
            }
        } catch (Exception e) {
            // MessageDialog.openError(getSite().getShell(),
            // "Saving editor state",
            // "Error occured while saving editor state: "+e.getMessage());
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
        } catch (CoreException e) {
            ScavePlugin.log(e.getStatus());
        } catch (Exception e) {
            // MessageDialog.openError(getSite().getShell(),
            // "Restoring editor state",
            // "Error occured while restoring editor state: "+e.getMessage());
            ScavePlugin.logError(e);
        }
    }

    /*
     * Navigation
     */
    @Override
    public INavigationLocation createEmptyNavigationLocation() {
        return new ScaveNavigationLocation(this, true);
    }

    @Override
    public INavigationLocation createNavigationLocation() {
        return new ScaveNavigationLocation(this, false);
    }

    public void markNavigationLocation() {
        getSite().getPage().getNavigationHistory().markLocation(this);
    }

    public void pageChangedByUser(int newPageIndex) {
        Control page = getControl(newPageIndex);
        if (page instanceof FormEditorPage) {
            markNavigationLocation();
        }
    }

    public final Chart findOpenChartById(int chartId) {
        int count = getPageCount();
        for (int i = 0; i < count; i++) {
            IEditorPart editor = getEditor(i);
            if (editor != null && editor.getEditorInput() instanceof ChartScriptEditorInput) {
                Chart chart = ((ChartScriptEditorInput)editor.getEditorInput()).chart;
                if (chart.getId() == chartId)
                    return chart;
            }
        }
        return null;
    }

    /*
     * IGotoMarker
     */
    @Override
    public void gotoMarker(IMarker marker) {
        try {
            String sourceId = marker.getAttribute(IMarker.SOURCE_ID).toString();
            boolean sourceIdIsInteger = sourceId != null && sourceId.matches("[\\d]+");

            if (marker.getType().equals(IMarker.PROBLEM) && sourceIdIsInteger) {
                int chartId = Integer.parseInt(sourceId);

                Chart chart = null;
                for (AnalysisItem i : analysis.getCharts().getCharts()) {
                    if (!(i instanceof Chart))
                        continue;
                    Chart c = (Chart)i;
                    if (c.getId() == chartId) {
                        chart = c;
                        break;
                    }
                }

                if (chart == null)
                    chart = findOpenChartById(chartId);

                if (chart == null)
                    return;

                IEditorPart[] parts = findEditors(new ChartScriptEditorInput(chart));

                for (IEditorPart part : parts) {
                    ChartScriptEditor scriptEditor = (ChartScriptEditor) part;
                    setActiveEditor(scriptEditor);
                    scriptEditor.gotoMarker(marker);
                    break;
                }
            } else {
                // TODO
                // List<?> targetObjects = markerHelper.getTargetObjects(editingDomain, marker);
                // if (!targetObjects.isEmpty())
                // setSelectionToViewer(targetObjects);
            }
        } catch (CoreException exception) {
            ScavePlugin.logError(exception);
        }
    }

    /**
     * This sets the selection into whichever viewer is active.
     */
    public void setSelectionToViewer(Collection<?> collection) {
        handleSelectionChange(new StructuredSelection(collection.toArray()));
    }

    /**
     * Notify listeners on {@link org.eclipse.jface.viewers.ISelectionProvider}
     * about a selection change.
     */
    protected void fireSelectionChangedEvent(ISelection selection) {
        for (ISelectionChangedListener listener : selectionChangedListeners)
            listener.selectionChanged(new SelectionChangedEvent(this, selection));
    }

    // /**
    // * This creates a context menu for the viewer and adds a listener as well
    // registering the menu for extension.
    // */
    // protected void createContextMenuFor(StructuredViewer viewer) {
    // createContextMenuFor(viewer.getControl());
    // }
    //
    // protected void createContextMenuFor(Control control) {
    // MenuManager contextMenu = new MenuManager("#PopUp");
    // contextMenu.add(new Separator("additions"));
    // contextMenu.setRemoveAllWhenShown(true);
    // contextMenu.addMenuListener(this);
    // Menu menu = contextMenu.createContextMenu(control);
    // control.setMenu(menu);
    //
    // // Note: don't register the context menu, otherwise "Run As", "Debug As",
    // "Team", and other irrelevant menu items appear...
    // //getSite().registerContextMenu(contextMenu, viewer);
    // }

    /**
     * This is the method used by the framework to install your own controls.
     */
    public void createPages() {
        createModel();
        if (analysis != null) {
            doCreatePages();
        } else {
            addFixedPage(new FormEditorPage(getContainer(), SWT.NONE, this) {
                {
                    // set up UI
                    setPageTitle("Error");
                    setFormTitle("Error");

                    getContent().setLayout(new GridLayout());

                    Label label = new Label(getContent(), SWT.WRAP);
                    label.setText("Analysis could not be opened.");
                    label.setBackground(this.getBackground());
                    label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false));
                }
            });
        }
    }

    /**
     * This is how the framework determines which interfaces we implement.
     */
    @SuppressWarnings("unchecked")
    public <T> T getAdapter(Class<T> key) {
        if (key.equals(IContentOutlinePage.class))
            return showOutlineView() ? (T) getContentOutlinePage() : null;
        else if (key.equals(IPropertySheetPage.class))
            return (T) getPropertySheetPage();
        else if (key.equals(IGotoMarker.class))
            return (T) this;
        else
            return super.getAdapter(key);
    }

    /**
     * This is for implementing {@link IEditorPart} and simply tests the command
     * stack.
     */
    public boolean isDirty() {

        for (int i = 0; i < getPageCount(); ++i) {
            FormEditorPage editorPage = getEditorPage(i);
            if (editorPage instanceof InputsPage) {
                InputsPage inputsPage = (InputsPage)editorPage;

                CommandStack commandStack = inputsPage.getCommandStack();
                if (commandStack.isSaveNeeded())
                    return true;
            }
            else if (editorPage instanceof ChartsPage) {
                ChartsPage chartsPage = (ChartsPage)editorPage;

                CommandStack commandStack = chartsPage.getCommandStack();
                if (commandStack.isSaveNeeded())
                    return true;
            }
            else if (editorPage instanceof ChartPage) {
                ChartPage chartPage = (ChartPage)editorPage;

                ChartScriptEditor chartScriptEditor = chartPage.getChartScriptEditor();

                if (chartScriptEditor.isDirty())
                    return true;

				Chart chart = chartScriptEditor.getChart();

                CommandStack commandStack = chartScriptEditor.getCommandStack();
				if ((commandStack.isSaveNeeded()) || (chart.isTemporary() && commandStack.wasObjectAffected(chart)))
                    return true;
            }
        }

        return false;
    }

    /**
     * This is for implementing {@link IEditorPart} and simply saves the model file.
     */
    public void doSave(IProgressMonitor progressMonitor) {

        Map<Chart, String> editedScripts = new HashMap<Chart, String>();

        for (int i = 0; i < getPageCount(); ++i) {
            FormEditorPage editorPage = getEditorPage(i);
            if (editorPage instanceof ChartPage) {
                ChartPage chartPage = (ChartPage)editorPage;
                ChartScriptEditor chartScriptEditor = chartPage.getChartScriptEditor();
                askToKeepEditedTemporaryChart(chartScriptEditor);
                editedScripts.put(chartScriptEditor.getChart(), chartScriptEditor.getDocument().get());
            }
        }

        IFileEditorInput modelFile = (IFileEditorInput) getEditorInput();
        IFile f = modelFile.getFile();

        try {
            AnalysisSaver.saveAnalysis(analysis, f, editedScripts);

            // Refresh the necessary state.

            for (int i = 0; i < getPageCount(); ++i) {
                FormEditorPage editorPage = getEditorPage(i);
                if (editorPage instanceof InputsPage) {
                    InputsPage inputsPage = (InputsPage)editorPage;

                    CommandStack commandStack = inputsPage.getCommandStack();
                    commandStack.saved();
                }
                else if (editorPage instanceof ChartsPage) {
                    ChartsPage chartsPage = (ChartsPage)editorPage;

                    CommandStack commandStack = chartsPage.getCommandStack();
                    commandStack.saved();
                }
                else if (editorPage instanceof ChartPage) {
                    ChartPage chartPage = (ChartPage)editorPage;
                    ChartScriptEditor chartScriptEditor = chartPage.getChartScriptEditor();
                    chartScriptEditor.saved();
                }
            }

            firePropertyChange(IEditorPart.PROP_DIRTY);
        } catch (CoreException e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Cannot save .anf file", e.getMessage());
        }
    }

    /**
     * This always returns true because it is not currently supported.
     */
    public boolean isSaveAsAllowed() {
        return true;
    }

    /**
     * "Save As" also changes the editor's input.
     */
    public void doSaveAs() {
        SaveAsDialog saveAsDialog = new SaveAsDialog(getSite().getShell());
        saveAsDialog.open();
        IPath path = saveAsDialog.getResult();
        if (path != null) {
            IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(path);
            if (file != null)
                doSaveAs(new FileEditorInput(file));
        }
    }

    /**
     * Perform "Save As"
     */
    protected void doSaveAs(IEditorInput editorInput) {
        setInputWithNotify(editorInput);
        setPartName(editorInput.getName());
        IStatusLineManager statusLineManager = getEditorSite().getActionBars().getStatusLineManager();
        IProgressMonitor progressMonitor = statusLineManager != null ? statusLineManager.getProgressMonitor() : new NullProgressMonitor();
        IFile fileInput = ((FileEditorInput)editorInput).getFile();
        chartTemplateRegistry.setProject(fileInput.getProject());
        doSave(progressMonitor);
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider}.
     */
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.add(listener);
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider}.
     */
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionChangedListeners.remove(listener);
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to
     * return this editor's overall selection.
     */
    public ISelection getSelection() {
        return editorSelection;
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to set
     * this editor's overall selection. Calling this will result in notifying the
     * listeners.
     */
    public void setSelection(ISelection selection) {
        handleSelectionChange(selection);
    }

    /**
     * Utility method to update "Selected X objects" text on the status bar.
     */
    protected void updateStatusLineManager(IStatusLineManager statusLineManager, ISelection selection) {
        if (statusLineManager != null) {
            if (selection instanceof IDListSelection) {
                IDListSelection idlistSelection = (IDListSelection) selection;
                int scalars = idlistSelection.getScalarsCount();
                int vectors = idlistSelection.getVectorsCount();
                int statistics = idlistSelection.getStatisticsCount();
                int histograms = idlistSelection.getHistogramsCount();
                if (scalars + vectors + statistics + histograms == 0)
                    statusLineManager.setMessage("No item selected");
                else {
                    List<String> strings = new ArrayList<String>(3);
                    if (scalars > 0)
                        strings.add(StringUtils.formatCounted(scalars, "scalar"));
                    if (vectors > 0)
                        strings.add(StringUtils.formatCounted(vectors, "vector"));
                    if (statistics > 0)
                        strings.add(StringUtils.formatCounted(statistics, "statistics"));
                    if (histograms > 0)
                        strings.add(StringUtils.formatCounted(histograms, "histogram"));
                    String message = "Selected " + StringUtils.join(strings, ", ", " and ");
                    statusLineManager.setMessage(message);
                }
            } else if (selection instanceof IStructuredSelection) {
                Collection<?> collection = ((IStructuredSelection) selection).toList();
                if (collection.size() == 0) {
                    statusLineManager.setMessage("No item selected");
                } else if (collection.size() == 1) {
                    Object object = collection.iterator().next();
                    // XXX unify label providers
                    // String text = new InputsViewLabelProvider().getText(object);
                    // if (text == null)
                    String text = object.toString(); // TODO refine
                    statusLineManager.setMessage("Selected: " + text);
                } else {
                    statusLineManager.setMessage("" + collection.size() + " items selected");
                }
            } else {
                statusLineManager.setMessage("");
            }
        }
    }

    public ScaveEditorActions getActions() {
        return actions;
    }

    protected boolean showOutlineView() {
        return true;
    }

    public PythonProcessPool getPythonProcessPool() {
        return processPool;
    }

    public ChartTemplateRegistry getChartTemplateRegistry() {
        return chartTemplateRegistry;
    }

    protected int openChartScriptEditor(Chart chart) throws PartInitException {

        ChartScriptEditor editor = new ChartScriptEditor(this, chart);
        ChartScriptEditorInput input = new ChartScriptEditorInput(chart);

//        chart.addListener(new IModelChangeListener() {
//            @Override
//            public void modelChanged(ModelChangeEvent event) {
//                if (event.getSubject() == chart && !editor.getDocument().get().equals(chart.getScript()))
//                    editor.getDocument().set(chart.getScript());
//            }
//        });

        int index = addClosablePage(editor, input, editor.getChartDisplayName());

        IMemento editorMemento = getMementoFor(editor);
        if (editorMemento != null)
            editor.restoreState(editorMemento);

        return index;
    }

	public CommandStack getActiveCommandStack() {
		FormEditorPage activeEditorPage = getActiveEditorPage();
		if (activeEditorPage == inputsPage)
			return inputsPage.getCommandStack();
		else if (activeEditorPage == chartsPage)
			return chartsPage.getCommandStack();
		else if (activeEditorPage instanceof ChartPage)
			return ((ChartPage)activeEditorPage).getChartScriptEditor().getCommandStack();
		return null;
	}
}
