package org.omnetpp.scave2.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Removes selected elements.
 */
public class RemoveAction extends AbstractScaveAction {
	public RemoveAction() {
		setText("Remove");
		setToolTipText("Remove selected items");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
		// use EMF.Edit Framework do to the removal (this makes it undoable)
		Command command = RemoveCommand.create(scaveEditor.getEditingDomain(), structuredSelection.toList());
		scaveEditor.getEditingDomain().getCommandStack().execute(command);
	}

	@Override
	public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return !selection.isEmpty() && containsEObjectsOnly(selection);
	}
}
