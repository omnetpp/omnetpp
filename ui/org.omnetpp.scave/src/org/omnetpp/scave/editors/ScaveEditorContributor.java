/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.emf.common.ui.viewer.IViewerProvider;
import org.eclipse.emf.edit.domain.IEditingDomainProvider;
import org.eclipse.emf.edit.ui.action.CopyAction;
import org.eclipse.emf.edit.ui.action.CutAction;
import org.eclipse.emf.edit.ui.action.PasteAction;
import org.eclipse.emf.edit.ui.action.RedoAction;
import org.eclipse.emf.edit.ui.action.UndoAction;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
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
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.part.IPage;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.views.properties.IPropertySheetPage;
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
import org.omnetpp.scave.actions.GotoChartSheetDefinitionAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.NewBarChartAction;
import org.omnetpp.scave.actions.NewChartSheetAction;
import org.omnetpp.scave.actions.NewDatasetAction;
import org.omnetpp.scave.actions.NewLineChartAction;
import org.omnetpp.scave.actions.NewScatterChartAction;
import org.omnetpp.scave.actions.OpenChartAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.actions.SaveTempChartAction;
import org.omnetpp.scave.actions.SaveTempChartSheetAction;
import org.omnetpp.scave.actions.SelectAllAction;
import org.omnetpp.scave.actions.ShowOutputVectorViewAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.charting.IChartView;
import org.omnetpp.scave.views.DatasetView;

/**
 * Editor contributor for ScaveEditor.
 * 
 * @author andras, tomi
 */
public class ScaveEditorContributor extends MultiPageEditorActionBarContributor implements IPropertyListener, ISelectionChangedListener {

    private static ScaveEditorContributor instance;
    protected ISelectionProvider selectionProvider; // current selection provider

    protected IEditorPart activeEditor;
    protected CutAction cutAction;
    protected CopyAction copyAction;
    protected PasteAction pasteAction;
    protected UndoAction undoAction;
    protected RedoAction redoAction;

    // global retarget actions
    private RetargetAction undoRetargetAction;
    private RetargetAction redoRetargetAction;
    private RetargetAction deleteRetargetAction;

    // generic actions
    private OpenChartAction openAction;
    private EditAction editAction;
    private RemoveAction removeAction; // action handler of deleteRetargetAction
    private SelectAllAction selectAllAction;
    private ExportChartsAction exportChartsAction;

    // ChartPage/ChartSheetPage actions
    private ZoomChartAction hzoomInAction;
    private ZoomChartAction hzoomOutAction;
    private ZoomChartAction vzoomInAction;
    private ZoomChartAction vzoomOutAction;
    private ZoomChartAction zoomToFitAction;
    private ChartMouseModeAction switchChartToPanModeAction;
    private ChartMouseModeAction switchChartToZoomModeAction;
    private CopyChartToClipboardAction copyChartToClipboardAction;
    private RefreshChartAction refreshChartAction;
    private GotoChartDefinitionAction gotoChartDefinitionAction;
    private GotoChartSheetDefinitionAction gotoChartSheetDefinitionAction;

    // BrowseDataPage actions
    private CopyToClipboardAction copyToClipboardAction;
    private ExportToSVGAction exportToSVGAction;
    private CreateTempChartAction createTempChartAction;
    private SaveTempChartAction saveTempChartAction;
    private SaveTempChartSheetAction saveTempChartSheetAction;
    private ShowOutputVectorViewAction showOutputVectorViewAction;
    private Map<String,ExportDataAction> exportActions;

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
            return activeEditor instanceof IViewerProvider;
        }

        @Override
        public void run() {
            if (activeEditor instanceof IViewerProvider) {
                Viewer viewer = ((IViewerProvider)activeEditor).getViewer();
                if (viewer != null) {
                    viewer.refresh();
                }
            }
        }
    };

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
     * Creates a multi-page contributor.
     */
    public ScaveEditorContributor() {
        if (instance==null)
            instance = this;
    }

    @Override
    public void init(IActionBars bars, IWorkbenchPage page) {
        registerAction(page, openAction = new OpenChartAction());
        registerAction(page, editAction = new EditAction());
        registerAction(page, selectAllAction = new SelectAllAction());
        registerAction(page, exportChartsAction = new ExportChartsAction());
        registerAction(page, removeAction = new RemoveAction());

        // ChartPage actions
        registerAction(page, hzoomInAction = new ZoomChartAction(true, false, 2.0));
        registerAction(page, hzoomOutAction = new ZoomChartAction(true, false, 1/2.0));
        registerAction(page, vzoomInAction = new ZoomChartAction(false, true, 2.0));
        registerAction(page, vzoomOutAction = new ZoomChartAction(false, true, 1/2.0));
        registerAction(page, zoomToFitAction = new ZoomChartAction(true, true, 0.0));
        registerAction(page, switchChartToPanModeAction = new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE));
        registerAction(page, switchChartToZoomModeAction = new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
        registerAction(page, copyChartToClipboardAction = new CopyChartToClipboardAction());
        registerAction(page, refreshChartAction = new RefreshChartAction());
        registerAction(page, gotoChartDefinitionAction = new GotoChartDefinitionAction());
        registerAction(page, gotoChartSheetDefinitionAction = new GotoChartSheetDefinitionAction());

        // BrowseDataPage actions
        exportActions = new HashMap<>();
        for (String format : ExportDataAction.FORMATS) {
            ExportDataAction action = new ExportDataAction(format);
            registerAction(page, action);
            exportActions.put(format, action);
        }
        registerAction(page, copyToClipboardAction = new CopyToClipboardAction());
        registerAction(page, exportToSVGAction = new ExportToSVGAction());
        registerAction(page, createTempChartAction = new CreateTempChartAction());
        registerAction(page, saveTempChartAction = new SaveTempChartAction());
        registerAction(page, saveTempChartSheetAction = new SaveTempChartSheetAction());
        registerAction(page, showOutputVectorViewAction = new ShowOutputVectorViewAction());

        super.init(bars, page);

        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
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
    }

    @Override
    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
        // nothing
    }

    public void populateContextMenu(IMenuManager menuManager) {
        menuManager.add(new Separator("edit"));

        // Add the edit menu actions.
        menuManager.add(new ActionContributionItem(undoAction));
        menuManager.add(new ActionContributionItem(redoAction));
        menuManager.add(new Separator());
        menuManager.add(new ActionContributionItem(cutAction));
        menuManager.add(new ActionContributionItem(copyAction));
        menuManager.add(new ActionContributionItem(pasteAction));
        menuManager.add(new Separator());
        menuManager.add(new ActionContributionItem(removeAction));
        menuManager.add(new Separator());

        menuManager.add(new Separator("additions"));
        menuManager.add(new Separator());

        // Add our other standard marker.
        menuManager.add(new Separator("additions-end"));

        addGlobalActions(menuManager);

        MenuManager submenuManager = new MenuManager("New");
        submenuManager.add(new NewBarChartAction(false));
        submenuManager.add(new NewLineChartAction(false));
        submenuManager.add(new NewScatterChartAction(false));
        submenuManager.add(new NewDatasetAction(false));
        submenuManager.add(new NewChartSheetAction(false));
        menuManager.insertBefore("edit", submenuManager);
        menuManager.insertBefore("edit", openAction);
        menuManager.insertBefore("edit", editAction);

        menuManager.insertAfter("additions-end", new Separator());
        menuManager.insertAfter("additions-end", createExportMenu());
        menuManager.insertAfter("additions-end", exportChartsAction);
    }

    public void shareGlobalActions(IPage page, IActionBars actionBars) {
        if (!(page instanceof IPropertySheetPage)) {
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
            actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);
            actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);
            actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);
        }
        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);
        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);

        // replace inherited deleteAction
        if (!(page instanceof IPropertySheetPage))
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
    }

    protected void addGlobalActions(IMenuManager menuManager) {
        menuManager.insertAfter("additions-end", new Separator("ui-actions"));
        menuManager.insertAfter("ui-actions", showPropertiesViewAction);
        menuManager.insertAfter("ui-actions", showDatasetViewAction);
        refreshViewerAction.setEnabled(refreshViewerAction.isEnabled());
        menuManager.insertAfter("ui-actions", refreshViewerAction);
    }

    @Override
    public void setActivePage(IEditorPart part) {
        // nothing
    }

    public OpenChartAction getOpenAction() {
        return openAction;
    }

    public EditAction getEditAction() {
        return editAction;
    }

    public RemoveAction getRemoveAction() {
        return removeAction;
    }

    public ZoomChartAction getHZoomInAction() {
        return hzoomInAction;
    }

    public ZoomChartAction getHZoomOutAction() {
        return hzoomOutAction;
    }

    public ZoomChartAction getVZoomInAction() {
        return vzoomInAction;
    }

    public ZoomChartAction getVZoomOutAction() {
        return vzoomOutAction;
    }

    public ZoomChartAction getZoomToFitAction() {
        return zoomToFitAction;
    }

    public ChartMouseModeAction getSwitchChartToPanModeAction() {
        return switchChartToPanModeAction;
    }

    public ChartMouseModeAction getSwitchChartToZoomModeAction() {
        return switchChartToZoomModeAction;
    }

    public RefreshChartAction getRefreshChartAction() {
        return refreshChartAction;
    }

    public GotoChartDefinitionAction getGotoChartDefinitionAction() {
        return gotoChartDefinitionAction;
    }

    public GotoChartSheetDefinitionAction getGotoChartSheetDefinitionAction() {
        return gotoChartSheetDefinitionAction;
    }

    public CopyChartToClipboardAction  getCopyChartToClipboardAction() {
        return copyChartToClipboardAction;
    }

    public CopyToClipboardAction getCopyToClipboardAction() {
        return copyToClipboardAction;
    }

    public ExportToSVGAction getExportToSVGAction() {
        return exportToSVGAction;
    }

    public Map<String, ExportDataAction> getExportActions() {
        return exportActions;
    }

    public CreateTempChartAction getCreateTempChartAction() {
        return createTempChartAction;
    }

    public SaveTempChartAction getSaveTempChartAction() {
        return saveTempChartAction;
    }

    public SaveTempChartSheetAction getSaveTempChartSheetAction() {
        return saveTempChartSheetAction;
    }
    public ShowOutputVectorViewAction getShowOutputVectorViewAction() {
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

        activeEditor = part;

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

    public void selectionChanged(SelectionChangedEvent event) {
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

        cutAction = new CutAction();
        cutAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_CUT));
        actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);

        copyAction = new CopyAction();
        copyAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_COPY));
        actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);

        pasteAction = new PasteAction();
        pasteAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_PASTE));
        actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);

        undoAction = new UndoAction();
        undoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_UNDO));
        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);

        redoAction = new RedoAction();
        redoAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_REDO));
        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);
    }

    public IEditorPart getActiveEditor() {
        return activeEditor;
    }

    public void deactivate() {
        activeEditor.removePropertyListener(this);

        cutAction.setActiveWorkbenchPart(null);
        copyAction.setActiveWorkbenchPart(null);
        pasteAction.setActiveWorkbenchPart(null);
        undoAction.setActiveWorkbenchPart(null);
        redoAction.setActiveWorkbenchPart(null);

        ISelectionProvider selectionProvider = activeEditor instanceof ISelectionProvider ? (ISelectionProvider) activeEditor
                : activeEditor.getEditorSite().getSelectionProvider();

        if (selectionProvider != null) {
            selectionProvider.removeSelectionChangedListener(cutAction);
            selectionProvider.removeSelectionChangedListener(copyAction);
            selectionProvider.removeSelectionChangedListener(pasteAction);
        }
    }

    public void activate() {
        activeEditor.addPropertyListener(this);

        cutAction.setActiveWorkbenchPart(activeEditor);
        copyAction.setActiveWorkbenchPart(activeEditor);
        pasteAction.setActiveWorkbenchPart(activeEditor);
        undoAction.setActiveWorkbenchPart(activeEditor);
        redoAction.setActiveWorkbenchPart(activeEditor);

        ISelectionProvider selectionProvider = activeEditor instanceof ISelectionProvider ? (ISelectionProvider) activeEditor
                : activeEditor.getEditorSite().getSelectionProvider();

        if (selectionProvider != null) {
            selectionProvider.addSelectionChangedListener(cutAction);
            selectionProvider.addSelectionChangedListener(copyAction);
            selectionProvider.addSelectionChangedListener(pasteAction);
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

            cutAction.updateSelection(structuredSelection);
            copyAction.updateSelection(structuredSelection);
            pasteAction.updateSelection(structuredSelection);
        }

        undoAction.update();
        redoAction.update();
    }

    public void propertyChanged(Object source, int id) {
        update();
    }

}
