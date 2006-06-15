package org.omnetpp.scave2.editors;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.scave.model.presentation.ScaveModelActionBarContributor;
import org.omnetpp.scave2.actions.AbstractScaveAction;
import org.omnetpp.scave2.actions.AddResultFileAction;
import org.omnetpp.scave2.actions.AddToDatasetAction;
import org.omnetpp.scave2.actions.AddWildcardResultFileAction;
import org.omnetpp.scave2.actions.CreateChartAction;
import org.omnetpp.scave2.actions.CreateDatasetAction;
import org.omnetpp.scave2.actions.EditAction;
import org.omnetpp.scave2.actions.OpenAction;
import org.omnetpp.scave2.actions.RemoveAction;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 *
 * FIXME do we need as many subclasses as editors?
 */
public class ScaveEditorContributor extends ScaveModelActionBarContributor {
	private static ScaveEditorContributor instance;
	
	public IAction addResultFileAction;
	public IAction addWildcardResultFileAction;
	public IAction openAction;
	public IAction editAction;
	public IAction removeAction;
	public IAction addToDatasetAction;
	public IAction createDatasetAction;
	public IAction createChartAction;


	/**
	 * Creates a multi-page contributor.
	 */
	public ScaveEditorContributor() {
		super();
		if (instance==null) instance = this;
	}

	public void init(IActionBars bars, IWorkbenchPage page) {
        super.init(bars, page);

        addResultFileAction = registerAction(page, new AddResultFileAction());
        addWildcardResultFileAction = registerAction(page, new AddWildcardResultFileAction());
        openAction = registerAction(page, new OpenAction());
        editAction = registerAction(page, new EditAction());
		removeAction = registerAction(page, new RemoveAction());
		addToDatasetAction = registerAction(page, new AddToDatasetAction());
		createDatasetAction = registerAction(page, new CreateDatasetAction());
		createChartAction = registerAction(page, new CreateChartAction());
	}

	private IAction registerAction(IWorkbenchPage page, final AbstractScaveAction action) {
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
	}

	public void contributeToStatusLine(IStatusLineManager statusLineManager) {
		super.contributeToStatusLine(statusLineManager);
	}
}
