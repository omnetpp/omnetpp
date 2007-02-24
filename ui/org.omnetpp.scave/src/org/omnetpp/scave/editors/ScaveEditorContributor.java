package org.omnetpp.scave.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.scave.actions.AddFilterToDatasetAction;
import org.omnetpp.scave.actions.AddSelectedToDatasetAction;
import org.omnetpp.scave.actions.CopyChartToClipboardAction;
import org.omnetpp.scave.actions.CopyToClipboardAction;
import org.omnetpp.scave.actions.CreateChartTemplateAction;
import org.omnetpp.scave.actions.CreateTempChartAction;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.IScaveAction;
import org.omnetpp.scave.actions.RefreshChartAction;
import org.omnetpp.scave.actions.ShowVectorBrowserViewAction;
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
//	public IAction openAction;
//	public IAction editAction;
//	public IAction removeAction;
//	public IAction addToDatasetAction;
//	public IAction createDatasetAction;
//	public IAction createChartAction;
	
	// generic actions
	private IAction editAction;

	// ChartPage actions
	private IAction copyChartToClipboardAction;
	private IAction zoomInXAction;
	private IAction zoomOutXAction;
	private IAction zoomInYAction;
	private IAction zoomOutYAction;
	private IAction refreshChartAction;
	private IAction createChartTemplateAction;

	// BrowseDataPage actions
	private IAction addFilterToDatasetAction;
	private IAction addSelectedToDatasetAction;
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
        copyChartToClipboardAction = registerAction(page, new CopyChartToClipboardAction());
        zoomInXAction = registerAction(page, new ZoomChartAction(true, 1.5));
        zoomOutXAction = registerAction(page, new ZoomChartAction(true, 1/1.5));
        zoomInYAction = registerAction(page, new ZoomChartAction(false, 1.5));
        zoomOutYAction = registerAction(page, new ZoomChartAction(false, 1/1.5));
        refreshChartAction = registerAction(page, new RefreshChartAction());
        createChartTemplateAction = registerAction(page, new CreateChartTemplateAction());

    	// BrowseDataPage actions
        addFilterToDatasetAction = registerAction(page, new AddFilterToDatasetAction());
    	addSelectedToDatasetAction = registerAction(page, new AddSelectedToDatasetAction());
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
		super.contributeToMenu(manager);
	}

	public void contributeToToolBar(IToolBarManager manager) {
		super.contributeToToolBar(manager);
		manager.insertBefore("scavemodel-additions", zoomInXAction);
		manager.insertBefore("scavemodel-additions", zoomOutXAction);
		manager.insertBefore("scavemodel-additions", zoomInYAction);
		manager.insertBefore("scavemodel-additions", zoomOutYAction);
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
    public IAction getZoomInXAction() {
		return zoomInXAction;
	}
    public IAction getZoomOutXAction() {
		return zoomOutXAction;
	}
    public IAction getZoomInYAction() {
		return zoomInYAction;
	}
    public IAction getZoomOutYAction() {
		return zoomOutYAction;
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
