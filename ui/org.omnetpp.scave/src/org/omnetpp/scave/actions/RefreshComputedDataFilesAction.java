package org.omnetpp.scave.actions;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model2.ComputedResultFileUpdater;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class RefreshComputedDataFilesAction extends AbstractScaveAction {

	public RefreshComputedDataFilesAction() {
		setText("Refresh computed");
	}

	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		if (selection != null && selection.getFirstElement() instanceof Dataset ||
			selection.getFirstElement() instanceof DatasetItem)
		{
			EObject selected = (EObject)selection.getFirstElement();
			Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(selected, Dataset.class);
			DatasetItem target = selected instanceof DatasetItem ? (DatasetItem)selected : null;
			ComputedResultFileUpdater.generateComputedFiles(dataset, target, scaveEditor.getResultFileManager());
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return 	selection.getFirstElement() instanceof Dataset ||
				selection.getFirstElement() instanceof DatasetItem;
	}
}

