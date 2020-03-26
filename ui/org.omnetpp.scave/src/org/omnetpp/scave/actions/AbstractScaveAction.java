/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Base class to factor out common code in Action classes. Subclasses
 * need to implement <code>doRun</code> and <code>isApplicable</code>
 * @author andras
 */
public abstract class AbstractScaveAction extends Action implements IScaveAction {

    /**
     * If set, then the target of the action is the selection of the viewer.
     */
    protected Viewer viewer;

    public AbstractScaveAction() {
    }

    public AbstractScaveAction(int style) {
        super("", style);
    }

    /**
     * Implements {@link IScaveAction#setViewer(Viewer)}.
     */
    public void setViewer(Viewer viewer) {
        this.viewer = viewer;
    }

    /**
     * Delegate work to doRun() if the editor is ScaveEditor and selection is
     * an IStructuredSelection; otherwise ignore the request.
     * Redefine isApplicable() to refine the above condition.
     */
    @Override
    public void run() {
        try {
            IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
            IEditorPart editor = page.getActiveEditor();
            ISelection selection = viewer != null ? viewer.getSelection() : page.getSelection();
            if (editor instanceof ScaveEditor && isApplicable((ScaveEditor)editor, selection))
                doRun((ScaveEditor)editor, selection);
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "An error occurred: " + e.toString());
            ScavePlugin.logError(e);
        }
    }

    /**
     * Gets invoked from run() if the editor is ScaveEditor -- redefine it to do the real work.
     * @throws CoreException TODO
     */
    protected abstract void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException;

    /**
     * To be called from a JFace {@link ISelectionChangedListener} or the selection service's
     * {@link ISelectionListener} to enable/disable the action. To get enabled, the editor
     * must be a ScaveEditor, selection an IStructuredSelection, and <code>isApplicable()</code>
     * should return true. Override <code>isApplicable()</code> accordingly.
     */
    public void selectionChanged(ISelection selection) {
        IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
        if (page==null) return;
        IEditorPart editor = page.getActiveEditor();
        boolean isApplicable = editor instanceof ScaveEditor && isApplicable((ScaveEditor)editor, selection);
        setEnabled(isApplicable);
    }

    /**
     * To be called when some state changed which the action depends on.
     */
    public void update() {
        IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
        if (page != null) {
            boolean enable = false;
            IEditorPart editor = page.getActiveEditor();
            if (editor instanceof ScaveEditor) {
                ISelection selection = viewer != null ? viewer.getSelection() : page.getSelection();
                enable = isApplicable((ScaveEditor)editor, selection != null ? selection : new StructuredSelection());
            }
            setEnabled(enable);
        }
    }

    /**
     * Redefine this method to control when the action should be enabled.
     */
    abstract protected boolean isApplicable(ScaveEditor editor, ISelection selection);

    /**
     * Utility method.
     */
    protected static IStructuredSelection asStructuredOrEmpty(ISelection selection) {
        return selection instanceof IStructuredSelection ? (IStructuredSelection)selection : StructuredSelection.EMPTY;
    }
}
