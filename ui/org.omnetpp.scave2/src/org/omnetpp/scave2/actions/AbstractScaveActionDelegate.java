package org.omnetpp.scave2.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IActionDelegate;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Base class to factor out common code in ActionDelegate classes. 
 * @author andras
 */
public abstract class AbstractScaveActionDelegate implements IActionDelegate {
	
	/**
	 * Delegate work to doRun() if the editor is ScaveEditor and selection is
	 * an IStructuredSelection; otherwise ignore the request. 
	 */
	public void run(IAction action) {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart editor = page.getActiveEditor();
		ISelection selection = page.getSelection();
		if (editor instanceof ScaveEditor && selection instanceof IStructuredSelection) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			doRun(scaveEditor, structuredSelection);
		}
	}

	/**
	 * Gets invoked from run() if the editor is ScaveEditor and selection is
	 * an IStructuredSelection -- redefine it to do the real work. 
	 */
	protected abstract void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection);

	/**
	 * Disable the action if the editor is not a ScaveEditor or 
	 * selection is not an IStructuredSelection.  
	 */
	public void selectionChanged(IAction action, ISelection selection) {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart editor = page.getActiveEditor();
		if (editor instanceof ScaveEditor && selection instanceof IStructuredSelection) {
			action.setEnabled(false);
		} else {
			action.setEnabled(true);
		}
	}
}
