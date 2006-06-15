package org.omnetpp.scave2.actions;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Base class to factor out common code in Action classes. 
 * @author andras
 */
public abstract class AbstractScaveAction extends Action implements IScaveAction {

	/**
	 * Delegate work to doRun() if the editor is ScaveEditor and selection is
	 * an IStructuredSelection; otherwise ignore the request.
	 * Redefine isApplicable() to refine the above condition. 
	 */
	@Override
	public void run() {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart editor = page.getActiveEditor();
		ISelection selection = page.getSelection();
		if (editor instanceof ScaveEditor && selection instanceof IStructuredSelection && isApplicable((ScaveEditor)editor, (IStructuredSelection)selection)) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			doRun(scaveEditor, structuredSelection);
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
