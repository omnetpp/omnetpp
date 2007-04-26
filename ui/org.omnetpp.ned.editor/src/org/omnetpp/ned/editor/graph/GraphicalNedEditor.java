package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.EventObject;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.KeyHandler;
import org.eclipse.gef.KeyStroke;
import org.eclipse.gef.MouseWheelHandler;
import org.eclipse.gef.MouseWheelZoomHandler;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.dnd.TemplateTransferDropTargetListener;
import org.eclipse.gef.editparts.ScalableRootEditPart;
import org.eclipse.gef.editparts.ZoomManager;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.actions.AlignmentAction;
import org.eclipse.gef.ui.actions.DirectEditAction;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.gef.ui.actions.MatchHeightAction;
import org.eclipse.gef.ui.actions.MatchWidthAction;
import org.eclipse.gef.ui.actions.ToggleSnapToGeometryAction;
import org.eclipse.gef.ui.actions.ZoomInAction;
import org.eclipse.gef.ui.actions.ZoomOutAction;
import org.eclipse.gef.ui.parts.ContentOutlinePage;
import org.eclipse.gef.ui.parts.GraphicalEditorWithFlyoutPalette;
import org.eclipse.gef.ui.parts.GraphicalViewerKeyHandler;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.gef.ui.parts.SelectionSynchronizer;
import org.eclipse.gef.ui.parts.TreeViewer;
import org.eclipse.gef.ui.properties.UndoablePropertySheetEntry;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.util.TransferDropTargetListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.IPageSite;
import org.eclipse.ui.part.MultiPageEditorSite;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.omnetpp.common.editor.ShowViewAction;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.actions.ConvertToNewFormatAction;
import org.omnetpp.ned.editor.graph.actions.GNEDContextMenuProvider;
import org.omnetpp.ned.editor.graph.actions.ReLayoutAction;
import org.omnetpp.ned.editor.graph.actions.UnpinAction;
import org.omnetpp.ned.editor.graph.edit.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.edit.outline.NedTreeEditPartFactory;
import org.omnetpp.ned.editor.graph.misc.NedSelectionSynchronizer;
import org.omnetpp.ned.editor.graph.misc.PaletteManager;
import org.omnetpp.ned.editor.graph.properties.IPropertySourceSupport;
import org.omnetpp.ned.editor.graph.properties.NedPropertySourceAdapterFactory;
import org.omnetpp.ned.editor.graph.properties.view.BasePreferrerPropertySheetSorter;
import org.omnetpp.ned.editor.graph.properties.view.PropertySheetPageEx;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.NedFileNodeEx;


/**
 * This is the main class for the graphical NED editor
 * @author rhornig
 */
public class GraphicalNedEditor extends GraphicalEditorWithFlyoutPalette {

    class OutlinePage extends ContentOutlinePage {

        public OutlinePage(EditPartViewer viewer) {
            super(viewer);
        }

        @Override
        public void init(IPageSite pageSite) {
            super.init(pageSite);
            // register actions for the ediotr
            ActionRegistry registry = getActionRegistry();
            IActionBars bars = pageSite.getActionBars();

            String id = ActionFactory.UNDO.getId();
            bars.setGlobalActionHandler(id, registry.getAction(id));
            id = ActionFactory.REDO.getId();
            bars.setGlobalActionHandler(id, registry.getAction(id));
            id = ActionFactory.DELETE.getId();
            bars.setGlobalActionHandler(id, registry.getAction(id));
            bars.updateActionBars();
        }

        protected void configureOutlineViewer() {
            getViewer().setEditDomain(getEditDomain());
            getViewer().setEditPartFactory(new NedTreeEditPartFactory());
            ContextMenuProvider provider = new GNEDContextMenuProvider(getViewer(), getActionRegistry());
            getViewer().setContextMenu(provider);
            getSite().registerContextMenu("org.omnetpp.ned.editor.graph.outline.contextmenu", //$NON-NLS-1$
                    provider, getSite().getSelectionProvider());
            getViewer().setKeyHandler(getCommonKeyHandler());
            getViewer().addDropTargetListener((TransferDropTargetListener)
        			new TemplateTransferDropTargetListener(getViewer()));
        }

        @Override
        public void createControl(Composite parent) {
            super.createControl(parent);
            configureOutlineViewer();
            getSelectionSynchronizer().addViewer(getViewer());
            setContents(getModel());
        }

        @Override
        public void dispose() {
            getSelectionSynchronizer().removeViewer(getViewer());
            super.dispose();
        }

        public void setContents(Object contents) {
            getViewer().setContents(contents);
        }

    }

    private KeyHandler sharedKeyHandler;
    private PaletteManager paletteManager;
    private OutlinePage outlinePage;
    private boolean editorSaving = false;

    private NedFileNodeEx nedFileModel;
    private SelectionSynchronizer synchronizer;
    private Command lastUndoCommand;
    private Command lastRedoCommand;

    public GraphicalNedEditor() {
        setEditDomain(new DefaultEditDomain(this));

        // register the PropertySourceFactory for the different model elements
        Platform.getAdapterManager().registerAdapters(new NedPropertySourceAdapterFactory(), 
                IPropertySourceSupport.class);

    }

    @Override
    public void dispose() {
        NEDResourcesPlugin.getNEDResources().getNEDComponentChangeListenerList().remove(paletteManager);
        super.dispose();
    }
    
    @Override
    protected SelectionSynchronizer getSelectionSynchronizer() {
        if (synchronizer == null)
            synchronizer = new NedSelectionSynchronizer();
        return synchronizer;
    }
    
    protected PaletteRoot getPaletteRoot() {
        if (paletteManager == null) {
            paletteManager = new PaletteManager(this);
            // attach the palette manager as a listener to the resource manager plugin
            // so it will be notified if the palette should be updated
            NEDResourcesPlugin.getNEDResources().getNEDComponentChangeListenerList().add(paletteManager);
        }
        return paletteManager.getRootPalette();
    }

    @Override
    public void commandStackChanged(EventObject event) {
    	firePropertyChange(IEditorPart.PROP_DIRTY);
    	super.commandStackChanged(event);
    }

    @SuppressWarnings("unchecked")
	@Override
    protected void configureGraphicalViewer() {
        super.configureGraphicalViewer();
        ScrollingGraphicalViewer viewer = (ScrollingGraphicalViewer) getGraphicalViewer();
        
        ScalableRootEditPart root = new ScalableRootEditPart();

        List<String> zoomLevels = new ArrayList<String>(3);
        zoomLevels.add(ZoomManager.FIT_ALL);
        zoomLevels.add(ZoomManager.FIT_WIDTH);
        zoomLevels.add(ZoomManager.FIT_HEIGHT);
        root.getZoomManager().setZoomLevelContributions(zoomLevels);

        IAction zoomIn = new ZoomInAction(root.getZoomManager());
        getActionRegistry().registerAction(zoomIn);
        IAction zoomOut = new ZoomOutAction(root.getZoomManager());
        getActionRegistry().registerAction(zoomOut);

        // set the root edit part as the main viewer
        viewer.setRootEditPart(root);

        viewer.setEditPartFactory(new NedEditPartFactory());
        ContextMenuProvider provider = new GNEDContextMenuProvider(viewer, getActionRegistry());
        viewer.setContextMenu(provider);
        getSite().registerContextMenu("org.omnetpp.ned.editor.graph.contextmenu", provider, viewer);
        viewer.setKeyHandler(new GraphicalViewerKeyHandler(viewer).setParent(getCommonKeyHandler()));

        loadProperties();

        // Actions

        IAction snapAction = new ToggleSnapToGeometryAction(getGraphicalViewer());
        getActionRegistry().registerAction(snapAction);

    }

    @Override
    public DefaultEditDomain getEditDomain() {
        // overridden to make it visible
        return super.getEditDomain();
    }

    @Override
    public void doSave(final IProgressMonitor progressMonitor) {
        Assert.isTrue(false, "save is not implemented");
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.parts.GraphicalEditorWithFlyoutPalette#getAdapter(java.lang.Class)
     * Adds content outline and zoom support
     */
    @Override
    @SuppressWarnings("unchecked")
    public Object getAdapter(Class type) {
        if (type == org.eclipse.ui.views.properties.IPropertySheetPage.class) {
            PropertySheetPageEx page = new PropertySheetPageEx();
            // customize the property sheet with a custom sorter (to place the "Base"
            // category at the beginning)
            page.setSorter(new BasePreferrerPropertySheetSorter());
            page.setRootEntry(new UndoablePropertySheetEntry(getCommandStack()));
            return page;
        }
        if (type == IContentOutlinePage.class) {
            if (outlinePage == null)
                outlinePage = new OutlinePage(new TreeViewer());
            return outlinePage;
        }
        if (type == ZoomManager.class)
            return getGraphicalViewer().getProperty(ZoomManager.class.toString());

        return super.getAdapter(type);
    }

    /**
     * Returns the KeyHandler with common bindings for both the Outline and
     * Graphical Views. For example, delete is a common action.
     */
    protected KeyHandler getCommonKeyHandler() {
        if (sharedKeyHandler == null) {
            sharedKeyHandler = new KeyHandler();
            sharedKeyHandler.put(KeyStroke.getPressed(SWT.F2, 0), getActionRegistry().getAction(
                    GEFActionConstants.DIRECT_EDIT));
        }
        return sharedKeyHandler;
    }


    @Override
    protected void initializeGraphicalViewer() {
        super.initializeGraphicalViewer();
        getGraphicalViewer().setContents(getModel());
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.parts.GraphicalEditor#createActions()
     * Register the used acions
     */
    @SuppressWarnings("unchecked")
	@Override
    protected void createActions() {
        super.createActions();
        ActionRegistry registry = getActionRegistry();
        IAction action;

        action = new MatchWidthAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new MatchHeightAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ConvertToNewFormatAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new DirectEditAction((IWorkbenchPart) this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ShowViewAction(IPageLayout.ID_PROP_SHEET);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.LEFT);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.RIGHT);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.TOP);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.BOTTOM);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.CENTER);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new AlignmentAction((IWorkbenchPart) this, PositionConstants.MIDDLE);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new UnpinAction((IWorkbenchPart)this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ReLayoutAction((IWorkbenchPart)this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());
    }

    protected FigureCanvas getEditor() {
        return (FigureCanvas) getGraphicalViewer().getControl();
    }

    @Override
    public boolean isSaveAsAllowed() {
        return true;
    }

    protected void loadProperties() {
        // Scroll-wheel Zoom support
        getGraphicalViewer().setProperty(MouseWheelHandler.KeyGenerator.getKey(SWT.MOD1),
                MouseWheelZoomHandler.SINGLETON);
    }

    public NedFileNodeEx getModel() {
        return nedFileModel;
    }

    public void setModel(NedFileNodeEx nedModel) {
        if (nedFileModel == nedModel)
            return;

        nedFileModel = nedModel;
        // flush the stack so if a new model was added we cannot redo/undo anything
        getCommandStack().flush();

        if (!editorSaving) {
            if (getGraphicalViewer() != null) {
                getGraphicalViewer().setContents(getModel());
                loadProperties();
            }
            if (outlinePage != null) {
                outlinePage.setContents(getModel());
            }
        }
    }

    // XXX HACK OVERRIDDEN because selection does not work if the editor is embedded in a multipage editor
    // GraphicalEditor doesn't accept the selection change event if embedded in a multipage editor
    // hack can be removed once the issue corrected in GEF
    // see bug https://bugs.eclipse.org/bugs/show_bug.cgi?id=107703
    // see http://dev.eclipse.org/newslists/news.eclipse.tools.gef/msg10485.html
    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
        // update actions ONLY if we are the active editor or the parent editor which is a multipage editor
        IEditorPart activeEditor = getSite().getPage().getActiveEditor();
        if (this == activeEditor ||
              (getSite() instanceof MultiPageEditorSite &&
              ((MultiPageEditorSite)getSite()).getMultiPageEditor() == activeEditor))
            updateActions(getSelectionActions());
    }

    /**
     * Reveals a model element in the editor (or its nearest ancestor which 
     * has an associated editPart)
     * @param model
     */
    public void reveal(NEDElement model) {
        EditPart editPart = null;
         while( (model != null) && 
                (editPart = (EditPart)getGraphicalViewer().getEditPartRegistry().get(model)) == null)
             model = model.getParent();
        
         if (editPart == null) {
             getGraphicalViewer().deselectAll();
         } else { 
             getGraphicalViewer().reveal(editPart);
             getGraphicalViewer().select(editPart);
         }
    }
    
    /**
     * Marks the current editor content state, so we will be able to detect any change in the editor
     * used for editor change optimization
     */
    public void markContent() {
        lastUndoCommand = getCommandStack().getUndoCommand();
        lastRedoCommand = getCommandStack().getRedoCommand();
    }

    /**
     * @return Whether the content of the editor has changed since the last markContent call.
     */
    public boolean hasContentChanged() {
        return !(lastUndoCommand == getCommandStack().getUndoCommand() && lastRedoCommand == getCommandStack().getRedoCommand());
        
    }
    
}