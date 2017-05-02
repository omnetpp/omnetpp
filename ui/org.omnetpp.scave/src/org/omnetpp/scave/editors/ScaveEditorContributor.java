/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.emf.common.ui.viewer.IViewerProvider;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.edit.domain.IEditingDomainProvider;
import org.eclipse.emf.edit.ui.action.ControlAction;
import org.eclipse.emf.edit.ui.action.CopyAction;
import org.eclipse.emf.edit.ui.action.CreateChildAction;
import org.eclipse.emf.edit.ui.action.CreateSiblingAction;
import org.eclipse.emf.edit.ui.action.CutAction;
import org.eclipse.emf.edit.ui.action.DeleteAction;
import org.eclipse.emf.edit.ui.action.LoadResourceAction;
import org.eclipse.emf.edit.ui.action.PasteAction;
import org.eclipse.emf.edit.ui.action.RedoAction;
import org.eclipse.emf.edit.ui.action.UndoAction;
import org.eclipse.emf.edit.ui.action.ValidateAction;
import org.eclipse.emf.edit.ui.provider.DiagnosticDecorator;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IContributionManager;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.SubContributionItem;
import org.eclipse.jface.action.SubToolBarManager;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPropertyListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.part.IPage;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheet;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.CopyToClipboardAction;
import org.omnetpp.scave.actions.CreateTempChartAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.ExportChartsAction;
import org.omnetpp.scave.actions.ExportDataAction;
import org.omnetpp.scave.actions.ExportToSVGAction;
import org.omnetpp.scave.actions.GotoChartDefinitionAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.OpenChartAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.actions.SelectAllAction;
import org.omnetpp.scave.actions.ShowOutputVectorViewAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.charting.IChartView;
import org.omnetpp.scave.editors.ui.ChartsPage;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;
import org.omnetpp.scave.views.DatasetView;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class ScaveEditorContributor extends MultiPageEditorActionBarContributor implements IMenuListener, IPropertyListener, ISelectionChangedListener {
    private static ScaveEditorContributor instance;

    protected IEditorPart activeEditorPart;
    protected ISelectionProvider selectionProvider; // current selection provider

    /**
     * This will contain one {@link org.eclipse.emf.edit.ui.action.CreateChildAction} corresponding to each descriptor
     * generated for the current selection by the item provider.
     */
    protected Collection<IAction> createChildActions;

    /**
     * This is the menu manager into which menu contribution items should be added for CreateChild actions.
     */
    protected IMenuManager createChildMenuManager;

    /**
     * This will contain one {@link org.eclipse.emf.edit.ui.action.CreateSiblingAction} corresponding to each descriptor
     * generated for the current selection by the item provider.
     */
    protected Collection<IAction> createSiblingActions;

    /**
     * This is the menu manager into which menu contribution items should be added for CreateSibling actions.
     */
    protected IMenuManager createSiblingMenuManager;

    /**
     * This keeps track of the current editor part.
     */
    protected IEditorPart activeEditor;

    /**
     * This is the action used to implement delete.
     */
    protected DeleteAction originalDeleteAction;  //TODO this arguably isn't used (ownDeleteAction is), verify it and remove this field!

    /**
     * This is the action used to implement cut.
     */
    protected CutAction cutAction;

    /**
     * This is the action used to implement copy.
     */
    protected CopyAction copyAction;

    /**
     * This is the action used to implement paste.
     */
    protected PasteAction pasteAction;

    /**
     * This is the action used to implement undo.
     */
    protected UndoAction undoAction;

    /**
     * This is the action used to implement redo.
     */
    protected RedoAction redoAction;

    /**
     * This is the action used to load a resource.
     */
    protected LoadResourceAction loadResourceAction;

    /**
     * This is the action used to control or uncontrol a contained object.
     */
    protected ControlAction controlAction;

    /**
     * This is the action used to perform validation.
     */
    protected ValidateAction validateAction;

    /**
     * This is the action used to perform validation.
     * 
     * @since 2.9
     */
    protected DiagnosticDecorator.LiveValidator.LiveValidationAction liveValidationAction;

    /**
     * This style bit indicates that the "additions" separator should come after
     * the "edit" separator.
     */
    public static final int ADDITIONS_LAST_STYLE = 0x1;

    /**
     * This is used to encode the style bits.
     */
    protected int style;

//  public IAction addResultFileAction;
//  public IAction addWildcardResultFileAction;
//  public IAction removeAction;
//  public IAction addToDatasetAction;
//  public IAction createDatasetAction;
//  public IAction createChartAction;

    // global retarget actions
    private RetargetAction undoRetargetAction;
    private RetargetAction redoRetargetAction;
    private RetargetAction deleteRetargetAction;

    // container of conditional toolbar actions (delete/open/edit)
    private SubToolBarManager optionalToolbarActions;

    // generic actions
    private IAction openAction;
    private IAction editAction;
    private IScaveAction ourDeleteAction; // action handler of deleteRetargetAction
    private IAction selectAllAction;
    private IAction exportChartsAction;

    // ChartPage/ChartSheetPage actions
    private IAction hzoomInAction;
    private IAction hzoomOutAction;
    private IAction vzoomInAction;
    private IAction vzoomOutAction;
    private IAction zoomToFitAction;
    private IAction switchChartToPanModeAction;
    private IAction switchChartToZoomModeAction;
    private IAction copyChartToClipboardAction;
    private IAction refreshChartAction;
    private IAction gotoChartDefinitionAction;

    // BrowseDataPage actions
    private IAction copyToClipboardAction;
    private IAction exportToSVGAction;
    private IAction createTempChartAction;
    private IAction showOutputVectorViewAction;
    private Map<String,IAction> exportActions;

    IPropertyChangeListener zoomListener = new IPropertyChangeListener() {
        @Override
        public void propertyChange(PropertyChangeEvent event) {
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X)
                ((ZoomChartAction)hzoomOutAction).updateEnabled();
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y)
                ((ZoomChartAction)vzoomOutAction).updateEnabled();
        }
    };

    /**
     * This action opens the Dataset view.
     */
    private IAction showDatasetViewAction = new Action("Show Dataset View") {
        @Override
        public void run() {
            try {
                getPage().showView(DatasetView.ID);
            }
            catch (PartInitException exception) {
                ScavePlugin.logError(exception);
            }
        }
    };

    protected IAction showPropertiesViewAction = new Action("Show Properties View") {
        @Override
        public void run() {
            try {
                getPage().showView("org.eclipse.ui.views.PropertySheet");
            }
            catch (PartInitException exception) {
                ScavePlugin.logError(exception);
            }
        }
    };

    protected IAction refreshViewerAction = new Action("Refresh") {
        @Override
        public boolean isEnabled() {
            return activeEditorPart instanceof IViewerProvider;
        }

        @Override
        public void run() {
            if (activeEditorPart instanceof IViewerProvider) {
                Viewer viewer = ((IViewerProvider)activeEditorPart).getViewer();
                if (viewer != null) {
                    viewer.refresh();
                }
            }
        }
    };

    /**
     * Creates a multi-page contributor.
     */
    public ScaveEditorContributor() {
        style = ADDITIONS_LAST_STYLE;
        loadResourceAction = new LoadResourceAction();
        validateAction = new ValidateAction();
        controlAction = new ControlAction();
        
        if (instance==null)
            instance = this;
    }

    @Override
    public void init(IActionBars bars, IWorkbenchPage page) {
        openAction = registerAction(page, new OpenChartAction());
        editAction = registerAction(page, new EditAction());
        selectAllAction = registerAction(page, new SelectAllAction());
        exportChartsAction = registerAction(page, new ExportChartsAction());

        // replacement of the inherited deleteAction
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        ourDeleteAction = registerAction(page, new RemoveAction());
        ourDeleteAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));

        // ChartPage actions
        hzoomInAction = registerAction(page, new ZoomChartAction(true, false, 2.0));
        hzoomOutAction = registerAction(page, new ZoomChartAction(true, false, 1/2.0));
        vzoomInAction = registerAction(page, new ZoomChartAction(false, true, 2.0));
        vzoomOutAction = registerAction(page, new ZoomChartAction(false, true, 1/2.0));
        zoomToFitAction = registerAction(page, new ZoomChartAction(true, true, 0.0));
        switchChartToPanModeAction = registerAction(page, new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE));
        switchChartToZoomModeAction = registerAction(page, new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
        copyChartToClipboardAction = registerAction(page, new CopyChartToClipboardAction());
        refreshChartAction = registerAction(page, new RefreshChartAction());
        gotoChartDefinitionAction = registerAction(page, new GotoChartDefinitionAction());

        // BrowseDataPage actions
        exportActions = new HashMap<String,IAction>();
        for (String format : ExportDataAction.FORMATS) {
            IAction action = registerAction(page, new ExportDataAction(format));
            exportActions.put(format, action);
        }
        copyToClipboardAction = registerAction(page, new CopyToClipboardAction());
        exportToSVGAction = registerAction(page, new ExportToSVGAction());
        createTempChartAction = registerAction(page, new CreateTempChartAction());
        showOutputVectorViewAction = registerAction(page, new ShowOutputVectorViewAction());

//      addResultFileAction = registerAction(page, new AddResultFileAction());
//      addWildcardResultFileAction = registerAction(page, new AddWildcardResultFileAction());
//      openAction = registerAction(page, new OpenAction());
//      editAction = registerAction(page, new EditAction());
//      removeAction = registerAction(page, new RemoveAction());
//      addToDatasetAction = registerAction(page, new AddToDatasetAction());
//      createDatasetAction = registerAction(page, new CreateDatasetAction());
//      createChartAction = registerAction(page, new CreateChartAction());
        super.init(bars, page);

        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), ourDeleteAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
    }

    private IScaveAction registerAction(IWorkbenchPage page, final IScaveAction action) {
        page.getWorkbenchWindow().getSelectionService().addSelectionListener(new ISelectionListener() {
            @Override
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                action.selectionChanged(selection);
            }
        });
        return action;
    }

    /**
     * Listen on zoom state changes of the chart.
     */
    public void registerChart(final IChartView chartView) {
        chartView.addPropertyChangeListener(zoomListener);
    }

    public static ScaveEditorContributor getDefault() {
        return instance;
    }

    @Override
    public void dispose() {
        super.dispose();
        // FIXME remove the selection listener from the other actions
        getPage().removePartListener(undoRetargetAction);
        getPage().removePartListener(redoRetargetAction);
        getPage().removePartListener(deleteRetargetAction);
        undoRetargetAction.dispose();
        redoRetargetAction.dispose();
        deleteRetargetAction.dispose();

        instance = null;
    }

    @Override
    public void contributeToMenu(IMenuManager manager) {
        // do not contribute to the menu bar
    }

    @Override
    public void contributeToToolBar(IToolBarManager manager) {
        // note: most actions are displayed on toolbars inside the editor area
        manager.add(new Separator("scavemodel-settings"));
        manager.add(new Separator("scavemodel-additions"));

        undoRetargetAction = new RetargetAction(ActionFactory.UNDO.getId(), "Undo");
        redoRetargetAction = new RetargetAction(ActionFactory.REDO.getId(), "Redo");
        deleteRetargetAction = new RetargetAction(ActionFactory.DELETE.getId(), "Delete");

        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        undoRetargetAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
        undoRetargetAction.setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO_DISABLED));
        redoRetargetAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO));
        redoRetargetAction.setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO_DISABLED));
        deleteRetargetAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));
        deleteRetargetAction.setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE_DISABLED));

        getPage().addPartListener(undoRetargetAction);
        getPage().addPartListener(redoRetargetAction);
        getPage().addPartListener(deleteRetargetAction);

        manager.add(undoRetargetAction);
        manager.add(redoRetargetAction);

        optionalToolbarActions = new SubToolBarManager(manager);
        optionalToolbarActions.add(deleteRetargetAction);
//        optionalToolbarActions.add(openAction);
//        optionalToolbarActions.add(editAction);
//
//        manager.insertBefore("scavemodel-additions", createTempChartAction);
//
//        manager.insertBefore("scavemodel-additions", switchChartToPanModeAction);
//        manager.insertBefore("scavemodel-additions", switchChartToZoomModeAction);
//        manager.insertBefore("scavemodel-additions", hzoomInAction);
//        manager.insertBefore("scavemodel-additions", hzoomOutAction);
//        manager.insertBefore("scavemodel-additions", vzoomInAction);
//        manager.insertBefore("scavemodel-additions", vzoomOutAction);
//        manager.insertBefore("scavemodel-additions", zoomToFitAction);
//        manager.insertBefore("scavemodel-additions", refreshChartAction);
    }

    @Override
    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
        // nothing
    }

    public void menuAboutToShow(IMenuManager menuManager) {
        // This is called for context menus of the model tree viewers
        // Add our standard marker.
        if ((style & ADDITIONS_LAST_STYLE) == 0)
            menuManager.add(new Separator("additions"));
        menuManager.add(new Separator("edit"));

        // Add the edit menu actions.
        menuManager.add(new ActionContributionItem(undoAction));
        menuManager.add(new ActionContributionItem(redoAction));
        menuManager.add(new Separator());
        menuManager.add(new ActionContributionItem(cutAction));
        menuManager.add(new ActionContributionItem(copyAction));
        menuManager.add(new ActionContributionItem(pasteAction));
        menuManager.add(new Separator());
        menuManager.add(new ActionContributionItem(ourDeleteAction));
        menuManager.add(new Separator());

        if ((style & ADDITIONS_LAST_STYLE) != 0) {
            menuManager.add(new Separator("additions"));
            menuManager.add(new Separator());
        }
        
        // Add our other standard marker.
        menuManager.add(new Separator("additions-end"));

        addGlobalActions(menuManager);

        MenuManager submenuManager = null;

        submenuManager = new MenuManager("Create Child");
        populateManager(submenuManager, createChildActions, null);
        menuManager.insertBefore("edit", submenuManager);

        submenuManager = new MenuManager("Create Sibling");
        populateManager(submenuManager, createSiblingActions, null);
        menuManager.insertBefore("edit", submenuManager);
      
        // replace the inherited deleteAction with ours, that handle references and temp obects well
        IContributionItem deleteActionItem = null;
        for (IContributionItem item : menuManager.getItems())
            if (item instanceof ActionContributionItem) {
                ActionContributionItem acItem = (ActionContributionItem)item;
                if (acItem.getAction() == originalDeleteAction) {
                    deleteActionItem = item;
                    break;
                }
            }
        if (deleteActionItem != null) {
            menuManager.remove(deleteActionItem);
            menuManager.insertBefore("additions-end", ourDeleteAction);
        }

        menuManager.insertBefore("edit", openAction);
        menuManager.insertBefore("edit", editAction);

        menuManager.insertAfter("additions-end", new Separator());
        menuManager.insertAfter("additions-end", createExportMenu());
        menuManager.insertAfter("additions-end", exportChartsAction);


    }

    public void shareGlobalActions(IPage page, IActionBars actionBars) {
        if (!(page instanceof IPropertySheetPage)) {
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), ourDeleteAction);
            actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);
            actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);
            actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);
        }
        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);
        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);

        // replace inherited deleteAction
        if (!(page instanceof IPropertySheetPage))
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), ourDeleteAction);
    }

    protected void addGlobalActions(IMenuManager menuManager) {
        menuManager.insertAfter("additions-end", new Separator("ui-actions"));
        menuManager.insertAfter("ui-actions", showPropertiesViewAction);
        menuManager.insertAfter("ui-actions", showDatasetViewAction);
        refreshViewerAction.setEnabled(refreshViewerAction.isEnabled());
        menuManager.insertAfter("ui-actions", refreshViewerAction);
    }

    protected void showOptionalToolbarActions(boolean visible) {
        if (optionalToolbarActions != null) {
            optionalToolbarActions.setVisible(visible);
            optionalToolbarActions.update(true);
        }
    }

    @Override
    public void setActivePage(IEditorPart part) {
        boolean visible = false;
        if (activeEditorPart instanceof ScaveEditor) {
            ScaveEditor scaveEditor = (ScaveEditor)activeEditorPart;
            ScaveEditorPage page = scaveEditor.getActiveEditorPage();
            visible = page instanceof ChartsPage;
        }
        showOptionalToolbarActions(visible); //TODO ???
    }

    public IAction getOpenAction() {
        return openAction;
    }

    public IAction getEditAction() {
        return editAction;
    }

    public IAction getDeleteAction() {
        return ourDeleteAction;
    }
    
    public IAction getHZoomInAction() {
        return hzoomInAction;
    }
    public IAction getHZoomOutAction() {
        return hzoomOutAction;
    }
    public IAction getVZoomInAction() {
        return vzoomInAction;
    }
    public IAction getVZoomOutAction() {
        return vzoomOutAction;
    }
    public IAction getZoomToFitAction() {
        return zoomToFitAction;
    }
    public IAction getSwitchChartToPanModeAction() {
        return switchChartToPanModeAction;
    }
    public IAction getSwitchChartToZoomModeAction() {
        return switchChartToZoomModeAction;
    }
    public IAction getRefreshChartAction() {
        return refreshChartAction;
    }
    public IAction getGotoChartDefinitionAction() {
        return gotoChartDefinitionAction;
    }
    public IAction getCopyChartToClipboardAction() {
        return copyChartToClipboardAction;
    }
    public IAction getCopyToClipboardAction() {
        return copyToClipboardAction;
    }
    public IAction getExportToSVGAction() {
        return exportToSVGAction;
    }
    public Map<String, IAction> getExportActions() {
        return exportActions;
    }
    public IAction getCreateTempChartAction() {
        return createTempChartAction;
    }
    public IAction getShowOutputVectorViewAction() {
        return showOutputVectorViewAction;
    }
    public RetargetAction getUndoRetargetAction() {
        return undoRetargetAction;
    }
    public RetargetAction getRedoRetargetAction() {
        return redoRetargetAction;
    }
    public IMenuManager createExportMenu() {
        return createExportMenu(new MenuManager("Export Data"));
    }
    public IMenuManager createExportMenu(IMenuManager exportMenu) {
        if (exportActions != null) {
            for (String format : ExportDataAction.FORMATS) {
                IAction action = exportActions.get(format);
                if (action != null)
                    exportMenu.add(action);
            }
        }
        return exportMenu;
    }
    

    /**
     * When the active editor changes, this remembers the change and registers with it as a selection provider.
     */
    @Override
    public void setActiveEditor(IEditorPart part) {
        super.setActiveEditor(part);

        if (part != activeEditor) {
            if (activeEditor != null)
                deactivate();

            if (part instanceof IEditingDomainProvider) {
                activeEditor = part;
                activate();
            }
        }

        activeEditorPart = part; //TODO merge activeEditor and activeEditorPart

        // Switch to the new selection provider.
        if (selectionProvider != null) {
            selectionProvider.removeSelectionChangedListener(this);
        }
        if (part == null) {
            selectionProvider = null;
        }
        else {
            selectionProvider = part.getSite().getSelectionProvider();
            selectionProvider.addSelectionChangedListener(this);

            // Fake a selection changed event to update the menus.
            if (selectionProvider.getSelection() != null)
                selectionChanged(new SelectionChangedEvent(selectionProvider, selectionProvider.getSelection()));
        }
    }

    /**
     * This implements {@link org.eclipse.jface.viewers.ISelectionChangedListener},
     * handling {@link org.eclipse.jface.viewers.SelectionChangedEvent}s by querying for the children and siblings
     * that can be added to the selected object and updating the menus accordingly.
     */
    public void selectionChanged(SelectionChangedEvent event) {
        // Remove any menu items for old selection.
        //
        if (createChildMenuManager != null)
            depopulateManager(createChildMenuManager, createChildActions);
        if (createSiblingMenuManager != null)
            depopulateManager(createSiblingMenuManager, createSiblingActions);

        // Query the new selection for appropriate new child/sibling descriptors
        //
        Collection<?> newChildDescriptors = null;
        Collection<?> newSiblingDescriptors = null;

        ISelection selection = event.getSelection();
        if (selection instanceof IStructuredSelection && ((IStructuredSelection)selection).size() == 1) {
            Object object = ((IStructuredSelection)selection).getFirstElement();

            EditingDomain domain = ((IEditingDomainProvider)activeEditorPart).getEditingDomain();

            newChildDescriptors = domain.getNewChildDescriptors(object, null);
            newSiblingDescriptors = domain.getNewChildDescriptors(null, object);
        }

        // Generate actions for selection; populate and redraw the menus.
        //
        createChildActions = generateCreateChildActions(newChildDescriptors, selection);
        createSiblingActions = generateCreateSiblingActions(newSiblingDescriptors, selection);

        if (createChildMenuManager != null) {
            populateManager(createChildMenuManager, createChildActions, null);
            createChildMenuManager.update(true);
        }
        if (createSiblingMenuManager != null) {
            populateManager(createSiblingMenuManager, createSiblingActions, null);
            createSiblingMenuManager.update(true);
        }
    }

    /**
     * This generates a {@link org.eclipse.emf.edit.ui.action.CreateChildAction} for each object in <code>descriptors</code>,
     * and returns the collection of these actions.
     */
    protected Collection<IAction> generateCreateChildActions(Collection<?> descriptors, ISelection selection) {
        Collection<IAction> actions = new ArrayList<IAction>();
        if (descriptors != null)
            for (Object descriptor : descriptors)
                actions.add(new CreateChildAction(activeEditorPart, selection, descriptor));
        return actions;
    }

    /**
     * This generates a {@link org.eclipse.emf.edit.ui.action.CreateSiblingAction} for each object in <code>descriptors</code>,
     * and returns the collection of these actions.
     */
    protected Collection<IAction> generateCreateSiblingActions(Collection<?> descriptors, ISelection selection) {
        Collection<IAction> actions = new ArrayList<IAction>();
        if (descriptors != null)
            for (Object descriptor : descriptors)
                actions.add(new CreateSiblingAction(activeEditorPart, selection, descriptor));
        return actions;
    }

    /**
     * This populates the specified <code>manager</code> with {@link org.eclipse.jface.action.ActionContributionItem}s
     * based on the {@link org.eclipse.jface.action.IAction}s contained in the <code>actions</code> collection,
     * by inserting them before the specified contribution item <code>contributionID</code>.
     */
    protected void populateManager(IContributionManager manager, Collection<? extends IAction> actions, String contributionID) {
        if (actions != null) {
            for (IAction action : actions) {
                if (contributionID != null)
                    manager.insertBefore(contributionID, action);
                else
                    manager.add(action);
            }
        }
    }
        
    /**
     * This removes from the specified <code>manager</code> all {@link org.eclipse.jface.action.ActionContributionItem}s
     * based on the {@link org.eclipse.jface.action.IAction}s contained in the <code>actions</code> collection.
     */
    protected void depopulateManager(IContributionManager manager, Collection<? extends IAction> actions) {
        if (actions != null) {
            IContributionItem[] items = manager.getItems();
            for (int i = 0; i < items.length; i++) {
                // Look into SubContributionItems
                IContributionItem contributionItem = items[i];
                while (contributionItem instanceof SubContributionItem)
                    contributionItem = ((SubContributionItem)contributionItem).getInnerItem();

                // Delete the ActionContributionItems with matching action.
                if (contributionItem instanceof ActionContributionItem) {
                    IAction action = ((ActionContributionItem)contributionItem).getAction();
                    if (actions.contains(action))
                        manager.remove(contributionItem);
                }
            }
        }
    }

    /**
     * This ensures that a delete action will clean up all references to deleted objects.
     */
    protected boolean removeAllReferencesOnDelete() {
        return true;
    }
    

    @Override
    public void init(IActionBars actionBars) {
        super.init(actionBars);
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();

        originalDeleteAction = createDeleteAction();
        originalDeleteAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));
        actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), originalDeleteAction);

        cutAction = createCutAction();
        cutAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_CUT));
        actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);

        copyAction = createCopyAction();
        copyAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_COPY));
        actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);

        pasteAction = createPasteAction();
        pasteAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_PASTE));
        actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);

        undoAction = createUndoAction();
        undoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);

        redoAction = createRedoAction();
        redoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO));
        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);
    }

    /**
     * Returns the action used to implement delete.
     * 
     * @see #originalDeleteAction
     * @since 2.6
     */
    protected DeleteAction createDeleteAction() {
        return new DeleteAction(removeAllReferencesOnDelete());
    }

    /**
     * Returns the action used to implement cut.
     * 
     * @see #cutAction
     * @since 2.6
     */
    protected CutAction createCutAction() {
        return new CutAction();
    }

    /**
     * Returns the action used to implement copy.
     * 
     * @see #copyAction
     * @since 2.6
     */
    protected CopyAction createCopyAction() {
        return new CopyAction();
    }

    /**
     * Returns the action used to implement paste.
     * 
     * @see #pasteAction
     * @since 2.6
     */
    protected PasteAction createPasteAction() {
        return new PasteAction();
    }

    /**
     * Returns the action used to implement undo.
     * 
     * @see #undoAction
     * @since 2.6
     */
    protected UndoAction createUndoAction() {
        return new UndoAction();
    }

    /**
     * Returns the action used to implement redo.
     * 
     * @see #redoAction
     * @since 2.6
     */
    protected RedoAction createRedoAction() {
        return new RedoAction();
    }

    /**
     * @deprecated
     */
    @Deprecated
    public void setActiveView(IViewPart part) {
        IActionBars actionBars = part.getViewSite().getActionBars();
        if (!(part instanceof PropertySheet)) {
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), originalDeleteAction);
            actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);
            actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);
            actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);
        }
        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);
        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);

        actionBars.updateActionBars();
    }

    public IEditorPart getActiveEditor() {
        return activeEditor;
    }

    public void deactivate() {
        activeEditor.removePropertyListener(this);

        originalDeleteAction.setActiveWorkbenchPart(null);
        cutAction.setActiveWorkbenchPart(null);
        copyAction.setActiveWorkbenchPart(null);
        pasteAction.setActiveWorkbenchPart(null);
        undoAction.setActiveWorkbenchPart(null);
        redoAction.setActiveWorkbenchPart(null);

        if (loadResourceAction != null)
            loadResourceAction.setActiveWorkbenchPart(null);

        if (controlAction != null)
            controlAction.setActiveWorkbenchPart(null);

        if (validateAction != null)
            validateAction.setActiveWorkbenchPart(null);

        if (liveValidationAction != null)
            liveValidationAction.setActiveWorkbenchPart(null);

        ISelectionProvider selectionProvider = activeEditor instanceof ISelectionProvider ? (ISelectionProvider) activeEditor
                : activeEditor.getEditorSite().getSelectionProvider();

        if (selectionProvider != null) {
            selectionProvider.removeSelectionChangedListener(originalDeleteAction);
            selectionProvider.removeSelectionChangedListener(cutAction);
            selectionProvider.removeSelectionChangedListener(copyAction);
            selectionProvider.removeSelectionChangedListener(pasteAction);

            if (validateAction != null)
                selectionProvider.removeSelectionChangedListener(validateAction);

            if (controlAction != null)
                selectionProvider.removeSelectionChangedListener(controlAction);
        }
    }

    public void activate() {
        activeEditor.addPropertyListener(this);

        originalDeleteAction.setActiveWorkbenchPart(activeEditor);
        cutAction.setActiveWorkbenchPart(activeEditor);
        copyAction.setActiveWorkbenchPart(activeEditor);
        pasteAction.setActiveWorkbenchPart(activeEditor);
        undoAction.setActiveWorkbenchPart(activeEditor);
        redoAction.setActiveWorkbenchPart(activeEditor);

        if (loadResourceAction != null)
            loadResourceAction.setActiveWorkbenchPart(activeEditor);

        if (controlAction != null)
            controlAction.setActiveWorkbenchPart(activeEditor);

        if (validateAction != null)
            validateAction.setActiveWorkbenchPart(activeEditor);

        if (liveValidationAction != null)
            liveValidationAction.setActiveWorkbenchPart(activeEditor);

        ISelectionProvider selectionProvider = activeEditor instanceof ISelectionProvider ? (ISelectionProvider) activeEditor
                : activeEditor.getEditorSite().getSelectionProvider();

        if (selectionProvider != null) {
            selectionProvider.addSelectionChangedListener(originalDeleteAction);
            selectionProvider.addSelectionChangedListener(cutAction);
            selectionProvider.addSelectionChangedListener(copyAction);
            selectionProvider.addSelectionChangedListener(pasteAction);

            if (validateAction != null)
                selectionProvider.addSelectionChangedListener(validateAction);

            if (controlAction != null)
                selectionProvider.addSelectionChangedListener(controlAction);
        }

        update();
    }

    public void update() {
        ISelectionProvider selectionProvider = activeEditor instanceof ISelectionProvider ? (ISelectionProvider) activeEditor
                : activeEditor.getEditorSite().getSelectionProvider();

        if (selectionProvider != null) {
            ISelection selection = selectionProvider.getSelection();
            IStructuredSelection structuredSelection = selection instanceof IStructuredSelection
                    ? (IStructuredSelection) selection : StructuredSelection.EMPTY;

            originalDeleteAction.updateSelection(structuredSelection);
            cutAction.updateSelection(structuredSelection);
            copyAction.updateSelection(structuredSelection);
            pasteAction.updateSelection(structuredSelection);

            if (validateAction != null)
                validateAction.updateSelection(structuredSelection);

            if (controlAction != null)
                controlAction.updateSelection(structuredSelection);
        }

        undoAction.update();
        redoAction.update();

        if (loadResourceAction != null)
            loadResourceAction.update();

        if (liveValidationAction != null)
            liveValidationAction.update();
    }

    public void propertyChanged(Object source, int id) {
        update();
    }
    
}
