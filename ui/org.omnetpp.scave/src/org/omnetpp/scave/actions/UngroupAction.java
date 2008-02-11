package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.Collection;

import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.ReplaceCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Group;

/**
 * Ungroup the items of the selected group.
 */
public class UngroupAction extends AbstractScaveAction {
	public UngroupAction() {
		setText("Ungroup");
		setToolTipText("Remove group item and merge its contents");
	}

	@SuppressWarnings("unchecked")
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		Group group = getSelectedGroup(selection);
		if (group != null) {
			EObject parent = group.eContainer();
			Collection items = new ArrayList(group.getItems());
			if (parent != null && items.size() > 0) {
				Object elist = parent.eGet(group.eContainingFeature());
				if (elist instanceof EList) {
					CompoundCommand command = new CompoundCommand("Ungroup");
					command.append(RemoveCommand.create(
									editor.getEditingDomain(),
									items));
					command.append(ReplaceCommand.create(
									editor.getEditingDomain(),
									group.eContainer(),
									group.eContainingFeature(),
									group,
									items));
					editor.executeCommand(command);
				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		Group group = getSelectedGroup(selection);
		return group != null && group.eContainer() != null && group.getItems().size() > 0;
	}
	
	private static Group getSelectedGroup(IStructuredSelection selection) {
		if (selection != null && selection.size() == 1 && 
			selection.getFirstElement() instanceof Group)
			return (Group)selection.getFirstElement();
		else
			return null;
	}
}
