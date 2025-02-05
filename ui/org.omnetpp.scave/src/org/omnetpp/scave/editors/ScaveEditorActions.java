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
import org.eclipse.ui.PartInitException;
import org.omnetpp.common.Debug;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.ConfigureChartAction;
import org.omnetpp.scave.actions.CopyImageToClipboardAction;
import org.omnetpp.scave.actions.CopyRowsToClipboardAction;
import org.omnetpp.scave.actions.ExportChartsAction;
import org.omnetpp.scave.actions.ExportDataAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.KillPythonProcessAction;
import org.omnetpp.scave.actions.OpenChartAction;
import org.omnetpp.scave.actions.PlotAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.RefreshResultFilesAction;
import org.omnetpp.scave.actions.ReloadResultFilesAction;
import org.omnetpp.scave.actions.SaveChartAsTemplateAction;
import org.omnetpp.scave.actions.SaveImageAction;
import org.omnetpp.scave.actions.ShowOutputVectorViewAction;
import org.omnetpp.scave.actions.analysismodel.CompareToTemplateAction;
import org.omnetpp.scave.actions.analysismodel.CopyAction;
import org.omnetpp.scave.actions.analysismodel.CutAction;
import org.omnetpp.scave.actions.analysismodel.EditInputFileAction;
import org.omnetpp.scave.actions.analysismodel.NewChartFromTemplateAction;
import org.omnetpp.scave.actions.analysismodel.NewChartFromTemplateGalleryAction;
import org.omnetpp.scave.actions.analysismodel.NewFolderAction;
import org.omnetpp.scave.actions.analysismodel.PasteAction;
import org.omnetpp.scave.actions.analysismodel.RedoAction;
import org.omnetpp.scave.actions.analysismodel.RemoveAction;
import org.omnetpp.scave.actions.analysismodel.RenameChartAction;
import org.omnetpp.scave.actions.analysismodel.ResetChartToTemplateAction;
import org.omnetpp.scave.actions.analysismodel.SaveTempChartAction;
import org.omnetpp.scave.actions.analysismodel.UndoAction;
import org.omnetpp.scave.actions.nativeplots.ChartMouseModeAction;
import org.omnetpp.scave.actions.nativeplots.RestoreOriginalViewAction;
import org.omnetpp.scave.actions.nativeplots.ZoomChartAction;
import org.omnetpp.scave.actions.nativeplots.ZoomToFitDataAction;
import org.omnetpp.scave.actions.ui.SelectAllAction;
import org.omnetpp.scave.actions.ui.SetItemWidthAction;
import org.omnetpp.scave.actions.ui.ShowFieldsAsScalarsAction;
import org.omnetpp.scave.actions.ui.ViewModeAction;
import org.omnetpp.scave.charting.IPlotViewer;
import org.omnetpp.scave.model.ChartTemplate;

public class ScaveEditorActions {

    protected ScaveEditor editor;

    protected List<IScaveAction> actions = new ArrayList<>();

    public final ConfigureChartAction editAction = registerAction(new ConfigureChartAction());
    public final EditInputFileAction editInputFileAction = registerAction(new EditInputFileAction());
    public final SelectAllAction selectAllAction = registerAction(new SelectAllAction());
    public final RenameChartAction renameChartAction = registerAction(new RenameChartAction());
    public final ExportChartsAction exportChartsAction = registerAction(new ExportChartsAction());
    public final NewChartFromTemplateGalleryAction newChartFromTemplateGalleryAction = registerAction(new NewChartFromTemplateGalleryAction());
    public final ResetChartToTemplateAction resetToTemplateAction = registerAction(new ResetChartToTemplateAction());
    public final CompareToTemplateAction compareToTemplateAction = registerAction(new CompareToTemplateAction());
    public final SaveChartAsTemplateAction saveChartAsTemplateAction = registerAction(new SaveChartAsTemplateAction());
    public final RemoveAction removeAction = registerAction(new RemoveAction());
    public final RefreshResultFilesAction refreshResultFilesAction = registerAction(new RefreshResultFilesAction());
    public final ReloadResultFilesAction reloadResultFilesAction = registerAction(new ReloadResultFilesAction());
    public final ShowFieldsAsScalarsAction showFieldsAsScalarsAction = registerAction(new ShowFieldsAsScalarsAction());

    public final ViewModeAction viewIconsAction = registerAction(new ViewModeAction(IconGridViewer.ViewMode.ICONS, "Icons", ScaveImages.IMG_ETOOL16_VIEW_ICONS));
    public final ViewModeAction viewListAction = registerAction(new ViewModeAction(IconGridViewer.ViewMode.LIST, "List", ScaveImages.IMG_ETOOL16_VIEW_LIST));
    public final ViewModeAction viewMulticolumnListAction = registerAction(new ViewModeAction(IconGridViewer.ViewMode.MULTICOLUMN_LIST, "Multicolumn", ScaveImages.IMG_ETOOL16_VIEW_MULTICOLUMN));
    public final SetItemWidthAction setItemWidthAction = registerAction(new SetItemWidthAction());

    // ChartPage actions
    public final ZoomChartAction hzoomInAction = registerAction(new ZoomChartAction(true, false, 2.0));
    public final ZoomChartAction hzoomOutAction = registerAction(new ZoomChartAction(true, false, 1/2.0));
    public final ZoomChartAction vzoomInAction = registerAction(new ZoomChartAction(false, true, 2.0));
    public final ZoomChartAction vzoomOutAction = registerAction(new ZoomChartAction(false, true, 1/2.0));
    public final RestoreOriginalViewAction restoreOriginalViewAction = registerAction(new RestoreOriginalViewAction());
    public final ZoomToFitDataAction zoomToFitDataAction = registerAction(new ZoomToFitDataAction());
    public final ChartMouseModeAction switchChartToPanModeAction = registerAction(new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE));
    public final ChartMouseModeAction switchChartToZoomModeAction = registerAction(new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
    public final CopyImageToClipboardAction copyChartToClipboardAction = registerAction(new CopyImageToClipboardAction());
    public final RefreshChartAction refreshChartAction = registerAction(new RefreshChartAction());

    public final CutAction cutAction = registerAction(new CutAction());
    public final CopyAction copyAction = registerAction(new CopyAction());
    public final PasteAction pasteAction = registerAction(new PasteAction());
    public final UndoAction undoAction = registerAction(new UndoAction());
    public final RedoAction redoAction = registerAction(new RedoAction());

    public final NewFolderAction newFolderAction = registerAction(new NewFolderAction());

    // generic actions
    public final OpenChartAction openAction = registerAction(new OpenChartAction());

    public final CopyRowsToClipboardAction copyRowsToClipboardAction = registerAction(new CopyRowsToClipboardAction());
    public final SaveImageAction saveImageAction = registerAction(new SaveImageAction());

    public final PlotAction plotAction = registerAction(new PlotAction());
    public final SaveTempChartAction saveTempChartAction = registerAction(new SaveTempChartAction());
    public final ShowOutputVectorViewAction showOutputVectorViewAction = registerAction(new ShowOutputVectorViewAction());

    public final KillPythonProcessAction killAction = registerAction(new KillPythonProcessAction());

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
                hzoomOutAction.update();
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y)
                vzoomOutAction.update();
            if (event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X ||
                    event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y)
                restoreOriginalViewAction.update();
                zoomToFitDataAction.update();
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

    }

    private <T extends IScaveAction> T registerAction(T action) {
        actions.add(action);
        return action;
    }

    public Map<String, ExportDataAction> getExportActions() {
        return exportActions;
    }
    /**
     * Listen on zoom state changes of the chart.
     */
    public void registerPlot(final IPlotViewer plotViewer) {
        plotViewer.addPropertyChangeListener(zoomListener);
    }

    public void populateContextMenu(IMenuManager menuManager, boolean showNewSubmenu) {
        if (showNewSubmenu) {
            MenuManager submenuManager = new MenuManager("New");
            for (ChartTemplate t : editor.getChartTemplateRegistry().getAllTemplates())
                submenuManager.add(new NewChartFromTemplateAction(t, false));
            menuManager.add(submenuManager);

            menuManager.add(newFolderAction);
        }
        menuManager.add(openAction);
        menuManager.add(editAction);
        menuManager.add(renameChartAction);

        menuManager.add(new Separator("edit")); // standard marker
        menuManager.add(new ActionContributionItem(undoAction));
        menuManager.add(new ActionContributionItem(redoAction));
        menuManager.add(new Separator());
        menuManager.add(new ActionContributionItem(cutAction));
        menuManager.add(new ActionContributionItem(copyAction));
        menuManager.add(new ActionContributionItem(pasteAction));
        menuManager.add(new ActionContributionItem(removeAction));

        menuManager.add(new Separator("export"));
        menuManager.add(exportChartsAction);
        menuManager.add(createExportDataMenu("Export Chart Input As"));

        menuManager.add(new Separator("templates"));
        menuManager.add(new ActionContributionItem(compareToTemplateAction));
        menuManager.add(new ActionContributionItem(resetToTemplateAction));
        menuManager.add(new ActionContributionItem(saveChartAsTemplateAction));
        menuManager.add(new Separator());

        menuManager.add(new Separator("additions")); // standard marker
        menuManager.add(new Separator("additions-end")); // standard marker
        menuManager.insertAfter("additions-end", new Separator("ui-actions"));

    }

    public IMenuManager createExportDataMenu(String label) {
        return createExportDataMenu(new MenuManager(label, ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EXPORT), null));
    }

    public IMenuManager createExportDataMenu(IMenuManager exportMenu) {
        if (exportActions != null) {
            for (String format : ExportDataAction.FORMATS) {
                IAction action = exportActions.get(format);
                if (action != null)
                    exportMenu.add(action);
            }
        }
        return exportMenu;
    }

    public void updateActions() {
        Debug.time("Updating actions", 10, () -> {
            for (IScaveAction action : actions)
                action.update();
            editor.getEditorSite().getActionBars().updateActionBars();
        });
    }
}
