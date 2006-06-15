package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected datasets in the editor.
 */
public class OpenDatasetActionDelegate extends AbstractScaveActionDelegate {
	
	@Override
	public void doRun(ScaveEditor editor, IStructuredSelection selection) {
		if (isEnabled(editor, selection)) {
			editor.openDataset((Dataset)selection.getFirstElement());
		}
	}
	
	@Override
	public boolean isEnabled(ScaveEditor editor, IStructuredSelection selection) {
		return selection != null && selection.size() == 1 && 
				selection.getFirstElement() instanceof Dataset;
	}
}
