package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model2.ComputedResultFileUpdater;

public class RefreshComputedDataFileAction extends AbstractScaveAction {

	public RefreshComputedDataFileAction() {
		setText("Refresh computed data");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		if (selection != null && selection.getFirstElement() instanceof ProcessingOp)
		{
			ProcessingOp selected = (ProcessingOp)selection.getFirstElement();
			selected.setComputationHash(0);
			ComputedResultFileUpdater.instance().ensureComputedFile(selected, scaveEditor.getResultFileManager(), null);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return 	selection.getFirstElement() instanceof ProcessingOp;
	}
}

