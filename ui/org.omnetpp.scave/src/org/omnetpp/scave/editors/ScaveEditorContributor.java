package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.RetargetAction;
import org.eclipse.ui.part.IPage;
import org.eclipse.ui.views.properties.IPropertySheetPage;

import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.*;
import org.omnetpp.scave.model.presentation.ScaveModelActionBarContributor;
import org.omnetpp.scave.views.DatasetView;

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
	private IScaveAction deleteAction; // action handler of deleteRetargetAction
	private IAction selectAllAction;
	private IAction refreshComputedFilesAction;

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
	 * This action opens the Dataset view.
	 */
	private IAction showDatasetViewAction =
		new Action("Show Dataset view") {
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

	/**
	 * Creates a multi-page contributor.
	 */
	public ScaveEditorContributor() {
		super(false);
		if (instance==null) instance = this;
	}

	@Override
    public void init(IActionBars bars, IWorkbenchPage page) {
        openAction = registerAction(page, new OpenAction());
        editAction = registerAction(page, new EditAction());
        groupAction = registerAction(page, new GroupAction());
        ungroupAction = registerAction(page, new UngroupAction());
        selectAllAction = registerAction(page, new SelectAllAction());
        refreshComputedFilesAction = registerAction(page, new RefreshComputedDataFileAction());

        // replacement of the inherited deleteAction
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        deleteAction = registerAction(page, new RemoveAction());
        deleteAction.setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));

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

        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), deleteAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), selectAllAction);
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
		// super.contributeToMenu(manager);
	}

	@Override
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

	@Override
    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
		super.contributeToStatusLine(statusLineManager);
	}

	@Override
	public void menuAboutToShow(IMenuManager menuManager) {
		// This is called for context menus of the model tree viewers
		super.menuAboutToShow(menuManager);
		// replace the inherited deleteAction with ours, that handle references and temp obects well
		IContributionItem deleteActionItem = null;
		for (IContributionItem item : menuManager.getItems())
			if (item instanceof ActionContributionItem) {
				ActionContributionItem acItem = (ActionContributionItem)item;
				if (acItem.getAction() == super.deleteAction) {
					deleteActionItem = item;
					break;
				}
			}
		if (deleteActionItem != null) {
			menuManager.remove(deleteActionItem);
			menuManager.insertBefore("additions-end", deleteAction);
		}

		menuManager.insertBefore("additions", openAction);
		menuManager.insertBefore("additions", editAction);

		menuManager.insertBefore("edit", refreshComputedFilesAction);
		menuManager.insertBefore("edit", groupAction);
		menuManager.insertBefore("edit", ungroupAction);
		menuManager.insertBefore("edit", new Separator());
		menuManager.insertBefore("edit", createExportMenu());
	}

	@Override
    public void shareGlobalActions(IPage page, IActionBars actionBars)
	{
		super.shareGlobalActions(page, actionBars);

		// replace inherited deleteAction
		if (!(page instanceof IPropertySheetPage))
			actionBars.setGlobalActionHandler(ActionFactory.DELETE.getId(), deleteAction);
	}

	@Override
	protected void addGlobalActions(IMenuManager menuManager) {
		menuManager.insertAfter("additions-end", new Separator("ui-actions"));
		menuManager.insertAfter("ui-actions", showPropertiesViewAction);
		menuManager.insertAfter("ui-actions", showDatasetViewAction);
		refreshViewerAction.setEnabled(refreshViewerAction.isEnabled());
		menuManager.insertAfter("ui-actions", refreshViewerAction);
	}

	@Override
    public void update()
	{
		super.update();
		// FIXME explain the intention of the code below

		ISelectionProvider selectionProvider =
			activeEditor instanceof ISelectionProvider ?
				(ISelectionProvider)activeEditor :
					activeEditor.getEditorSite().getSelectionProvider();

		if (selectionProvider != null)
		{
			ISelection selection = selectionProvider.getSelection();
			IStructuredSelection structuredSelection =
				selection instanceof IStructuredSelection ?  (IStructuredSelection)selection : StructuredSelection.EMPTY;

			deleteAction.selectionChanged(structuredSelection);
		}
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
	public RetargetAction getUndoRetargetAction() {
		return undoRetargetAction;
	}
	public RetargetAction getRedoRetargetAction() {
		return redoRetargetAction;
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
