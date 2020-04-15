/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph;

import java.util.ArrayList;
import java.util.EventObject;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.FigureCanvas;
import org.eclipse.draw2d.FocusEvent;
import org.eclipse.draw2d.FocusListener;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ImageFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MouseEvent;
import org.eclipse.draw2d.MouseListener;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.ContextMenuProvider;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.KeyHandler;
import org.eclipse.gef.KeyStroke;
import org.eclipse.gef.SelectionManager;
import org.eclipse.gef.SnapToGeometry;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CommandStack;
import org.eclipse.gef.editparts.ScalableRootEditPart;
import org.eclipse.gef.internal.ui.palette.editparts.DrawerEditPart;
import org.eclipse.gef.internal.ui.palette.editparts.DrawerFigure;
import org.eclipse.gef.internal.ui.palette.editparts.GroupEditPart;
import org.eclipse.gef.palette.PaletteContainer;
import org.eclipse.gef.palette.PaletteDrawer;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.ui.actions.ActionRegistry;
import org.eclipse.gef.ui.actions.AlignmentAction;
import org.eclipse.gef.ui.actions.GEFActionConstants;
import org.eclipse.gef.ui.actions.MatchHeightAction;
import org.eclipse.gef.ui.actions.MatchWidthAction;
import org.eclipse.gef.ui.actions.PrintAction;
import org.eclipse.gef.ui.palette.PaletteContextMenuProvider;
import org.eclipse.gef.ui.palette.PaletteEditPartFactory;
import org.eclipse.gef.ui.palette.PaletteViewer;
import org.eclipse.gef.ui.palette.PaletteViewerPreferences;
import org.eclipse.gef.ui.palette.PaletteViewerProvider;
import org.eclipse.gef.ui.palette.editparts.PaletteAnimator;
import org.eclipse.gef.ui.parts.GraphicalEditorWithFlyoutPalette;
import org.eclipse.gef.ui.parts.GraphicalViewerKeyHandler;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.gef.ui.parts.SelectionSynchronizer;
import org.eclipse.gef.ui.parts.TreeViewer;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.contexts.IContextService;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.MultiPageEditorSite;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.Debug;
import org.omnetpp.common.editor.text.NedCommentFormatter;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.HtmlHoverInfo;
import org.omnetpp.common.ui.IHoverInfoProvider;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.NedEditor;
import org.omnetpp.ned.editor.NedEditorPlugin;
import org.omnetpp.ned.editor.NedEditorPreferenceInitializer;
import org.omnetpp.ned.editor.graph.actions.CopyAction;
import org.omnetpp.ned.editor.graph.actions.CutAction;
import org.omnetpp.ned.editor.graph.actions.ExportImageAction;
import org.omnetpp.ned.editor.graph.actions.GNedContextMenuProvider;
import org.omnetpp.ned.editor.graph.actions.NedDirectEditAction;
import org.omnetpp.ned.editor.graph.actions.NedSelectAllAction;
import org.omnetpp.ned.editor.graph.actions.OpenTypeAction;
import org.omnetpp.ned.editor.graph.actions.PaletteFilterAction;
import org.omnetpp.ned.editor.graph.actions.ParametersDialogAction;
import org.omnetpp.ned.editor.graph.actions.PasteAction;
import org.omnetpp.ned.editor.graph.actions.PropertiesAction;
import org.omnetpp.ned.editor.graph.actions.RelayoutAction;
import org.omnetpp.ned.editor.graph.actions.ScaleDownIconsAction;
import org.omnetpp.ned.editor.graph.actions.ScaleUpIconsAction;
import org.omnetpp.ned.editor.graph.actions.TogglePinAction;
import org.omnetpp.ned.editor.graph.actions.ToggleSnapToGeometryAction;
import org.omnetpp.ned.editor.graph.actions.ZoomInAction;
import org.omnetpp.ned.editor.graph.actions.ZoomOutAction;
import org.omnetpp.ned.editor.graph.commands.ExternalChangeCommand;
import org.omnetpp.ned.editor.graph.misc.NedSelectionSynchronizer;
import org.omnetpp.ned.editor.graph.misc.PaletteManager;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.NedEditPartFactory;
import org.omnetpp.ned.editor.graph.properties.NedPropertySheetPage;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedAttributeChangeEvent;
import org.omnetpp.ned.model.notification.NedBeginModelChangeEvent;
import org.omnetpp.ned.model.notification.NedEndModelChangeEvent;
import org.omnetpp.ned.model.notification.NedFileRemovedEvent;
import org.omnetpp.ned.model.notification.NedModelChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.SubmoduleElement;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;


/**
 * This is the main class for the graphical NED editor. The editor should work without external dependencies
 * It stores its own model as an INEDModel tree, no dependencies from the embedding multi page editor
 * or the sibling text editor allowed. On editor load the provided editorInput (IFile) is used to lookup
 * the NEDResources plugin for the model.
 *
 * @author rhornig
 */
@SuppressWarnings("restriction")
public class GraphicalNedEditor
    extends GraphicalEditorWithFlyoutPalette
    implements INedChangeListener
{
    private static final String PROP_PALETTE_FILTER = "paletteFilter";
    public final static Color HIGHLIGHT_COLOR = new Color(null, 255, 0, 0);
    public final static Color LOWLIGHT_COLOR = new Color(null, 128, 0, 0);
    public final static String ID = NedEditor.ID+".graph";

    private KeyHandler sharedKeyHandler;
    private PaletteManager paletteManager;
    private NedOutlinePage outlinePage;
    private NedPropertySheetPage propertySheetPage;
    private boolean editorSaving = false;
    private SelectionSynchronizer synchronizer;

    // last state of the command stack (used to detect changes since last page switch)
    private Command lastUndoCommand;

    // storage for NED change notifications enclosed in begin/end
    private ExternalChangeCommand pendingExternalChangeCommand;

    // open NEDBeginChangeEvent notifications
    private int nedBeginChangeCount = 0;

    private DelayedJob paletteRefreshJob = new DelayedJob(100) {
        public void run() {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    if (getGraphicalControl() != null && !getGraphicalControl().isDisposed()) {
                        paletteManager.refresh();
                        PaletteViewer paletteViewer = getPaletteViewerProvider().getEditDomain().getPaletteViewer();
                        EditPart lastUsedToolEntry = (EditPart)paletteViewer.getEditPartRegistry().get(paletteManager.getLastUsedCreationToolEntry());
                        if (lastUsedToolEntry != null)
                            paletteViewer.reveal(lastUsedToolEntry);
                    }
                }
            });
        }
    };

    public GraphicalNedEditor() {
        paletteManager = new PaletteManager(this);

        DefaultEditDomain editDomain = new DefaultEditDomain(this);

        editDomain.setCommandStack(new LocalCommandStack());
        setEditDomain(editDomain);
    }

    @Override @SuppressWarnings("unchecked")
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);

        // we listen on changes too
        NedResourcesPlugin.getNedResources().addNedModelChangeListener(this);

        IContextService contextService = (IContextService)site.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.nedGraphEditor");
        // restore palette filter from a persistent property
        PersistentResourcePropertyManager propertyManager = new PersistentResourcePropertyManager(NedEditorPlugin.PLUGIN_ID);
        try {
            if (propertyManager.hasProperty(getFile(), PROP_PALETTE_FILTER)) {
                Set<String> excludedPackages = (Set<String>)propertyManager.getProperty(getFile(), PROP_PALETTE_FILTER);
                getPaletteManager().setExcludedPackages(excludedPackages);
            }
        }
        catch (Exception e) {
            propertyManager.removeProperty(getFile(), PROP_PALETTE_FILTER);
            NedEditorPlugin.logError("Cannot restore palette filter",  e);
        }
    }

    @Override
    public void dispose() {
        NedResourcesPlugin.getNedResources().removeNedModelChangeListener(this);
        paletteRefreshJob.cancel();

        // save palette filter to a persistent property
        PersistentResourcePropertyManager propertyManager = new PersistentResourcePropertyManager(NedEditorPlugin.PLUGIN_ID);
        try {
            if (getFile().getProject().exists())
                propertyManager.setProperty(getFile(), PROP_PALETTE_FILTER, getPaletteManager().getExcludedPackages());
        }
        catch (Exception e) {
            NedEditorPlugin.logError("cannot save palette filter",  e);
        }

        super.dispose();
    }

    @Override
    protected SelectionSynchronizer getSelectionSynchronizer() {
        if (synchronizer == null)
            synchronizer = new NedSelectionSynchronizer();
        return synchronizer;
    }

    @Override
    protected PaletteRoot getPaletteRoot() {
        return paletteManager.getRootPalette();
    }

    public PaletteManager getPaletteManager() {
        return paletteManager;
    }

    @Override
    protected PaletteViewerProvider createPaletteViewerProvider() {
        // overridden to add an extra item to the palette's context menu (Andras)
        return new PaletteViewerProvider(getEditDomain()) {
            @Override
            protected void configurePaletteViewer(PaletteViewer viewer) {
                viewer.setEditPartFactory(new PaletteEditPartFactory() {
                    @Override
                    protected EditPart createDrawerEditPart(EditPart parentEditPart, Object model) {
                        PaletteDrawer drawer = (PaletteDrawer)model;
                        if (drawer.getLabel().equals("Submodules"))
                            return new LocalDrawerEditPart(drawer);
                        else
                            // NOTE: makes the Types palette drawer stick to icons layout to save space for the Submodules palette drawer
                            return new DrawerEditPart((PaletteDrawer)model) {
                                @Override
                                protected int getLayoutSetting() {
                                    return PaletteViewerPreferences.LAYOUT_ICONS;
                                }
                            };
                    }

                    @Override
                    protected EditPart createGroupEditPart(EditPart parentEditPart, Object model) {
                        // NOTE: makes the Tools palette group stick to list layout to save space for the Submodules palette drawer
                        return new GroupEditPart((PaletteContainer)model) {
                            @Override
                            protected int getLayoutSetting() {
                                return PaletteViewerPreferences.LAYOUT_LIST;
                            }
                        };
                    }
                });
                viewer.setContextMenu(new PaletteContextMenuProvider(viewer) {
                    @Override
                    public void buildContextMenu(IMenuManager menu) {
                        super.buildContextMenu(menu);
                        menu.appendToGroup(GEFActionConstants.MB_ADDITIONS, new PaletteFilterAction(GraphicalNedEditor.this));
                    }
                });
            }
        };
    }

    @Override
    public void commandStackChanged(EventObject event) {
        firePropertyChange(IEditorPart.PROP_DIRTY);
        super.commandStackChanged(event);
    }

    @Override
    protected void initializeGraphicalViewer() {
        super.initializeGraphicalViewer();
        getGraphicalViewer().setContents(getModel());
    }

    @Override @SuppressWarnings({ "unchecked", "deprecation" })
    protected void configureGraphicalViewer() {
        super.configureGraphicalViewer();
        ScrollingGraphicalViewer viewer = (ScrollingGraphicalViewer) getGraphicalViewer();

        // Kludge: override getSelection() to remove invalid EditParts from the returned
        // selection. This is needed because during refreshChildren(), editPart.removeNotify()
        // gets called and fires a selection change. Receivers of the selection change
        // would find invalid editParts in the selection (editParts whose NedElement is no
        // longer in the model), because refreshChildren() has not completed yet.
        // Ideally, GEF should arrange that selection change only gets fired when the graphical
        // editor is already in a consistent state; here the workaround is to manually remove
        // those editParts from the returned selection.
        //
        viewer.setSelectionManager(new SelectionManager() {
            @Override
            public ISelection getSelection() {
                List sel = ((IStructuredSelection)super.getSelection()).toList();
                List newSel = new ArrayList();
                for (Object o : sel) {
                    if (o instanceof INedModelProvider) {
                        INedElement nedElement = ((INedModelProvider)o).getModel();
                        if (nedElement.getContainingNedFileElement() != null)
                            newSel.add(o);
                    }
                }
                return new StructuredSelection(newSel);
            }
        });

        ScalableRootEditPart root = new ScalableRootEditPart();

        // set the root edit part as the main viewer
        viewer.setRootEditPart(root);

        viewer.setEditPartFactory(new NedEditPartFactory());
        ContextMenuProvider provider = new GNedContextMenuProvider(viewer, getActionRegistry(), getSite());
        viewer.setProperty(SnapToGeometry.PROPERTY_SNAP_ENABLED, NedEditorPlugin.getDefault().getPreferenceStore().getBoolean(NedEditorPreferenceInitializer.SNAP_TO_GEOMETRY));
        viewer.setContextMenu(provider);
        // register the menu so we can contribute to it from other plugins BUT do not include the
        // contributions for the editor input (otherwise we will get a ton of unnecessary menus like
        // Debug As, Run As, Team menus etc.)
        getEditorSite().registerContextMenu(ID, provider, viewer, false);
        viewer.setKeyHandler(new GraphicalViewerKeyHandler(viewer).setParent(getCommonKeyHandler()));

        // generic system tooltip support for error markers
        FigureUtils.addTooltipSupport(getFigureCanvas(), root.getFigure());

        // add hover tooltip support for help and documentation tooltips
        HoverSupport hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(600, 200);
        hoverSupport.adapt(getFigureCanvas(), new IHoverInfoProvider() {
            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
                GraphicalEditPart epUnderMouse = (GraphicalEditPart)getGraphicalViewer().findObjectAt(new Point(x,y));
                // check if the figure has its own tooltip in this case we do not provide our own information provider
                IFigure figureUnderMouse = epUnderMouse.getFigure().findFigureAt(x, y);
                // we do not show hover if a tooltip already set on the figure
                if (figureUnderMouse != null && figureUnderMouse.getToolTip() != null)
                    return null;
                // or we are inside a compound module figure (we display the hover only over the border a title for
                // a compound module)
                if (epUnderMouse instanceof CompoundModuleEditPart &&
                        !((CompoundModuleEditPart)epUnderMouse).getFigure().isOnBorder(x, y))
                    return null;

                return new HtmlHoverInfo(getHTMLHoverTextFor(epUnderMouse));
            }
        });

        loadProperties();

        // create some actions here because they need an installed viewer
        IAction action = new ToggleSnapToGeometryAction(viewer);
        getActionRegistry().registerAction(action);

        // register global actions to the keybinding service otherwise the CTRL-Z CTRL-Y and DEL
        // will be captured by the text editor after switching there
        ActionRegistry registry = getActionRegistry();
        String id = ActionFactory.UNDO.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));
        id = ActionFactory.REDO.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));
        id = ActionFactory.DELETE.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));

        id = ActionFactory.COPY.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));
        id = ActionFactory.CUT.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));
        id = ActionFactory.PASTE.getId();
        getEditorSite().getKeyBindingService().registerAction(registry.getAction(id));
    }

    @Override
    public DefaultEditDomain getEditDomain() {
        // overridden to make it visible
        return super.getEditDomain();
    }

    @Override
    public ActionRegistry getActionRegistry() {
        // overridden to make it visible
        return super.getActionRegistry();
    }

    @Override
    public void doSave(final IProgressMonitor progressMonitor) {
        Assert.isTrue(false, "save is not implemented");
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.parts.GraphicalEditorWithFlyoutPalette#getAdapter(java.lang.Class)
     * Adds content outline
     */
    @SuppressWarnings("unchecked")
    @Override
    public Object getAdapter(Class type) {
        if (type == IPropertySheetPage.class) {
            if (propertySheetPage == null )
                propertySheetPage = new NedPropertySheetPage(this);
            return propertySheetPage;
        }

        if (type == IContentOutlinePage.class) {
            if (outlinePage == null)
                outlinePage = new NedOutlinePage(this, new TreeViewer());
            return outlinePage;
        }
        return super.getAdapter(type);
    }

    /**
     * Returns the KeyHandler with common bindings for both the Outline and
     * Graphical Views. For example, delete is a common action.
     */
    protected KeyHandler getCommonKeyHandler() {
        if (sharedKeyHandler == null) {
            sharedKeyHandler = new KeyHandler();
            sharedKeyHandler.put(KeyStroke.getPressed(SWT.F6, 0), getActionRegistry().getAction(GEFActionConstants.DIRECT_EDIT));
            sharedKeyHandler.put(KeyStroke.getPressed(SWT.F3, 0), getActionRegistry().getAction(OpenTypeAction.ID));
            sharedKeyHandler.put(KeyStroke.getPressed(SWT.CR, SWT.CR, SWT.CTRL), getActionRegistry().getAction(PropertiesAction.ID));
            sharedKeyHandler.put(KeyStroke.getPressed('+', SWT.KEYPAD_ADD, SWT.CTRL), getActionRegistry().getAction(ZoomInAction.ID));
            sharedKeyHandler.put(KeyStroke.getPressed('-', SWT.KEYPAD_SUBTRACT, SWT.CTRL), getActionRegistry().getAction(ZoomOutAction.ID));
            sharedKeyHandler.put(KeyStroke.getPressed((char)('I'-64), 'i', SWT.CTRL), getActionRegistry().getAction(ScaleUpIconsAction.ID));
            sharedKeyHandler.put(KeyStroke.getPressed((char)('O'-64), 'o', SWT.CTRL), getActionRegistry().getAction(ScaleDownIconsAction.ID));
        }
        return sharedKeyHandler;
    }


    /* (non-Javadoc)
     * @see org.eclipse.gef.ui.parts.GraphicalEditor#createActions()
     * Register the used actions
     */
    @SuppressWarnings("unchecked")
    @Override
    protected void createActions() {
        super.createActions();
        ActionRegistry registry = getActionRegistry();
        IAction action;

        // KLUDGE: Do not call getPrinterList() from PrintAction.calculateEnbabled()
        // because it causes exception. PrintAction is already registered in super class.
        // See:
        // http://dev.omnetpp.org/bugs/view.php?id=337
        // https://bugs.eclipse.org/bugs/show_bug.cgi?id=330165
        IAction origPrintAction = registry.getAction(ActionFactory.PRINT.getId());
        registry.removeAction(origPrintAction);
        IAction printAction = new PrintAction(this) {
            @Override
            protected boolean calculateEnabled() {
                return true;
            }
        };
        registry.registerAction(printAction);

        action = new NedSelectAllAction(this);
        registry.registerAction(action);

        action = new ExportImageAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new OpenTypeAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new PropertiesAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ParametersDialogAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new NedDirectEditAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new TogglePinAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());
        // depends on stack state change too. We should reflect the pinned state of a module on the status bar too
        getStackActions().add(action.getId());

        action = new RelayoutAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ZoomInAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ZoomOutAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ScaleUpIconsAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new ScaleDownIconsAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new CopyAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new CutAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new PasteAction(this);
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

        action = new MatchWidthAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

        action = new MatchHeightAction(this);
        registry.registerAction(action);
        getSelectionActions().add(action.getId());

    }

    public FigureCanvas getFigureCanvas() {
        return (FigureCanvas) getGraphicalViewer().getControl();
    }

    protected IFile getFile() {
        Assert.isTrue(getEditorInput() != null);
        return ((FileEditorInput)getEditorInput()).getFile();
    }

    public NedFileElementEx getModel() {
        return NedResourcesPlugin.getNedResources().getNedFileElement(getFile());
    }

    protected void loadProperties() {
        // Scroll-wheel Zoom support
//TODO        getGraphicalViewer().setProperty(MouseWheelHandler.KeyGenerator.getKey(SWT.MOD1), MouseWheelZoomHandler.SINGLETON);
    }

    @Override
    public void setInput(IEditorInput input) {
        Assert.isNotNull(input, "Graphical editor cannot display NULL input");
        // no change -- skip
        if (ObjectUtils.equals(input, getEditorInput()))
            return;

        super.setInput(input);

        Assert.isTrue(input instanceof IFileEditorInput, "Input of Graphical NED editor must be an IFileEditorInput");
        Assert.isTrue(NedResourcesPlugin.getNedResources().getConnectCount(getFile())>0);  // must be already connected

        getCommandStack().flush();
        NedFileElementEx model = getModel();

        if (!editorSaving) {
            if (getGraphicalViewer() != null) {
                getGraphicalViewer().setContents(model);
                loadProperties();
            }
            if (outlinePage != null) {
                outlinePage.setContents(getModel());
            }
        }
        paletteManager.refresh();
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
              getSite() instanceof MultiPageEditorSite &&
              ((MultiPageEditorSite)getSite()).getMultiPageEditor() == activeEditor)
            updateActions(getSelectionActions());
    }

    public void modelChanged(final NedModelEvent event) {
        // multi page editor will close us -> no need to do anything
        if (event instanceof NedFileRemovedEvent && ((NedFileRemovedEvent)event).getFile().equals(getFile()))
            return;

        // ignore event when closing the editor and the file has already been removed from the NEDResources
        if (!NedResourcesPlugin.getNedResources().containsNedFileElement(getFile()))
            return;

        // we do a full refresh in response of a change
        // if we are in a background thread, refresh later when UI thread is active
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            public void run() {
                // count begin/end nesting
                if (event instanceof NedBeginModelChangeEvent)
                    nedBeginChangeCount++;
                else if (event instanceof NedEndModelChangeEvent)
                    nedBeginChangeCount--;
                // Debug.println(event.toString() + ",  beginCount=" + nedBeginChangeCount);
                Assert.isTrue(nedBeginChangeCount >= 0, "begin/end mismatch");

                // record notification event as an external change iff it refers to our model and we are not the originator
                if (event instanceof NedModelChangeEvent && event.getSource() != null &&
                    event.getSource().getContainingNedFileElement() == getModel() &&
                    !((LocalCommandStack)getCommandStack()).isInsideBeginEnd())
                {
                    if (pendingExternalChangeCommand == null)
                        pendingExternalChangeCommand = new ExternalChangeCommand();
//                  Debug.println("adding " + event + " to current external change command");
                    pendingExternalChangeCommand.addEvent(event);
                }

                // "execute" (==nop) external change command after "end" (or if came without begin/end)
                if (nedBeginChangeCount == 0 && pendingExternalChangeCommand != null) {
                    ExternalChangeCommand tmp = pendingExternalChangeCommand;
                    pendingExternalChangeCommand = null;
                    //Debug.println("executing external change command");
                    getCommandStack().execute(tmp);
                    //Debug.println("done executing external change command");
                }

                // adjust connections after submodule name change, etc
                // If a submodule name has changed, we must change all the connections in the same compound module
                // that is attached to this module, so the model will remain consistent.
                //
                // NOTE: corresponding code used to be in SubmoduleElementEx.setName(), but that's
                // not the right place. This is more of a refactoring, e.g. ideally we'd have to
                // update all derived compound modules too, possibly after asking the user for
                // confirmation -- which is more easily done here.
                //
                if (event instanceof NedAttributeChangeEvent && event.getSource() instanceof SubmoduleElementEx) {
                    NedAttributeChangeEvent e = (NedAttributeChangeEvent) event;
                    if (e.getAttribute().equals(SubmoduleElement.ATT_NAME)) {
                        CompoundModuleElementEx compoundModule = ((SubmoduleElementEx)e.getSource()).getCompoundModule();
                        NedElementUtilEx.renameSubmoduleInConnections(compoundModule, (String)e.getOldValue(), (String)e.getNewValue());
                    }
                }

                // optimize refresh(): skip those between begin/end notifications
                if (nedBeginChangeCount == 0 && getGraphicalControl() != null && getGraphicalControl().isVisible())
                    refresh();
            }
        });
    }

    // refresh the whole content plus the outline page and the palette too
    public void refresh() {
        long startTime = System.currentTimeMillis();
        getGraphicalViewer().getContents().refresh();
        Debug.println("Graphical Editor refresh: " + (System.currentTimeMillis()-startTime) + "ms");

        paletteRefreshJob.restartTimer();
    }

    /**
     * Reveals a model element in the editor (or its nearest ancestor which
     * has an associated editPart)
     */
    public void reveal(INedElement model) {
        EditPart editPart = getNearestEditPartForModel(getGraphicalViewer(), model);

         if (editPart == null) {
             getGraphicalViewer().deselectAll();
         }
         else {
             getGraphicalViewer().reveal(editPart);
             getGraphicalViewer().select(editPart);
         }
    }

    /**
     * Tries to get the EditPart associated with the given model element. If not found
     * returns the first one among ancestors that has a corresponding editpart.
     * Can return NULL no editpart has been found
     */
    public static EditPart getNearestEditPartForModel(EditPartViewer viewer, INedElement model) {
            INedElement originalModel = model;
            while (model != null) {
                // get the part directly from the main registry
                EditPart ep = (EditPart)viewer.getEditPartRegistry().get(model);
                // if we reached the compound module level, try to look in the registered connections too
                // maybe the originalModel was one of our connections. This is needed because
                // ConnectionEditParts are not registered in the main registry rather in the compoundmodule
                // editparts local registry
                if (ep instanceof CompoundModuleEditPart) {
                    ConnectionEditPart connEP = ((CompoundModuleEditPart)ep).getModelToConnectionPartsRegistry().get(originalModel);
                    if (connEP != null)
                        return connEP;
                }
                // if it was not a connection return that editpart
                if (ep != null)
                    return ep;
                // not matching, check the parent too
                model = model.getParent();
            }
        return null;
    }

    /**
     * Marks the current editor content state, so we will be able to detect any change in the editor
     * used for editor change optimization
     */
    public void markContent() {   //TODO: still needed?
        lastUndoCommand = getCommandStack().getUndoCommand();
    }

    /**
     * Returns whether the content of the editor has changed since the last markContent call.
     */
    public boolean hasContentChanged() {  //TODO: still needed?
        return !(lastUndoCommand == getCommandStack().getUndoCommand());
    }

    public void markSaved() {
        getCommandStack().markSaveLocation();
    }

    protected String getHTMLHoverTextFor(EditPart ep) {
        if (!(ep instanceof INedModelProvider))
            return null;

        INedElement element = ((INedModelProvider)ep).getModel();
        if (element instanceof NedFileElementEx)
            return null;

        String hoverText = "";

        // brief
        hoverText += "<b>" + StringUtils.quoteForHtml(NedModelLabelProvider.getInstance().getText(element)) + "</b>\n";

        //debug code:
        //hoverText += element.getSourceLocation() + "<br/>" + element.getSourceRegion();
        //DisplayString ds = ((IHasDisplayString)element).getDisplayString();
        //String displayStringOwnerSource = "<pre>" + ds.getOwner().getSource() + "</pre>";
        //String xml = "<pre>" + StringUtils.quoteForHtml(NedTreeUtil.generateXmlFromPojoElementTree(element, "", false)) + "</pre>";
        //hoverText += displayStringOwnerSource;
        //hoverText += xml;

        // comment
        String comment = StringUtils.trimToEmpty(element.getComment());

        // comment from the submodule's or connection channel's type
        String typeComment = "";
        if (element instanceof ISubmoduleOrConnection) {
            INedTypeElement typeElement = ((ISubmoduleOrConnection)element).getEffectiveTypeRef();
            if (typeElement != null)
                typeComment = StringUtils.trimToEmpty(typeElement.getComment());
        }

        String htmlComment = "";
        if (!StringUtils.isEmpty(comment)) {
            boolean tildeMode = comment.matches(".*(~[a-zA-Z_]).*");
            htmlComment += NedCommentFormatter.makeHtmlDocu(comment, false, tildeMode, null);
        }

        if (!StringUtils.isEmpty(typeComment)) {
            //typeComment = "<i>" + typeElement.getName() + " documentation:</i><br/>\n" + typeComment;
            boolean tildeMode = typeComment.matches(".*(~[a-zA-Z_]).*");
            htmlComment += NedCommentFormatter.makeHtmlDocu(typeComment, false, tildeMode, null);
        }

        hoverText += StringUtils.isBlank(htmlComment) ? "<br><br>" : htmlComment; // if there's not comment that contains <p>, we need linefeed between title and source

        //debug code:
        //hoverText += "<br/><br/>" + "SyntaxProblemMaxCumulatedSeverity:" + element.getSyntaxProblemMaxCumulatedSeverity() +
        //          ", ConsistencyProblemMaxCumulatedSeverity:" + element.getConsistencyProblemMaxCumulatedSeverity();
        //INedElement fileElement = element.getParentWithTag(NedElementTags.NED_NED_FILE);
        //hoverText += "<br/><br/>" + "File: SyntaxProblemMaxCumulatedSeverity:" + fileElement.getSyntaxProblemMaxCumulatedSeverity() +
        //", ConsistencyProblemMaxCumulatedSeverity:" + fileElement.getConsistencyProblemMaxCumulatedSeverity();

        String nedCode = StringUtils.stripLeadingCommentLines(element.getNedSource().trim(), "//");
        hoverText += "<i>Source:</i><pre>" + StringUtils.quoteForHtml(StringUtils.abbreviate(nedCode, 1000)) + "</pre>";

        return HoverSupport.addHTMLStyleSheet(hoverText);
    }

    /**
     * For debugging
     */
    public static void dumpEditPartHierarchy(EditPart editPart, String indent) {
        Debug.println(indent + editPart.toString());
        for (Object child : editPart.getChildren())
            dumpEditPartHierarchy((EditPart)child, indent+"  ");
    }

    /**
     * This class provides the submodule type filtering mechanism within the palette.
     *
     * @author levy
     */
    private final class LocalDrawerEditPart extends DrawerEditPart {
        private final class LocalDrawerFigure extends DrawerFigure {
            private final int ICON_WIDTH = 16;
            private final int ICON_SPACING = 2;
            private String oldText;
            private Text text;
            // NOTE: this figure is not in the figure hierarchy due to difficulties with the superclass and its layout managers
            private IFigure filterFigure;

            private LocalDrawerFigure(Composite control) {
                super(control);

                text = new Text(control, SWT.BORDER | SWT.SINGLE);
                text.setVisible(false);
                text.setToolTipText("Filter submodules (regular expression)");
                text.addModifyListener(new ModifyListener() {
                    public void modifyText(ModifyEvent e) {
                        paletteManager.setSubmodulesFilter(text.getText());
                        paletteRefreshJob.restartTimer();
                    }
                });
                text.addFocusListener(new org.eclipse.swt.events.FocusAdapter() {
                    @Override
                    public void focusLost(org.eclipse.swt.events.FocusEvent e) {
                        text.setVisible(false);
                    }
                });
                text.addKeyListener(new KeyAdapter() {
                    @Override
                    public void keyPressed(KeyEvent e) {
                        if (e.keyCode == '\r')
                            text.setVisible(false);
                        else if (e.keyCode == SWT.ESC) {
                            text.setVisible(false);
                            text.setText(oldText);
                        }
                    }
                });

                filterFigure = new ImageFigure(ImageFactory.global().getImage("_internal/toolbar/filter"));
                filterFigure.setToolTip(new Label("Hello"));
                filterFigure.addMouseListener(new MouseListener() {
                    public void mouseDoubleClicked(MouseEvent me) {
                    }

                    public void mousePressed(MouseEvent me) {
                        toggleEditFilter();
                    }

                    public void mouseReleased(MouseEvent me) {
                    }
                });
            }

            @Override
            public IFigure getToolTip() {
                return createToolTip();
            }

            @Override
            public IFigure findMouseEventTargetAt(int x, int y) {
                // NOTE: work around mouse handling for the filter figure
                if (filterFigure.containsPoint(x, y))
                    return filterFigure;
                else
                    return super.findMouseEventTargetAt(x, y);
            }

            @Override
            public void paint(Graphics graphics) {
                // NOTE: this way it follows resizing
                updateTextBounds();
                updateFilterFigureBounds();
                super.paint(graphics);
                filterFigure.paint(graphics);
            }

            private void toggleEditFilter() {
                text.setVisible(!text.getVisible());

                if (text.getVisible())
                    Display.getDefault().asyncExec(new Runnable() {
                        public void run() {
                            text.setSelection(0, text.getText().length());
                            text.setFocus();
                            oldText = text.getText();
                        }
                    });
            }

            private void updateTextBounds() {
                Dimension d = getCollapseToggle().getSize();
                text.setSize(d.width - 3 * (ICON_WIDTH + ICON_SPACING), d.height - 2 * ICON_SPACING + 2);
                Point location = getBounds().getLocation();
                translateToAbsolute(location);
                text.setLocation(location.x + ICON_WIDTH + ICON_SPACING * 2, location.y + ICON_SPACING);

                // resize on MAC because the size of the field was too small
                if (Platform.OS_MACOSX.equals(Platform.getOS())) {
                    int bd = text.getBorderWidth();
                    text.setBounds(text.getLocation().x-bd, text.getLocation().y-5-bd, text.getSize().x+2*bd, text.getSize().y+2*bd+8);
                }

            }

            private void updateFilterFigureBounds() {
                Rectangle area = getCollapseToggle().getClientArea();
                filterFigure.setBounds(new Rectangle(area.x + area.width - ICON_WIDTH * 2, area.y, ICON_WIDTH, area.height));
            }
        }

        private LocalDrawerEditPart(PaletteDrawer drawer) {
            super(drawer);
        }

        @Override
        public IFigure createFigure() {
            DrawerFigure figure = new LocalDrawerFigure((Composite)getViewer().getControl());

            // the following is copied over from the superclass'es constructor
            figure.setExpanded(getDrawer().isInitiallyOpen());
            figure.setPinned(getDrawer().isInitiallyPinned());

            figure.getCollapseToggle().addFocusListener(new FocusListener.Stub() {
                @Override
                public void focusGained(FocusEvent fe) {
                    getViewer().select(LocalDrawerEditPart.this);
                }
            });

            figure.getScrollpane().getContents().addLayoutListener((PaletteAnimator)getViewer().getEditPartRegistry().get(PaletteAnimator.class));

            return figure;
        }
    }

    private class LocalCommandStack extends CommandStack {
        boolean insideBeginEnd = false;

        public boolean isInsideBeginEnd() {
            return insideBeginEnd;
        }

        // override notifyListeners() methods, so we can surround command execution
        // with begin/end. "begin" must be fired after all PRE listeners,
        // and "end" must be fired before all normal commandStackListeners and
        // POST listeners. See super.execute() for why the code look like this.
        @Override
        protected void notifyListeners(Command command, int state) {
            if (state==POST_EXECUTE || state==POST_UNDO || state==POST_REDO) {
                NedResourcesPlugin.getNedResources().fireEndChangeEvent();
                Assert.isTrue(insideBeginEnd); // no nested PRE/POST notifications in GEF
                insideBeginEnd = false;
                notifyListeners();  // do postponed notifyListeners() now
            }

            super.notifyListeners(command, state);

            if (state==PRE_EXECUTE || state==PRE_UNDO || state==PRE_REDO) {
                Assert.isTrue(!insideBeginEnd, "Nested execute is not allowed");
                NedResourcesPlugin.getNedResources().fireBeginChangeEvent();
                insideBeginEnd = true;
            }
        }

        @Override
        @SuppressWarnings("deprecation")
        protected void notifyListeners() {
            if (!insideBeginEnd)
                super.notifyListeners(); // postpone to top of POST_EXECUTE
        }

        private boolean isModelEditable() {
            NedFileElementEx model = getModel();
            return !model.isReadOnly() && !model.hasSyntaxError();
        }

        // override execute(), canUndo() and canRedo() methods to make readonly models uneditable.

        @Override
        public void execute(Command command) {
            if (isModelEditable())
                super.execute(command);
        }

        @Override
        public boolean canUndo() {
            return super.canUndo() && isModelEditable();
        }

        @Override
        public boolean canRedo() {
            return super.canRedo() && isModelEditable();
        }
    }
}

