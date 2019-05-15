/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
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
import org.eclipse.ui.part.IPage;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.CopyAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.CopyToClipboardAction;
import org.omnetpp.scave.actions.CreateTempChartAction;
import org.omnetpp.scave.actions.CreateTempMatplotlibChartAction;
import org.omnetpp.scave.actions.CutAction;
import org.omnetpp.scave.actions.EditChartAction;
import org.omnetpp.scave.actions.EditInputFileAction;
import org.omnetpp.scave.actions.ExportChartsAction;
import org.omnetpp.scave.actions.ExportDataAction;
import org.omnetpp.scave.actions.ExportToSVGAction;
import org.omnetpp.scave.actions.GotoChartDefinitionAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.NewChartFromTemplateAction;
import org.omnetpp.scave.actions.OpenChartAction;
import org.omnetpp.scave.actions.PasteAction;
import org.omnetpp.scave.actions.RedoAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.RemoveAction;
import org.omnetpp.scave.actions.SaveTempChartAction;
import org.omnetpp.scave.actions.SelectAllAction;
import org.omnetpp.scave.actions.ShowOutputVectorViewAction;
import org.omnetpp.scave.actions.UndoAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.charting.IChartView;
import org.omnetpp.scave.charttemplates.ChartTemplate;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.python.KillPythonProcessAction;

public class ScaveEditorContributor extends MultiPageEditorActionBarContributor implements IPropertyListener, ISelectionChangedListener {

    private static ScaveEditorContributor instance;
    protected ISelectionProvider selectionProvider; // current selection provider

    protected IEditorPart activeEditor;


    ISelectionListener selectionListener = new ISelectionListener() {
        @Override
        public void selectionChanged(IWorkbenchPart part, ISelection selection) {
            updateActions();
        }
    };

    protected List<IScaveAction> actions = new ArrayList<>();

    protected CutAction cutAction;
    protected CopyAction copyAction;
    protected PasteAction pasteAction;
    protected UndoAction undoAction;
    protected RedoAction redoAction;


    // generic actions
    private OpenChartAction openAction;
    private EditChartAction editAction;
    private EditInputFileAction editInputFileAction;
    private RemoveAction removeAction; // action handler of deleteRetargetAction
    private SelectAllAction selectAllAction;
    private ExportChartsAction exportChartsAction;

    // ChartPage actions
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

    // BrowseDataPage actions
    private CopyToClipboardAction copyToClipboardAction;
    private ExportToSVGAction exportToSVGAction;
    private CreateTempChartAction createTempChartAction;
    private CreateTempMatplotlibChartAction createTempMatplotlibChartAction;
    private SaveTempChartAction saveTempChartAction;
    private ShowOutputVectorViewAction showOutputVectorViewAction;
    private Map<String,ExportDataAction> exportActions;

    // Python actions
    private KillPythonProcessAction killAction;

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

    IPropertyChangeListener zoomListener = new IPropertyChangeListener() {
        @Override
        public void propertyChange(PropertyChangeEvent event) {
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X)
                hzoomOutAction.updateEnabled();
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y)
                vzoomOutAction.updateEnabled();
        }
    };

    public static ScaveEditorContributor getDefault() {
        return instance;
    }

    /**
     * Creates a multi-page contributor.
     */
    public ScaveEditorContributor() {
        if (instance == null)
            instance = this;
    }

    @Override
    public void init(IActionBars bars, IWorkbenchPage page) {
        registerAction(page, openAction = new OpenChartAction());
        registerAction(page, editAction = new EditChartAction());
        registerAction(page, editInputFileAction = new EditInputFileAction());
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
        registerAction(page, createTempMatplotlibChartAction = new CreateTempMatplotlibChartAction());
        registerAction(page, saveTempChartAction = new SaveTempChartAction());
        registerAction(page, showOutputVectorViewAction = new ShowOutputVectorViewAction());

        registerAction(page, killAction = new KillPythonProcessAction());

        super.init(bars, page);

        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
    }

    // TODO:
    private IScaveAction registerAction(IWorkbenchPage page, final IScaveAction action) {
        actions.add(action);
        return action;
    }

    public OpenChartAction getOpenAction() {
        return openAction;
    }

    public EditChartAction getEditAction() {
        return editAction;
    }

    public EditInputFileAction getEditInputFileAction() {
        return editInputFileAction;
    }

    public UndoAction getUndoAction() {
        return undoAction;
    }

    public RedoAction getRedoAction() {
        return redoAction;
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

    public CreateTempMatplotlibChartAction getCreateTempMatplotlibChartAction() {
        return createTempMatplotlibChartAction;
    }

    public SaveTempChartAction getSaveTempChartAction() {
        return saveTempChartAction;
    }

    public ShowOutputVectorViewAction getShowOutputVectorViewAction() {
        return showOutputVectorViewAction;
    }

    public KillPythonProcessAction getKillAction() {
        return killAction;
    }

    /**
     * Listen on zoom state changes of the chart.
     */
    public void registerChart(final IChartView chartView) {
        chartView.addPropertyChangeListener(zoomListener);
    }

    @Override
    public void dispose() {
        super.dispose();
        instance = null;
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

        MenuManager submenuManager = new MenuManager("New From Template");

        for (ChartTemplate t : ChartTemplateRegistry.getAllTemplates())
            submenuManager.add(new NewChartFromTemplateAction(t));

        menuManager.insertBefore("edit", submenuManager);

        menuManager.insertBefore("edit", openAction);
        menuManager.insertBefore("edit", editAction);
        // menuManager.insertBefore("edit", new EditScriptAction());

        menuManager.insertAfter("additions-end", new Separator());
        menuManager.insertAfter("additions-end", createExportMenu());
        menuManager.insertAfter("additions-end", exportChartsAction);
    }

    public void shareGlobalActions(IPage page, IActionBars actionBars) {
        if (!(page instanceof IPropertySheetPage)) {
            actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
            actionBars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
            actionBars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);
            actionBars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);
            actionBars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);
        }
//        actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);
//        actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);
    }

    protected void addGlobalActions(IMenuManager menuManager) {
        menuManager.insertAfter("additions-end", new Separator("ui-actions"));
        menuManager.insertAfter("ui-actions", showPropertiesViewAction);
    }

    @Override
    public void setActivePage(IEditorPart part) {
        // nothing
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

            if (part instanceof ScaveEditor) {
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
            if (selectionProvider != null) { // this may also happen
                selectionProvider.addSelectionChangedListener(this);

                // Fake a selection changed event to update the menus.
                if (selectionProvider.getSelection() != null)
                    selectionChanged(new SelectionChangedEvent(selectionProvider, selectionProvider.getSelection()));
            }
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
        //((ScaveEditor)activeEditor).getCommandStack().removeListener(this);
        // TODO: remove all actions from workbench selection listener lists
    }

    public void activate() {
        activeEditor.addPropertyListener(this);

        // getA .getWorkbenchWindow().getSelectionService().addSelectionListener(selectionListener);
        //((ScaveEditor)activeEditor).getCommandStack().addListener(this);

     // TODO: add all actions to workbench selection listener lists
    }

    public void updateActions() {
        for (IScaveAction action : actions) {
            action.updateEnabled();
        }
    }
//    public void update() {
//        undoAction.update();
//        redoAction.update();
//    }

    public void propertyChanged(Object source, int id) {
        //update();
    }

}
