package org.omnetpp.scave2.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected chart sheets in the editor.
 */
public class OpenChartSheetActionDelegate extends AbstractScaveActionDelegate {
	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection structuredSelection) {
		for (Object element : structuredSelection.toArray()) {
			if (element instanceof ChartSheet) {
				ChartSheet dataset = (ChartSheet)element;
				scaveEditor.openChartSheet(dataset);
			}
		}
	}
}
