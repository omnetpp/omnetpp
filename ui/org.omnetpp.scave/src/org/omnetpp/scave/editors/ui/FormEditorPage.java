/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartViewer;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Common functionality of Scave multi-page editor pages with title and toolbar.
 * @author andras
 */
public class FormEditorPage extends Composite {
    //private static final Color PALETTE_BG_COLOR = new Color(null, 234, 240, 252);
    //private static final Color BUTTONS_BG_COLOR = new Color(null, 239, 244, 253);
    //private static final Color PALETTE_BG_COLOR = new Color(null, 241, 245, 253);
    //private static final Color BUTTONS_BG_COLOR = new Color(null, 249, 251, 254);

    protected ScaveEditor scaveEditor = null;  // backreference to the containing editor

    private Label formTitle;
    private Composite content;
    private ToolBar toolbar;
    private ToolBarManager toolbarManager;
    private String pageTitle = "untitled";

    protected FocusManager focusManager = null;

    public FormEditorPage(Composite parent, int style, ScaveEditor scaveEditor) {
        super(parent, style);
        setLayout(new GridLayout(2, false));
        formTitle = new Label(this, SWT.NONE);
        formTitle.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        toolbar = new ToolBar(this, SWT.NONE);
        content = new Composite(this, SWT.NONE);
        content.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)content.getLayoutData()).horizontalSpan = 2;

        this.scaveEditor = scaveEditor;
        this.toolbarManager = new ToolBarManager(toolbar);
    }

    public Label getFormTitle() {
        return formTitle;
    }

    public ToolBar getToolbar() {
        return toolbar;
    }

    public Composite getContent() {
        return content;
    }

    public void setFormTitle(String title) {
        formTitle.setFont(new Font(null, "Arial", 12, SWT.BOLD));
        formTitle.setForeground(new Color(null, 0, 128, 255));
        formTitle.setText(title);
    }

    /**
     * Return the text that should appear on the page's tab.
     */
    public String getPageTitle() {
        return pageTitle;
    }

    /**
     * Sets the text that should appear on the page's tab.
     */
    public void setPageTitle(String title) {
        pageTitle = title;
        scaveEditor.setPageTitle(this, title);
    }

    public FocusManager getFocusManager() {
        return focusManager;
    }

    public void setFocusManager(FocusManager focusManager) {
        this.focusManager = focusManager;
    }

    @Override
    public boolean setFocus() {
        // try to restore focus where it was last time
        if (getFocusManager() != null && getFocusManager().setFocus())
            return true;
        return super.setFocus();
    }

    /**
     * Updates the page title, etc.
     * Called when the model changed.
     */
    public void updatePage(Notification notification) {
    }

    public void addToToolbar(IAction action) {
        toolbarManager.add(new ActionContributionItem(action));
        toolbarManager.update(true);
    }

    protected void setToolbarActionVisible(IAction action, boolean visible) {
        IContributionItem item = findActionOnToolbar(action);
        item.setVisible(visible);
        toolbarManager.update(true);
    }

    private IContributionItem findActionOnToolbar(IAction action) {
        for (IContributionItem item : toolbarManager.getItems())
            if (item instanceof ActionContributionItem && ((ActionContributionItem)item).getAction() == action)
                return item;
        return null;
    }

    public void addSeparatorToToolbar() {
        toolbarManager.add(new Separator());
        toolbarManager.update(true);
    }

    /**
     * Sets up the given control so that when a file is drag-dropped into it,
     * it will be added to Inputs unless it's already in there.
     */
    public void setupResultFileDropTarget(Control dropTargetControl) {
        // set up DropTarget, and add FileTransfer to it;
        Assert.isTrue(dropTargetControl.getData("DropTarget") == null);
        DropTarget dropTarget = new DropTarget(dropTargetControl, DND.DROP_DEFAULT | DND.DROP_COPY);
        dropTarget.setTransfer(new Transfer[] {FileTransfer.getInstance()});

        // add our listener to handle file transfer to the DropTarget
        dropTarget.addDropListener(new DropTargetAdapter() {
            @Override
            public void dragEnter(DropTargetEvent event) {
                if (event.detail == DND.DROP_DEFAULT)
                    event.detail = DND.DROP_COPY;
            }
            @Override
            public void dragOperationChanged(DropTargetEvent event) {
                if (event.detail == DND.DROP_DEFAULT)
                    event.detail = DND.DROP_COPY;
            }
            @Override
            public void drop(DropTargetEvent event) {
                if (event.data instanceof String[]) {
                    String [] fileNames = (String[])event.data;
                    for (int i=0; i<fileNames.length; i++)
                        addDroppedFileToInputs(fileNames[i]);
                }
            }
        });
    }

    /**
     * Adds the given file to Inputs unless it's already in there.
     * The file name should be an OS path (D:\... or /home/you/...),
     * not a workspace path!
     *
     * XXX Limitation: currently the file must be available via the
     * workspace as well. This may get improved in the future.
     */
    private void addDroppedFileToInputs(String fileName) {
        // convert OS path to workspace path
        IFile iFile = ScaveEditor.findFileInWorkspace(fileName);
        if (iFile==null) {
            IStatus error =
                ScavePlugin.getErrorStatus(0, "Path not found in the workspace" + fileName, null);
            ErrorDialog.openError(getShell(), null, null, error);
            return;
        }
        scaveEditor.addWorkspaceFileToInputs(iFile);
    }

    public void showStatusMessage(String message) {
        IWorkbenchWindow window = scaveEditor.getSite().getWorkbenchWindow();
        if (window instanceof ApplicationWindow)
            ((ApplicationWindow)window).setStatus(message);
    }

    /**
     * Notification about the activation of the page of the
     * multipage editor.
     */
    public void pageActivated() {
    }

    /**
     * Notification about the change of the workbench selection.
     * Pages are synchronizing their selection of their viewers
     * by calling {@link #setViewerSelectionNoNotify(Viewer, ISelection)}.
     */
    public void selectionChanged(ISelection selection) {
    }

    protected void setViewerSelectionNoNotify(Viewer viewer, ISelection selection) {
        scaveEditor.setViewerSelectionNoNotify(viewer, selection);
    }

    /**
     * Returns the active chart on this page.
     */
    public ChartViewer getActiveChartViewer() {
        return null;
    }

    public boolean gotoObject(Object object) {
        return false;
    }

    /**
     * Saves the state of the page before the editor closed.
     * Override this method when the page has some persistable state.
     */
    public void saveState(IMemento memento) {
    }

    /**
     * Restores the state of the page from the saved state.
     * Override this method when the page has some persistable state.
     */
    public void restoreState(IMemento memento) {
    }
}
