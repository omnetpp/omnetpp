package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected chart sheets in the editor.
 */
public class OpenChartSheetActionDelegate extends AbstractScaveActionDelegate {
	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
		if (isEnabled(scaveEditor, structuredSelection)) {
			ChartSheet chartsheet = (ChartSheet)structuredSelection.getFirstElement();
			scaveEditor.openChartSheet(chartsheet);
		}
	}
	
	@Override
	protected boolean isEnabled(ScaveEditor editor, IStructuredSelection selection) {
		return selection != null &&
				selection.size() == 1 &&
				selection.getFirstElement() instanceof ChartSheet;
	}
}
