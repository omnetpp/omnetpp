/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;


import java.util.ArrayList;
import java.util.Collection;
import java.util.EventObject;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.command.BasicCommandStack;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CommandStackListener;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.ui.MarkerHelper;
import org.eclipse.emf.common.util.BasicDiagnostic;
import org.eclipse.emf.common.util.Diagnostic;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.edit.domain.IEditingDomainProvider;
import org.eclipse.emf.edit.provider.AdapterFactoryItemDelegator;
import org.eclipse.emf.edit.provider.ComposedAdapterFactory;
import org.eclipse.emf.edit.provider.ReflectiveItemProviderAdapterFactory;
import org.eclipse.emf.edit.provider.resource.ResourceItemProviderAdapterFactory;
import org.eclipse.emf.edit.ui.action.EditingDomainActionBarContributor;
import org.eclipse.emf.edit.ui.dnd.EditingDomainViewerDropAdapter;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.emf.edit.ui.util.EditUIMarkerHelper;
import org.eclipse.emf.edit.ui.view.ExtendedPropertySheetPage;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.util.LocalSelectionTransfer;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.StructuredViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.views.contentoutline.ContentOutline;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySheetEntry;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheet;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheetSorter;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.common.ui.MultiPageEditorPartExt;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.treeproviders.InputsViewLabelProvider;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelProvider;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.provider.ScaveEditPlugin;
import org.omnetpp.scave.model2.ResultItemRef;
import org.omnetpp.scave.model2.provider.ScaveModelItemProviderAdapterFactory;


/**
 * This is an abstract base class for EMF-based editors. Compare
 * with EMF-generated multi-page editors.
 */
//FIXME if Datasets is empty, context menu offers "New Input File" if Inputs is selected in the content outline
public abstract class AbstractEMFModelEditor extends MultiPageEditorPartExt
    implements IEditingDomainProvider, ISelectionProvider, IMenuListener, IGotoMarker {

    /**
     * This keeps track of the editing domain that is used to track all changes to the model.
     */
    protected AdapterFactoryEditingDomain editingDomain;

    /**
     * This is the one adapter factory used for providing views of the model.
     */
    protected ComposedAdapterFactory adapterFactory;

    /**
     * This is the content outline page.
     */
    protected IContentOutlinePage contentOutlinePage;

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
    ISelectionChangedListener selectionChangedListener = new ISelectionChangedListener() {
        public void selectionChanged(SelectionChangedEvent selectionChangedEvent) {
            handleSelectionChange(selectionChangedEvent.getSelection());
        }
    };

    /**
     * This keeps track of all the {@link org.eclipse.jface.viewers.ISelectionChangedListener}s that are listening to this editor.
     * We need this because we implement ISelectionProvider which includes having to manage a listener list.
     */
    protected Collection<ISelectionChangedListener> selectionChangedListeners = new ArrayList<ISelectionChangedListener>();

    /**
     * This keeps track of the selection of the editor as a whole.
     */
    protected ISelection editorSelection = StructuredSelection.EMPTY;

    protected MarkerHelper markerHelper = new EditUIMarkerHelper();

    /**
     * This listens for when the outline becomes active
     */
    protected IPartListener partListener =
        new IPartListener() {
            public void partActivated(IWorkbenchPart p) {
                if (p instanceof ContentOutline) {
                    if (((ContentOutline)p).getCurrentPage() == contentOutlinePage) {
                        getActionBarContributor().setActiveEditor(AbstractEMFModelEditor.this);
                    }
                }
                else if (p instanceof PropertySheet) {
                    if (propertySheetPages.contains(((PropertySheet)p).getCurrentPage())) {
                        getActionBarContributor().setActiveEditor(AbstractEMFModelEditor.this);
                        handleActivate();
                    }
                }
                else if (p == AbstractEMFModelEditor.this) {
                    handleActivate();
                }
            }
            public void partBroughtToTop(IWorkbenchPart p) {
            }
            public void partClosed(IWorkbenchPart p) {
            }
            public void partDeactivated(IWorkbenchPart p) {
            }
            public void partOpened(IWorkbenchPart p) {
            }
        };


    protected void handleActivate() {
    }
    
    /**
     * This creates a model editor.
     */
    public AbstractEMFModelEditor() {
        super();

        // Create an adapter factory that yields item providers.
        //
        List<AdapterFactory> factories = new ArrayList<AdapterFactory>();
        factories.add(new ResourceItemProviderAdapterFactory());
        factories.add(new ScaveModelItemProviderAdapterFactory());
        factories.add(new ReflectiveItemProviderAdapterFactory());

        adapterFactory = new ComposedAdapterFactory(factories);

        // Create the command stack that will notify this editor as commands are executed.
        //
        BasicCommandStack commandStack = new BasicCommandStack();

        // Add a listener to set the most recent command's affected objects to be the selection of the viewer with focus.
        //
        commandStack.addCommandStackListener
            (new CommandStackListener() {
                 public void commandStackChanged(final EventObject event) {
                     getContainer().getDisplay().asyncExec
                         (new Runnable() {
                              public void run() {
                                  firePropertyChange(IEditorPart.PROP_DIRTY);

                                  // Try to select the affected objects.
                                  //
                                  Command mostRecentCommand = ((CommandStack)event.getSource()).getMostRecentCommand();
                                  if (mostRecentCommand != null) {
                                      setSelectionToViewer(mostRecentCommand.getAffectedObjects());
                                  }
                                  for (Iterator<PropertySheetPage> i = propertySheetPages.iterator(); i.hasNext(); ) {
                                      PropertySheetPage propertySheetPage = i.next();
                                      if (propertySheetPage.getControl().isDisposed()) {
                                          i.remove();
                                      }
                                      else {
                                          propertySheetPage.refresh();
                                      }
                                  }
                              }
                          });
                 }
             });

        // Create the editing domain with a special command stack.
        //
        editingDomain = new AdapterFactoryEditingDomain(adapterFactory, commandStack, new HashMap<Resource,Boolean>());
    }

    /**
     * This is here for the listener to be able to call it.
     */
    protected void firePropertyChange(int action) {
        super.firePropertyChange(action);
    }

    /**
     * This sets the selection into whichever viewer is active.
     */
    public void setSelectionToViewer(Collection<?> collection) {
        handleSelectionChange(new StructuredSelection(collection.toArray()));
    }

    /**
     * Utility function to update selection in a viewer without generating
     * further notifications.
     */
    public void setViewerSelectionNoNotify(Viewer target, ISelection selection) {
        if (target!=null) {
            target.removeSelectionChangedListener(selectionChangedListener);
            target.setSelection(selection,true);
            target.addSelectionChangedListener(selectionChangedListener);
        }
    }

    boolean selectionChangeInProgress = false; // to prevent recursive notifications

    /**
     * Propagates the selection everywhere. Override if you have more widgets to update!
     */
    protected void handleSelectionChange(ISelection selection) {
        //FIXME merge this method into fireSelectionChangedEvent()!!! that's where the guard should be as well!
        boolean selectionReallyChanged = selection!=editorSelection && !selection.equals(editorSelection);
        if (!selectionChangeInProgress || selectionReallyChanged) {
            try {
                selectionChangeInProgress = true; // "finally" ensures this gets reset in any case
                editorSelection = selection;
                 // FIXME notifying the view about IDListSelections would remove the selection from the editor!
                if (!(selection instanceof IDListSelection)) {
                    //setViewerSelectionNoNotify(contentOutlineViewer, selection);
                    updateStatusLineManager(contentOutlineStatusLineManager, selection);
                }
                updateStatusLineManager(getActionBars().getStatusLineManager(), selection);
                fireSelectionChangedEvent(selection);
            } finally {
                selectionChangeInProgress = false;
            }
        }
    }

    /**
     * Notify listeners on {@link org.eclipse.jface.viewers.ISelectionProvider} about a selection change.
     */
    protected void fireSelectionChangedEvent(ISelection selection) {
        for (ISelectionChangedListener listener : selectionChangedListeners)
            listener.selectionChanged(new SelectionChangedEvent(this, selection));
    }

    /**
     * This returns the editing domain as required by the {@link IEditingDomainProvider} interface.
     * This is important for implementing the static methods of {@link AdapterFactoryEditingDomain}
     * and for supporting {@link org.eclipse.emf.edit.ui.action.CommandAction}.
     */
    public EditingDomain getEditingDomain() {
        return editingDomain;
    }


    /**
     * This creates a context menu for the viewer and adds a listener as well registering the menu for extension.
     */
    protected void createContextMenuFor(StructuredViewer viewer) {
        createContextMenuFor(viewer.getControl());
    }
    
    protected void createContextMenuFor(Control control) {
        MenuManager contextMenu = new MenuManager("#PopUp");
        contextMenu.add(new Separator("additions"));
        contextMenu.setRemoveAllWhenShown(true);
        contextMenu.addMenuListener(this);
        Menu menu = contextMenu.createContextMenu(control);
        control.setMenu(menu);

        // Note: don't register the context menu, otherwise "Run As", "Debug As", "Team", and other irrelevant menu items appear...
        //getSite().registerContextMenu(contextMenu, viewer);
    }

    /**
     * Sets up the viewer so that it functions as drag source and drop target for model elements
     */
    protected void setupDragAndDropSupportFor(StructuredViewer viewer) {
        int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
        Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance(), LocalSelectionTransfer.getTransfer(), FileTransfer.getInstance() };
        viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
        viewer.addDropSupport(dndOperations, transfers, new EditingDomainViewerDropAdapter(editingDomain, viewer));
    }

    /**
     * This is the method called to load a resource into the editing
     * domain's resource set based on the editor's input.
     */
    public void createModel() {
        // Assumes that the input is a file object.
        IFileEditorInput modelFile = (IFileEditorInput)getEditorInput();
        URI resourceURI = URI.createPlatformResourceURI(modelFile.getFile().getFullPath().toString(), true);;
        try {
            editingDomain.getResourceSet().getResource(resourceURI, true);
        }
        catch (Exception e) {
            ScavePlugin.logError("coul not load resource", e);
            //TODO dialog?
        }
    }

    /**
     * Returns a diagnostic describing the errors and warnings listed in the resource
     * and the specified exception (if any).
     */
    public Diagnostic analyzeResourceProblems(Resource resource, Exception exception) {
        if (!resource.getErrors().isEmpty() || !resource.getWarnings().isEmpty()) {
            BasicDiagnostic basicDiagnostic = new BasicDiagnostic(Diagnostic.ERROR, "org.omnetpp.scave.model", 0, "Could not create model: " + resource.getURI(),
                    new Object [] { exception == null ? (Object)resource : exception });
            basicDiagnostic.merge(EcoreUtil.computeDiagnostic(resource, true));
            return basicDiagnostic;
        }
        else if (exception != null) {
            return new BasicDiagnostic(Diagnostic.ERROR, "org.omnetpp.scave.model", 0, "Could not create model: " + resource.getURI(), new Object[] { exception });
        }
        else {
            return Diagnostic.OK_INSTANCE;
        }
    }


    /**
     * This is the method used by the framework to install your own controls.
     */
    public void createPages() {
        createModel();
        doCreatePages();
    }

    /**
     * createPages() delegates real work here.
     * @author Andras
     */
    protected abstract void doCreatePages();

    /**
     * Utility class to add content and label providers, context menu etc to a TreeViewer
     * that is used to edit the model.
     */
    public void configureTreeViewer(final TreeViewer modelViewer) {
        ILabelProvider labelProvider =
            new DecoratingLabelProvider(
                new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)),
                new ScaveModelLabelDecorator());
        modelViewer.setContentProvider(new AdapterFactoryContentProvider(adapterFactory));
        modelViewer.setLabelProvider(labelProvider);
        modelViewer.setAutoExpandLevel(TreeViewer.ALL_LEVELS);
        // new AdapterFactoryTreeEditor(modelViewer.getTree(), adapterFactory); //XXX this appears to be something about in-place editing - do we need it?

        modelViewer.addSelectionChangedListener(selectionChangedListener);

        createContextMenuFor(modelViewer);
        setupDragAndDropSupportFor(modelViewer);

        // on double-click, open (the dataset or chart), or bring up the Properties dialog
        modelViewer.getTree().addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
                if (contributor != null) {
                    if (contributor.getOpenAction().isEnabled())
                        contributor.getOpenAction().run();
                    else if (contributor.getEditAction().isEnabled())
                        contributor.getEditAction().run();
                }
            }
        });

        new HoverSupport().adapt(modelViewer.getTree(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                Item item = modelViewer.getTree().getItem(new Point(x,y));
                Object element = item==null ? null : item.getData();
                if (element != null && modelViewer.getLabelProvider() instanceof DecoratingLabelProvider) {
                    ILabelProvider labelProvider = ((DecoratingLabelProvider)modelViewer.getLabelProvider()).getLabelProvider();
                    if (labelProvider instanceof ScaveModelLabelProvider) {
                        ScaveModelLabelProvider scaveLabelProvider = (ScaveModelLabelProvider)labelProvider;
                        return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(scaveLabelProvider.getTooltipText(element)));
                    }
                }
                return null;
            }
        });
    }

    public void configureIconGridViewer(IconGridViewer modelViewer) {
//        ILabelProvider labelProvider =
//            new DecoratingLabelProvider(
//                new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)),
//                new ScaveModelLabelDecorator());

        ILabelProvider labelProvider = new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                if (element instanceof LineChart)
                    return ScavePlugin.getImage("icons/full/obj/linechart.png");
                else if (element instanceof BarChart)
                    return ScavePlugin.getImage("icons/full/obj/barchart.png");
                else if (element instanceof ScatterChart)
                    return ScavePlugin.getImage("icons/full/obj/scatterchart.png");
                else if (element instanceof HistogramChart)
                    return ScavePlugin.getImage("icons/full/obj/histogramchart.png");
                else 
                    return null;
            }
            
            @Override
            public String getText(Object element) {
                if (element instanceof Chart)
                    return StringUtils.defaultIfBlank(((Chart) element).getName(), "<unnamed>");
                else 
                    return element == null ? "" : element.toString();
            }
        };
        
        modelViewer.setContentProvider(new AdapterFactoryContentProvider(adapterFactory));
        modelViewer.setLabelProvider(labelProvider);
//        modelViewer.setAutoExpandLevel(TreeViewer.ALL_LEVELS);
        // new AdapterFactoryTreeEditor(modelViewer.getTree(), adapterFactory); //XXX this appears to be something about in-place editing - do we need it?

        modelViewer.addSelectionChangedListener(selectionChangedListener);

        createContextMenuFor(modelViewer.getControl());
        createContextMenuFor(modelViewer.getCanvas());
//        setupDragAndDropSupportFor(modelViewer);

        // on double-click, open (the dataset or chart), or bring up the Properties dialog
        modelViewer.addDoubleClickListener(new IDoubleClickListener() {
            @Override
            public void doubleClick(DoubleClickEvent event) {
                ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
                if (contributor != null) {
                    if (contributor.getOpenAction().isEnabled())
                        contributor.getOpenAction().run();
                    else if (contributor.getEditAction().isEnabled())
                        contributor.getEditAction().run();
                }
            }
        });
//
//        new HoverSupport().adapt(modelViewer.getTree(), new IHoverInfoProvider() {
//            @Override
//            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
//                Item item = modelViewer.getTree().getItem(new Point(x,y));
//                Object element = item==null ? null : item.getData();
//                if (element != null && modelViewer.getLabelProvider() instanceof DecoratingLabelProvider) {
//                    ILabelProvider labelProvider = ((DecoratingLabelProvider)modelViewer.getLabelProvider()).getLabelProvider();
//                    if (labelProvider instanceof ScaveModelLabelProvider) {
//                        ScaveModelLabelProvider scaveLabelProvider = (ScaveModelLabelProvider)labelProvider;
//                        return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(scaveLabelProvider.getTooltipText(element)));
//                    }
//                }
//                return null;
//            }
//        });
    }

    /**
     * This is how the framework determines which interfaces we implement.
     */
    @SuppressWarnings("unchecked")
    public Object getAdapter(Class key) {
        if (key.equals(IContentOutlinePage.class)) {
            return showOutlineView() ? getContentOutlinePage() : null;
        }
        else if (key.equals(IPropertySheetPage.class)) {
            return getPropertySheetPage();
        }
        else if (key.equals(IGotoMarker.class)) {
            return this;
        }
        else {
            return super.getAdapter(key);
        }
    }

    // The content outline is just a tree.
    //
    class MyContentOutlinePage extends ContentOutlinePage {
        public void createControl(Composite parent) {
            super.createControl(parent);
            contentOutlineViewer = getTreeViewer();
            contentOutlineViewer.addSelectionChangedListener(this);

            // Set up the tree viewer.
            //
            contentOutlineViewer.setContentProvider(new AdapterFactoryContentProvider(adapterFactory));
            //contentOutlineViewer.setLabelProvider(new AdapterFactoryLabelProvider(adapterFactory));
            contentOutlineViewer.setLabelProvider(
                    new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)));
            initializeContentOutlineViewer(contentOutlineViewer); // should call setInput()
            contentOutlineViewer.expandToLevel(3);

            // Make sure our popups work.
            //
            createContextMenuFor(contentOutlineViewer);

            if (!editingDomain.getResourceSet().getResources().isEmpty()) {
              // Select the root object in the view.
              //
              ArrayList<Object> selection = new ArrayList<Object>();
              selection.add(editingDomain.getResourceSet().getResources().get(0));
              contentOutlineViewer.setSelection(new StructuredSelection(selection), true);
            }
        }

        public void makeContributions(IMenuManager menuManager, IToolBarManager toolBarManager, IStatusLineManager statusLineManager) {
            super.makeContributions(menuManager, toolBarManager, statusLineManager);
            contentOutlineStatusLineManager = statusLineManager;
        }

        public void setActionBars(IActionBars actionBars) {
            super.setActionBars(actionBars);
            getActionBarContributor().shareGlobalActions(this, actionBars);
        }
    }

    /**
     * This accesses a cached version of the content outliner.
     */
    public IContentOutlinePage getContentOutlinePage() {
        if (contentOutlinePage == null) {
            contentOutlinePage = new MyContentOutlinePage();
            // Listen to selection so that we can handle it is a special way.
            //
            contentOutlinePage.addSelectionChangedListener(selectionChangedListener);
        }

        return contentOutlinePage;
    }

    /**
     * Should call contentOutlineViewer.setInput().
     */
    protected abstract void initializeContentOutlineViewer(Viewer contentOutlineViewer);

    /**
     * This accesses a cached version of the property sheet.
     */
    public IPropertySheetPage getPropertySheetPage() {
        PropertySheetPage propertySheetPage =
                new ExtendedPropertySheetPage(editingDomain) {
                    @Override
                    public void setSelectionToViewer(List<?> selection) {
                        AbstractEMFModelEditor.this.setSelectionToViewer(selection);
                        AbstractEMFModelEditor.this.setFocus();
                    }

                    // this is a constructor fragment --Andras
                    {
                        // turn off sorting for our INonsortablePropertyDescriptors
                        setSorter(new PropertySheetSorter() {
                            public int compare(IPropertySheetEntry entryA, IPropertySheetEntry entryB) {
                                IPropertyDescriptor descriptorA = (IPropertyDescriptor) ReflectionUtils.getFieldValue(entryA, "descriptor"); // it's f***ing private --Andras
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
                        getActionBarContributor().shareGlobalActions(this, actionBars);
                    }

                    @Override
                    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                        if (selection instanceof IDListSelection) {
                            // re-package element into ResultItemRef, otherwise property sheet
                            // only gets a Long due to sel.toArray() in base class. We only want
                            // to show properties if there's only one item in the selection.
                            IDListSelection idList = (IDListSelection)selection;
                            if (idList.size() == 1) {
                                long id = (Long)idList.getFirstElement();
                                ResultItemRef resultItemRef = new ResultItemRef(id, idList.getResultFileManager());
                                selection = new StructuredSelection(resultItemRef);
                            }
                        }
                        super.selectionChanged(part, selection);
                    }
                };

        propertySheetPage.setPropertySourceProvider(new AdapterFactoryContentProvider(adapterFactory));
        propertySheetPages.add(propertySheetPage);

        return propertySheetPage;
    }

    /**
     * This is for implementing {@link IEditorPart} and simply tests the command stack.
     */
    public boolean isDirty() {
        return ((BasicCommandStack)editingDomain.getCommandStack()).isSaveNeeded();
    }

    /**
     * This is for implementing {@link IEditorPart} and simply saves the model file.
     */
    public void doSave(IProgressMonitor progressMonitor) {
        // Save only resources that have actually changed.
        //
        final Map<Object, Object> saveOptions = new HashMap<Object, Object>();
        saveOptions.put(Resource.OPTION_SAVE_ONLY_IF_CHANGED, Resource.OPTION_SAVE_ONLY_IF_CHANGED_MEMORY_BUFFER);
        saveOptions.put(Resource.OPTION_LINE_DELIMITER, Resource.OPTION_LINE_DELIMITER_UNSPECIFIED);

        // Do the work within an operation because this is a long running activity that modifies the workbench.
        WorkspaceModifyOperation operation =
            new WorkspaceModifyOperation() {
                // This is the method that gets invoked when the operation runs.
                public void execute(IProgressMonitor monitor) {
                    // Save the resources to the file system.
                    EList<Resource> resources = editingDomain.getResourceSet().getResources();
                    Assert.isTrue(resources.size() == 1);
                    Resource resource = resources.get(0);
                    if (!editingDomain.isReadOnly(resource)) {
                        try {
                            resource.save(saveOptions);
                        }
                        catch (Exception e) {
                            ScavePlugin.logError("Could not save resource", e);
                        }
                    }
                }
            };

        try {
            // This runs the options, and shows progress.
            new ProgressMonitorDialog(getSite().getShell()).run(true, false, operation);

            // Refresh the necessary state.
            ((BasicCommandStack)editingDomain.getCommandStack()).saveIsDone();
            firePropertyChange(IEditorPart.PROP_DIRTY);
        }
        catch (Exception exception) {
            ScaveEditPlugin.INSTANCE.log(exception);
        }
    }

    protected boolean isSaveable(Resource resource) {
        return true;
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
        SaveAsDialog saveAsDialog= new SaveAsDialog(getSite().getShell());
        saveAsDialog.open();
        IPath path= saveAsDialog.getResult();
        if (path != null) {
            IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(path);
            if (file != null)
                doSaveAs(URI.createPlatformResourceURI(file.getFullPath().toString(), true), new FileEditorInput(file));
        }
    }

    /**
     * Perform "Save As"
     */
    protected void doSaveAs(URI uri, IEditorInput editorInput) {
        editingDomain.getResourceSet().getResources().get(0).setURI(uri);
        setInputWithNotify(editorInput);
        setPartName(editorInput.getName());
        IProgressMonitor progressMonitor =
            getActionBars().getStatusLineManager() != null ?
                getActionBars().getStatusLineManager().getProgressMonitor() :
                new NullProgressMonitor();
        doSave(progressMonitor);
    }

    public void gotoMarker(IMarker marker) {
        List<?> targetObjects = markerHelper.getTargetObjects(editingDomain, marker);
        if (!targetObjects.isEmpty())
            setSelectionToViewer(targetObjects);
    }

    public void init(IEditorSite site, IEditorInput editorInput)
        throws PartInitException {
        setSite(site);
        setInputWithNotify(editorInput);
        setPartName(editorInput.getName());
        site.setSelectionProvider(this);
        site.getPage().addPartListener(partListener);
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
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to return this editor's overall selection.
     */
    public ISelection getSelection() {
        return editorSelection;
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionProvider} to set this editor's overall selection.
     * Calling this will result in notifing the listeners.
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
                IDListSelection idlistSelection = (IDListSelection)selection;
                int scalars = idlistSelection.getScalarsCount();
                int vectors = idlistSelection.getVectorsCount();
                int statistics = idlistSelection.getStatisticsCount();
                int histograms = idlistSelection.getHistogramsCount();
                if (scalars + vectors + statistics + histograms == 0)
                    statusLineManager.setMessage("No item selected");
                else {
                    List<String> strings = new ArrayList<String>(3);
                    if (scalars > 0) strings.add(StringUtils.formatCounted(scalars, "scalar"));
                    if (vectors > 0) strings.add(StringUtils.formatCounted(vectors, "vector"));
                    if (statistics > 0) strings.add(StringUtils.formatCounted(statistics, "statistics"));
                    if (histograms > 0) strings.add(StringUtils.formatCounted(histograms, "histogram"));
                    String message = "Selected " + StringUtils.join(strings, ", ", " and ");
                    statusLineManager.setMessage(message);
                }
            }
            else if (selection instanceof IStructuredSelection) {
                Collection<?> collection = ((IStructuredSelection)selection).toList();
                if (collection.size()==0) {
                        statusLineManager.setMessage("No item selected");
                }
                else if (collection.size()==1) {
                    Object object = collection.iterator().next();
                     // XXX unify label providers
                    String text = new InputsViewLabelProvider().getText(object);
                    if (text == null)
                        text = new AdapterFactoryItemDelegator(adapterFactory).getText(object);
                    statusLineManager.setMessage("Selected: " + text);
                }
                else {
                        statusLineManager.setMessage("" + collection.size() + " items selected");
                }
            }
            else {
                statusLineManager.setMessage("");
            }
        }
    }

    /**
     * This implements {@link org.eclipse.jface.action.IMenuListener} to help fill the context menus with contributions from the Edit menu.
     */
    public void menuAboutToShow(IMenuManager menuManager) {
        ((IMenuListener)getEditorSite().getActionBarContributor()).menuAboutToShow(menuManager);
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public EditingDomainActionBarContributor getActionBarContributor() {
        if (getEditorSite() != null)
            return (EditingDomainActionBarContributor)getEditorSite().getActionBarContributor();
        else
            return null;
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public IActionBars getActionBars() {
        return getActionBarContributor().getActionBars();
    }

    /**
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @generated
     */
    public AdapterFactory getAdapterFactory() {
        return adapterFactory;
    }

    /**
     * Dispose widget.
     */
    public void dispose() {
        if (getSite() != null && getSite().getPage() != null)
            getSite().getPage().removePartListener(partListener);

        adapterFactory.dispose();

        if (getActionBarContributor() != null && getActionBarContributor().getActiveEditor() == this) {
            getActionBarContributor().setActiveEditor(null);
        }

        for (PropertySheetPage propertySheetPage : propertySheetPages) {
            propertySheetPage.dispose();
        }

        if (contentOutlinePage != null) {
            contentOutlinePage.dispose();
        }

        super.dispose();
    }

    /**
     * Returns whether the outline view should be presented to the user (YES).
     */
    protected boolean showOutlineView() {
        return true;
    }
}
