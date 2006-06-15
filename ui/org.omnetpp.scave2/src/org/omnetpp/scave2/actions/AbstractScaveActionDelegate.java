package org.omnetpp.scave2.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorActionDelegate;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Base class to factor out common code in ActionDelegate classes. 
 * @author andras
 */
public abstract class AbstractScaveActionDelegate implements IEditorActionDelegate {

	private ScaveEditor editor;
	private IStructuredSelection selection;
	
	public void setActiveEditor(IAction action, IEditorPart targetEditor) {
		if (editor != targetEditor) {
			if (targetEditor instanceof ScaveEditor)
				editor = (ScaveEditor)targetEditor;
			else
				editor = null;
			ISelection selection = editor != null ? editor.getSelection() : null;
			selectionChanged(action, selection);
		}
	}
	
	/**
	 * Delegate work to doRun() if the editor is ScaveEditor and selection is
	 * an IStructuredSelection; otherwise ignore the request. 
	 */
	public void run(IAction action) {
		if (editor != null && isEnabled(editor, selection)) {
			doRun(editor, selection);
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
		this.selection = null;
		if (selection instanceof IStructuredSelection) {
			this.selection = (IStructuredSelection)selection;
		}
		action.setEnabled(editor != null && isEnabled(editor, this.selection));
	}
	
	/**
	 * Decides if the action can be executed.
	 */
	protected abstract boolean isEnabled(ScaveEditor editor, IStructuredSelection selection);
	
	/**
	 * Utility method. 
	 */
	protected void executeCommand(Command command) {
		editor.getEditingDomain().getCommandStack().execute(command);

	}
}
