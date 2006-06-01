package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected datasets in the editor.
 */
public class OpenDatasetActionDelegate extends AbstractScaveActionDelegate {
	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
		for (Object element : structuredSelection.toArray()) {
			if (element instanceof Dataset) {
				Dataset dataset = (Dataset)element;
				scaveEditor.openDataset(dataset);
			}
		}
	}
}
