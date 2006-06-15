package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * ...
 */
public class AddWildcardResultFileAction extends AbstractScaveAction {
	public AddWildcardResultFileAction() {
		setText("Add with wildcard...");
		setToolTipText("Use wildcards to specify a set of scalar or vector files");
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
