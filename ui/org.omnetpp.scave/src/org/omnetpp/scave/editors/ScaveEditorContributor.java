package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
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
import org.omnetpp.scave.actions.GotoChartDefinitionAction;
import org.omnetpp.scave.actions.GroupAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.OpenAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.ShowVectorBrowserViewAction;
import org.omnetpp.scave.actions.UngroupAction;
import org.omnetpp.scave.actions.ZoomChartAction;
import org.omnetpp.scave.model.presentation.ScaveModelActionBarContributor;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 */
public class ScaveEditorContributor extends ScaveModelActionBarContributor {
	private static ScaveEditorContributor instance;
	
//	public IAction addResultFileAction;
//	public IAction addWildcardResultFileAction;
//	public IAction removeAction;
//	public IAction addToDatasetAction;
//	public IAction createDatasetAction;
//	public IAction createChartAction;

	// global retarget actions
	private RetargetAction undoRetargetAction;
	private RetargetAction redoRetargetAction;
	private RetargetAction deleteRetargetAction;
	
	// generic actions
	private IAction openAction;
	private IAction editAction;
	private IAction groupAction;
	private IAction ungroupAction;

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
	private IAction gotoChartDefinitionAction;

	// BrowseDataPage actions
	private IAction addFilterToDatasetAction;
	private IAction addSelectedToDatasetAction;
	private IAction copyToClipboardAction;
	private IAction createTempChartAction;
	private IAction showVectorBrowserViewAction;
	private Map<String,IAction> exportActions;
	
	/**
	 * Creates a multi-page contributor.
	 */
	public ScaveEditorContributor() {
		super(false);
		if (instance==null) instance = this;
	}

	public void init(IActionBars bars, IWorkbenchPage page) {
        openAction = registerAction(page, new OpenAction());
        editAction = registerAction(page, new EditAction());
        groupAction = registerAction(page, new GroupAction());
        ungroupAction = registerAction(page, new UngroupAction());

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
        gotoChartDefinitionAction = registerAction(page, new GotoChartDefinitionAction());

    	// BrowseDataPage actions
        addFilterToDatasetAction = registerAction(page, new AddFilterToDatasetAction());
    	addSelectedToDatasetAction = registerAction(page, new AddSelectedToDatasetAction());
    	exportActions = new HashMap<String,IAction>();
		for (String format : ExportDataAction.FORMATS) {
			IAction action = registerAction(page, new ExportDataAction(format));
			exportActions.put(format, action);
		}
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

        getPage().removePartListener(undoRetargetAction);
        getPage().removePartListener(redoRetargetAction);
        getPage().removePartListener(deleteRetargetAction);
        undoRetargetAction.dispose();
        redoRetargetAction.dispose();
        deleteRetargetAction.dispose();
    }

	public void contributeToMenu(IMenuManager manager) {
		// super.contributeToMenu(manager);
	}

	public void contributeToToolBar(IToolBarManager manager) {
		super.contributeToToolBar(manager);

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
    	manager.add(deleteRetargetAction);

    	manager.add(openAction);
    	manager.add(editAction);
		
		manager.insertBefore("scavemodel-additions", createTempChartAction);

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
		// This is called for context menus of the model tree viewers
		super.menuAboutToShow(menuManager);
		menuManager.insertBefore("additions", openAction);
		menuManager.insertBefore("additions", editAction);
		
		menuManager.insertBefore("edit", groupAction);
		menuManager.insertBefore("edit", ungroupAction);
		menuManager.insertBefore("edit", new Separator());
		menuManager.insertBefore("edit", createExportMenu());
	}
	
	public IAction getOpenAction() {
		return openAction;
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
    public IAction getGotoChartDefinitionAction() {
		return gotoChartDefinitionAction;
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
	public IAction getCopyToClipboardAction() {
		return copyToClipboardAction;
	}
	public IAction getCreateTempChartAction() {
		return createTempChartAction;
	}
	public IAction getShowVectorBrowserViewAction() {
		return showVectorBrowserViewAction;
	}
	public IMenuManager createExportMenu() {
		IMenuManager exportMenu = new MenuManager("Export to File");
		if (exportActions != null) {
			for (String format : ExportDataAction.FORMATS) {
				IAction action = exportActions.get(format);
				if (action != null)
					exportMenu.add(action);
			}
		}
		return exportMenu;
	}
}
