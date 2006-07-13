package org.omnetpp.scave2.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.wizard.NewScaveObjectWizard;

/**
 * Appends a new child to the children of the selected object.
 * It opens a wizard where the type of the new child and its
 * attributes can be entered.
 */
public class NewAction extends AbstractScaveAction {
	public NewAction() {
		setText("New...");
		setToolTipText("Create new item");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		EObject parent = getParent(selection);
		if (parent != null) {
			NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor.getEditingDomain(), parent);
			WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
			if (dialog.open() == Window.OK) {
				Command command = AddCommand.create(editor.getEditingDomain(), parent, null, wizard.getNewScaveObject());
				editor.executeCommand(command);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return getParent(selection) != null;
	}
	
	private EObject getParent(IStructuredSelection selection) {
		return selection != null && selection.size() == 1 && selection.getFirstElement() instanceof EObject ?
			(EObject)selection.getFirstElement() : null;
	}
}
