package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * ...
 */
public class NewAction extends AbstractScaveAction {
	public NewAction() {
		setText("New...");
		setToolTipText("Create new item");
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
