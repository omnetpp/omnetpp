package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.scave.actions.AddFilterToDatasetAction;
import org.omnetpp.scave.actions.AddSelectedToDatasetAction;
import org.omnetpp.scave.actions.ChartMouseModeAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.CopyToClipboardAction;
import org.omnetpp.scave.actions.CreateChartTemplateAction;
import org.omnetpp.scave.actions.CreateTempChartAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.ExportDataAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.ShowVectorBrowserViewAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.model.presentation.ScaveModelActionBarContributor;

import static org.omnetpp.scave.actions.ExportDataAction.*;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class ScaveEditorContributor extends ScaveModelActionBarContributor {
	private static ScaveEditorContributor instance;
	
//	public IAction addResultFileAction;
//	public IAction addWildcardResultFileAction;
//	public IAction openAction;
//	public IAction editAction;
//	public IAction removeAction;
//	public IAction addToDatasetAction;
//	public IAction createDatasetAction;
//	public IAction createChartAction;
	
	// generic actions
	private IAction editAction;

	// ChartPage actions
//XXX
//	private IAction zoomInXAction;
//	private IAction zoomOutXAction;
//	private IAction zoomInYAction;
//	private IAction zoomOutYAction;
	private IAction zoomInAction;
	private IAction zoomOutAction;
	private IAction switchChartToPanModeAction;
	private IAction switchChartToZoomModeAction;
	private IAction copyChartToClipboardAction;
	private IAction refreshChartAction;
	private IAction createChartTemplateAction;

	// BrowseDataPage actions
	private IAction addFilterToDatasetAction;
	private IAction addSelectedToDatasetAction;
	private Map<String,IAction> exportDataActions = new HashMap<String,IAction>();
	private IAction copyToClipboardAction;
	private IAction createTempChartAction;
	private IAction showVectorBrowserViewAction;
	

	/**
	 * Creates a multi-page contributor.
	 */
	public ScaveEditorContributor() {
		super(false);
		if (instance==null) instance = this;
	}

	public void init(IActionBars bars, IWorkbenchPage page) {
        editAction = registerAction(page, new EditAction());

        // ChartPage actions
//XXX
//        zoomInXAction = registerAction(page, new ZoomChartAction(true, false, 1.5));
//        zoomOutXAction = registerAction(page, new ZoomChartAction(true, false, 1/1.5));
//        zoomInYAction = registerAction(page, new ZoomChartAction(false, true, 1.5));
//        zoomOutYAction = registerAction(page, new ZoomChartAction(false, true, 1/1.5));
        zoomInAction = registerAction(page, new ZoomChartAction(true, true, 2.0));
        zoomOutAction = registerAction(page, new ZoomChartAction(true, true, 1/2.0));
        switchChartToPanModeAction = registerAction(page, new ChartMouseModeAction(ZoomableCanvasMouseSupport.PAN_MODE));
        switchChartToZoomModeAction = registerAction(page, new ChartMouseModeAction(ZoomableCanvasMouseSupport.ZOOM_MODE));
        copyChartToClipboardAction = registerAction(page, new CopyChartToClipboardAction());
        refreshChartAction = registerAction(page, new RefreshChartAction());
        createChartTemplateAction = registerAction(page, new CreateChartTemplateAction());

    	// BrowseDataPage actions
        addFilterToDatasetAction = registerAction(page, new AddFilterToDatasetAction());
    	addSelectedToDatasetAction = registerAction(page, new AddSelectedToDatasetAction());
    	for (String format : ExportDataAction.FORMATS)
    		exportDataActions.put(format, registerAction(page, new ExportDataAction(format)));
    	copyToClipboardAction = registerAction(page, new CopyToClipboardAction());
    	createTempChartAction = registerAction(page, new CreateTempChartAction());
        showVectorBrowserViewAction = registerAction(page, new ShowVectorBrowserViewAction());
        
//      addResultFileAction = registerAction(page, new AddResultFileAction());
//      addWildcardResultFileAction = registerAction(page, new AddWildcardResultFileAction());
//      openAction = registerAction(page, new OpenAction());
//      editAction = registerAction(page, new EditAction());
//		removeAction = registerAction(page, new RemoveAction());
//		addToDatasetAction = registerAction(page, new AddToDatasetAction());
//		createDatasetAction = registerAction(page, new CreateDatasetAction());
//		createChartAction = registerAction(page, new CreateChartAction());
        super.init(bars, page);
	}

	private IScaveAction registerAction(IWorkbenchPage page, final IScaveAction action) {
		page.getWorkbenchWindow().getSelectionService().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				action.selectionChanged(selection);
			}
		});
		return action;
	}

	public static ScaveEditorContributor getDefault() {
		return instance;
	}
	
	public void dispose() {
		super.dispose();
    }

	public void contributeToMenu(IMenuManager manager) {
		// super.contributeToMenu(manager);
	}

	public void contributeToToolBar(IToolBarManager manager) {
		super.contributeToToolBar(manager);
//XXX
//		manager.insertBefore("scavemodel-additions", zoomInXAction);
//		manager.insertBefore("scavemodel-additions", zoomOutXAction);
//		manager.insertBefore("scavemodel-additions", zoomInYAction);
//		manager.insertBefore("scavemodel-additions", zoomOutYAction);
//		manager.insertBefore("scavemodel-additions", zoomInAction);
//		manager.insertBefore("scavemodel-additions", zoomOutAction);
		manager.insertBefore("scavemodel-additions", switchChartToPanModeAction);
		manager.insertBefore("scavemodel-additions", switchChartToZoomModeAction);
		manager.insertBefore("scavemodel-additions", zoomInAction);
		manager.insertBefore("scavemodel-additions", zoomOutAction);
		manager.insertBefore("scavemodel-additions", refreshChartAction);
		manager.insertBefore("scavemodel-additions", createChartTemplateAction);
	}

	public void contributeToStatusLine(IStatusLineManager statusLineManager) {
		super.contributeToStatusLine(statusLineManager);
	}

	@Override
	public void menuAboutToShow(IMenuManager menuManager) {
		super.menuAboutToShow(menuManager);
		menuManager.insertBefore("additions", editAction);
	}

	public IAction getEditAction() {
		return editAction;
	}
//XXX
//    public IAction getZoomInXAction() {
//		return zoomInXAction;
//	}
//    public IAction getZoomOutXAction() {
//		return zoomOutXAction;
//	}
//    public IAction getZoomInYAction() {
//		return zoomInYAction;
//	}
//    public IAction getZoomOutYAction() {
//		return zoomOutYAction;
//	}
    public IAction getZoomInAction() {
		return zoomInAction;
	}
    public IAction getZoomOutAction() {
		return zoomOutAction;
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
    public IAction getCreateChartTemplateAction() {
		return createChartTemplateAction;
	}
	public IAction getCopyChartToClipboardAction() {
		return copyChartToClipboardAction;
	}
	public IAction getAddFilterToDatasetAction() {
		return addFilterToDatasetAction;
	}
	public IAction getAddSelectedToDatasetAction() {
		return addSelectedToDatasetAction;
	}
	public IAction getExportDataAction(String format) {
		return exportDataActions.get(format);
	}
	public IAction getCopyToClipboardAction() {
		return copyToClipboardAction;
	}
	public IAction getCreateTempChartAction() {
		return createTempChartAction;
	}
	public IAction getShowVectorBrowserViewAction() {
		return showVectorBrowserViewAction;
	}
}
