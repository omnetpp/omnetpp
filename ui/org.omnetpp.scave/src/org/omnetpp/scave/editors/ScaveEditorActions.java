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
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.part.IPage;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.CopyAction;
import org.omnetpp.scave.actions.CopyChartImageToClipboardAction;
import org.omnetpp.scave.actions.CopyDataToClipboardAction;
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

public class ScaveEditorActions implements IPropertyListener, ISelectionChangedListener {

    protected ISelectionProvider selectionProvider; // current selection provider

    protected ScaveEditor editor;


    ISelectionListener selectionListener = new ISelectionListener() {
        @Override
        public void selectionChanged(IWorkbenchPart part, ISelection selection) {
            updateActions();
        }
    };

    protected List<IScaveAction> actions = new ArrayList<>();


    public final EditChartAction editAction = registerAction(new EditChartAction());
    public final EditInputFileAction editInputFileAction = registerAction(new EditInputFileAction());
    public final SelectAllAction selectAllAction = registerAction(new SelectAllAction());
    public final ExportChartsAction exportChartsAction = registerAction(new ExportChartsAction());
    public final RemoveAction removeAction = registerAction(new RemoveAction());

    // ChartPage actions
    public final ZoomChartAction hzoomInAction = registerAction(new ZoomChartAction(true, false, 2.0));
    public final ZoomChartAction hzoomOutAction = registerAction(new ZoomChartAction(true, false, 1/2.0));
    public final ZoomChartAction vzoomInAction = registerAction(new ZoomChartAction(false, true, 2.0));
    public final ZoomChartAction vzoomOutAction = registerAction(new ZoomChartAction(false, true, 1/2.0));
    public final ZoomChartAction zoomToFitAction = registerAction(new ZoomChartAction(true, true, 0.0));
    public final ChartMouseModeAction switchChartToPanModeAction = registerAction(new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE));
    public final ChartMouseModeAction switchChartToZoomModeAction = registerAction(new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
    public final CopyChartImageToClipboardAction copyChartToClipboardAction = registerAction(new CopyChartImageToClipboardAction());
    public final RefreshChartAction refreshChartAction = registerAction(new RefreshChartAction());
    public final GotoChartDefinitionAction gotoChartDefinitionAction = registerAction(new GotoChartDefinitionAction());


    public final CutAction cutAction = registerAction(new CutAction());
    public final CopyAction copyAction = registerAction(new CopyAction());
    public final PasteAction pasteAction = registerAction(new PasteAction());
    public final UndoAction undoAction = registerAction(new UndoAction());
    public final RedoAction redoAction = registerAction(new RedoAction());

    // generic actions
    public final OpenChartAction openAction = registerAction(new OpenChartAction());

    public final CopyDataToClipboardAction copyToClipboardAction = registerAction(new CopyDataToClipboardAction());
    public final ExportToSVGAction exportToSVGAction = registerAction(new ExportToSVGAction());
    public final CreateTempChartAction createTempChartAction = registerAction(new CreateTempChartAction());
    public final CreateTempMatplotlibChartAction createTempMatplotlibChartAction = registerAction(new CreateTempMatplotlibChartAction());
    public final SaveTempChartAction saveTempChartAction = registerAction(new SaveTempChartAction());
    public final ShowOutputVectorViewAction showOutputVectorViewAction = registerAction(new ShowOutputVectorViewAction());

    public final KillPythonProcessAction killAction = registerAction(new KillPythonProcessAction());


    // BrowseDataPage actions

    private Map<String,ExportDataAction> exportActions;


    protected IAction showPropertiesViewAction = new Action("Show Properties View") {
        @Override
        public void run() {
            try {
                editor.getEditorSite().getPage().showView("org.eclipse.ui.views.PropertySheet");
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
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X ||
                    event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y)
                zoomToFitAction.updateEnabled();
        }
    };

    public ScaveEditorActions(ScaveEditor editor) {
        this.editor = editor;

        // BrowseDataPage actions
        exportActions = new HashMap<>();
        for (String format : ExportDataAction.FORMATS) {
            ExportDataAction action = new ExportDataAction(format);
            registerAction(action);
            exportActions.put(format, action);
        }

        IActionBars bars = editor.getEditorSite().getActionBars();

        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), removeAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
        bars.setGlobalActionHandler(ActionFactory.CUT.getId(), cutAction);
        bars.setGlobalActionHandler(ActionFactory.COPY.getId(), copyAction);
        bars.setGlobalActionHandler(ActionFactory.PASTE.getId(), pasteAction);
        bars.setGlobalActionHandler(ActionFactory.UNDO.getId(), undoAction);
        bars.setGlobalActionHandler(ActionFactory.REDO.getId(), redoAction);
    }

    private <T extends IScaveAction> T registerAction(T action) {
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

    public CopyChartImageToClipboardAction  getCopyChartToClipboardAction() {
        return copyChartToClipboardAction;
    }

    public CopyDataToClipboardAction getCopyToClipboardAction() {
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

    public void selectionChanged(SelectionChangedEvent event) {
    }

    /**
     * This ensures that a delete action will clean up all references to deleted objects.
     */
    protected boolean removeAllReferencesOnDelete() {
        return true;
    }

    public IEditorPart getActiveEditor() {
        return editor;
    }

    public void deactivate() {
        editor.removePropertyListener(this);
        //((ScaveEditor)activeEditor).getCommandStack().removeListener(this);
        // TODO: remove all actions from workbench selection listener lists
    }

    public void activate() {
        editor.addPropertyListener(this);

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
