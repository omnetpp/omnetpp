package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.provider.IWrapperItemProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Opens the selected datasets/charts/chart sheets in the editor.
 */
public class OpenAction extends AbstractScaveAction {
	public OpenAction() {
		setText("Open");
		setToolTipText("Open item in a separate page");
	}
	
	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		for (Object element : selection.toArray()) {
			Object object = AdapterFactoryEditingDomain.unwrap(element);
			if (object instanceof Dataset) {
				Dataset dataset = (Dataset)object;
				scaveEditor.openDataset(dataset);
			}
			else if (object instanceof ChartSheet) {
				ChartSheet chartsheet = (ChartSheet)object;
				scaveEditor.openChartSheet(chartsheet);
			}
			else if (object instanceof Chart) {
				Chart chart = (Chart)object;
				scaveEditor.openChart(chart);
			}
		}
	}

	@Override
	public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		for (Object element : selection.toArray())
			if (element instanceof Dataset || element instanceof ChartSheet ||
				element instanceof Chart || element instanceof IWrapperItemProvider)
				return true;
		return false;
	}
}
