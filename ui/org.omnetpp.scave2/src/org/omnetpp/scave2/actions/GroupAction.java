package org.omnetpp.scave2.actions;

import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * ...
 */
public class GroupAction extends AbstractScaveAction {
	public GroupAction() {
		setText("Group");
		setToolTipText("Surround selected items with a group item");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		if (selection != null) {
			Range range = getRange(selection);
			if (range != null) {
				Collection elements = selection.toList();
				Group group = ScaveModelFactory.eINSTANCE.createGroup();
				CompoundCommand command = new CompoundCommand("Group");
				command.append(new RemoveCommand(editor.getEditingDomain(), range.container, elements));
				command.append(new AddCommand(editor.getEditingDomain(), range.container, group, range.fromIndex));
				command.append(new AddCommand(editor.getEditingDomain(), group.getItems(), elements));
				editor.executeCommand(command);
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return selection != null && selection.size() >= 1 && getRange(selection) != null;
	}
	
	static class Range {
		public EList container;
		public int fromIndex;
		public int toIndex;
	}
	
	private Range getRange(IStructuredSelection selection) {
		// TODO: convert the selection to a range
		return null;
	}
}
