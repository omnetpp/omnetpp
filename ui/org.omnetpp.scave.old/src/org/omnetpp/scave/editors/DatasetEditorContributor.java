/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;

import org.omnetpp.scave.actions.CopyAction;
import org.omnetpp.scave.actions.CutAction;
import org.omnetpp.scave.actions.DeleteAction;
import org.omnetpp.scave.actions.DeleteRestAction;
import org.omnetpp.scave.actions.ExtendDatasetAction;
import org.omnetpp.scave.actions.OpenResultFileAction;
import org.omnetpp.scave.actions.PasteAction;
import org.omnetpp.scave.actions.RedoAction;
import org.omnetpp.scave.actions.SelectAllAction;
import org.omnetpp.scave.actions.UndoAction;

/**
 * Manages the installation/deinstallation of global actions for multi-page editors.
 * Responsible for the redirection of global actions to the active editor.
 * Multi-page contributor replaces the contributors for the individual editors in the multi-page editor.
 *
 * FIXME do we need as many subclasses as editors?
 */
public class DatasetEditorContributor extends MultiPageEditorActionBarContributor {
    private IEditorPart activeEditorPart;

    private Action openResultFileAction;
    private Action extendDatasetAction;
    private Action deleteRestAction;

    // global actions will be retargetted to these actions
    private IAction localCutAction;
    private IAction localCopyAction;
    private IAction localPasteAction;
    private IAction localDeleteAction;
    private IAction localUndoAction;
    private IAction localRedoAction;
    private IAction localSelectAllAction;


    /**
     * Creates a multi-page contributor.
     */
    public DatasetEditorContributor() {
        super();
        createActions();
    }
    private void createActions() {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        openResultFileAction = new OpenResultFileAction(workbenchWindow);
        extendDatasetAction = new ExtendDatasetAction(workbenchWindow);
        deleteRestAction = new DeleteRestAction(workbenchWindow);

        // create local implementations of common actions
        localCutAction = new CutAction(workbenchWindow);
        localCopyAction = new CopyAction(workbenchWindow);
        localPasteAction = new PasteAction(workbenchWindow);
        localDeleteAction = new DeleteAction(workbenchWindow);
        localUndoAction = new UndoAction(workbenchWindow);
        localRedoAction = new RedoAction(workbenchWindow);
        localSelectAllAction = new SelectAllAction(workbenchWindow);
    }

    public void init(IActionBars bars, IWorkbenchPage page) {
        super.init(bars, page);
        bars.setGlobalActionHandler(ActionFactory.CUT.getId(), localCutAction);
        bars.setGlobalActionHandler(ActionFactory.COPY.getId(), localCopyAction);
        bars.setGlobalActionHandler(ActionFactory.PASTE.getId(), localPasteAction);
        bars.setGlobalActionHandler(ActionFactory.DELETE.getId(), localDeleteAction);
        bars.setGlobalActionHandler(ActionFactory.UNDO.getId(), localUndoAction);
        bars.setGlobalActionHandler(ActionFactory.REDO.getId(), localRedoAction);
        bars.setGlobalActionHandler(ActionFactory.SELECT_ALL.getId(), localSelectAllAction);
    }

    public void dispose() {
    }

    public void contributeToMenu(IMenuManager manager) {
        MenuManager datasetMenu = new MenuManager("Dataset");
        // It is important to append the menu to the group "additions". This group
        // is created between "Project" and "Tools" menus for this purpose.
        manager.insertAfter("additions", datasetMenu); //$NON-NLS-1$
        datasetMenu.add(openResultFileAction);
        datasetMenu.add(extendDatasetAction);
        datasetMenu.add(deleteRestAction);
    }

    public void contributeToToolBar(IToolBarManager manager) {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();

        // the SelectAll command didn't have an icon by default -- create it beforehand
        IAction selectAllAction = ActionFactory.SELECT_ALL.create(workbenchWindow);
        selectAllAction.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(IDE.SharedImages.IMG_OPEN_MARKER)); // FIXME icon

        // add command icons to toolbar
        manager.add(new Separator());
        manager.add(openResultFileAction);
        manager.add(new Separator());
        manager.add(ActionFactory.CUT.create(workbenchWindow));
        manager.add(ActionFactory.COPY.create(workbenchWindow));
        manager.add(ActionFactory.PASTE.create(workbenchWindow));
        manager.add(new Separator());
        manager.add(selectAllAction);
        manager.add(ActionFactory.DELETE.create(workbenchWindow));
        manager.add(deleteRestAction);
        manager.add(extendDatasetAction);
        manager.add(new Separator());
        manager.add(ActionFactory.UNDO.create(workbenchWindow));
        manager.add(ActionFactory.REDO.create(workbenchWindow));
    }

    public void contributeToStatusLine(IStatusLineManager statusLineManager) {
        super.contributeToStatusLine(statusLineManager);
    }

    public void setActivePage(IEditorPart part) {
        if (activeEditorPart == part)
            return;
        activeEditorPart = part;
        // do something...
    }
}
