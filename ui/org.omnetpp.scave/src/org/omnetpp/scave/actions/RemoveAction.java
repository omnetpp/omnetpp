package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.DeleteCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.edit.provider.IWrapperItemProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Removes selected elements.
 */
public class RemoveAction extends AbstractScaveAction {
	public RemoveAction() {
		setText("Delete");
		setToolTipText("Delete selected items");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
		Command command = createCommand(
							scaveEditor.getEditingDomain(),
							structuredSelection);
		scaveEditor.getEditingDomain().getCommandStack().execute(command);
	}

	@SuppressWarnings("unchecked")
	@Override
	public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		if (selection.isEmpty())
			return false;
		Iterator<Object> elements = selection.iterator();
		while (elements.hasNext()) {
			Object element = elements.next();
			if (element instanceof IWrapperItemProvider)
				element = ((IWrapperItemProvider)element).getValue();
			if (!(element instanceof EObject) || editor.isTemporaryObject((EObject)element))
				return false;
		}
		return true; // only non-temporary EObjects selected
	}

	/**
	 * Creates a command that removes the selected objects.
	 * 
	 * Simple DeleteCommand.create(ed, selection.toList()) does not work,
	 * because it would delete the referenced node when the references are
	 * displayed under their parent (Edit.Children=true).
	 * 
	 * The solution is to wrap the reference values
	 * (overriding isWrappingNeeded() in the ItemProvider) and execute
	 * RemoveCommand on them (instead of DeleteCommand).
	 * 
	 * TODO: fix EditingDomainActionBarContributor.deleteAction too.
	 */
	private Command createCommand(EditingDomain ed, IStructuredSelection selection) {
		CompoundCommand command = new CompoundCommand("Remove");
		Collection<Object> references = new ArrayList<Object>();
		Collection<Object> containments = new ArrayList<Object>();
		for (Iterator<?> i = selection.iterator(); i.hasNext();) {
			Object object = i.next();
			if (object instanceof IWrapperItemProvider)
				references.add(object);
			else
				containments.add(object);
		}
		if (references.size() > 0)
			command.append(RemoveCommand.create(ed, references));
		if (containments.size() > 0)
			command.append(DeleteCommand.create(ed, containments));
		return command.unwrap();
	}
}
