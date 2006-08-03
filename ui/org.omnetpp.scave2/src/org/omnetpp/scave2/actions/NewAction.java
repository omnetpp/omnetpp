package org.omnetpp.scave2.actions;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.CreateChildCommand;
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
	private EObject defaultParent;
	
	public NewAction() {
		this(null);
	}

	public NewAction(EObject defaultParent) {
		this.defaultParent = defaultParent;
		setText("New...");
		setToolTipText("Create new item");
	}
	
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		EObject parent = getParent(selection);
		if (parent != null) {
			NewScaveObjectWizard wizard = new NewScaveObjectWizard(editor, parent);
			WizardDialog dialog = new WizardDialog(editor.getSite().getShell(), wizard);
			if (dialog.open() == Window.OK) {
				Command command = CreateChildCommand.create(
									editor.getEditingDomain(),
									parent,
									wizard.getNewChildDescriptor(),
									selection.toList());  
				editor.executeCommand(command);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return getParent(selection) != null;
	}
	
	private EObject getParent(IStructuredSelection selection) {
		if (selection!=null && selection.size()==1 && selection.getFirstElement() instanceof EObject)
			return (EObject)selection.getFirstElement();
		else
			return defaultParent;
	}
}
