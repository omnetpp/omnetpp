/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
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
            if (editor instanceof ScaveEditor && selection instanceof IStructuredSelection && isApplicable((ScaveEditor)editor, (IStructuredSelection)selection)) {
                ScaveEditor scaveEditor = (ScaveEditor)editor;
                IStructuredSelection structuredSelection = (IStructuredSelection)selection;
                doRun(scaveEditor, structuredSelection);
            }
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Internal error: " + e.toString());
            ScavePlugin.logError(e);
        }
    }

    /**
     * Gets invoked from run() if the editor is ScaveEditor and selection is
     * an IStructuredSelection -- redefine it to do the real work.
     */
    protected abstract void doRun(ScaveEditor scaveEditor, IStructuredSelection selection);

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
        boolean isApplicable = editor instanceof ScaveEditor && selection instanceof IStructuredSelection && isApplicable((ScaveEditor)editor, (IStructuredSelection)selection);
        if (isEnabled()!=isApplicable)
            setEnabled(isApplicable);
    }

    /**
     * Updates the Enabled property of this action.
     * To be called when some state changed which the enabled property
     * the action depends on.
     */
    public void updateEnabled() {
        IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
        if (page != null) {
            IEditorPart editor = page.getActiveEditor();
            ISelection selection = viewer != null ? viewer.getSelection() : page.getSelection();
            boolean isApplicable = editor instanceof ScaveEditor && selection instanceof IStructuredSelection && isApplicable((ScaveEditor)editor, (IStructuredSelection)selection);
            if (isEnabled()!=isApplicable)
                setEnabled(isApplicable);
        }
    }

    /**
     * Redefine this method to control when the action should be enabled.
     */
    abstract protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection);

    /**
     * Utility function for use in isApplicable()
     */
    protected static boolean containsEObjectsOnly(IStructuredSelection selection) {
        if (selection.isEmpty())
            return true;
        for (Object o : selection.toArray())
            if (!(o instanceof EObject))
                return false;
        return true;
    }
}
