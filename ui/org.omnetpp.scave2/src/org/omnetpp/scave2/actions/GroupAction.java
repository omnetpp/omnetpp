package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
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
		//TODO
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return true; //TODO
	}
}
