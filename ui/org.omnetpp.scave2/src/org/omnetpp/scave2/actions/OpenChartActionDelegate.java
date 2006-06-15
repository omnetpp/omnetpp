package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave2.editors.ScaveEditor;

public class OpenChartActionDelegate extends AbstractScaveActionDelegate {

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
			editor.openChart((Chart)selection.getFirstElement());
	}

	@Override
	protected boolean isEnabled(ScaveEditor editor, IStructuredSelection selection) {
		return selection != null && selection.size() == 1 &&
			selection.getFirstElement() instanceof Chart;
	}

}
