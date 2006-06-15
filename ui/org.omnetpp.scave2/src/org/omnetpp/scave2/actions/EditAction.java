package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected datasets/charts/chart sheets in the editor.
 */
public class EditAction extends AbstractScaveAction {
	public EditAction() {
        setText("Edit...");
        setToolTipText("Edit selected item");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		//TODO open edit dialog
	}

	@Override
	public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return !selection.isEmpty() && containsEObjectsOnly(selection);
	}
}
