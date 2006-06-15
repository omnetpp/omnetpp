package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.FilterPanel;
import org.omnetpp.scave2.model.FilterParams;

/**
 * Creates a dataset from the filter setting on the BrowseDataPage and 
 * creates a chart displaying the dataset. 
 */
public class CreateChartAction extends AbstractScaveAction {

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilterPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		InputDialog dialog = new InputDialog(editor.getSite().getShell(),
				"Create chart", "Dataset name:", "", null);
		int result = dialog.open();
		if (result == Window.OK) {
			String name = dialog.getValue();
			String type = activePanel == editor.getBrowseDataPage().getScalarsPanel() ?
					"scalar" : "vector";
			FilterParams params = activePanel.getFilterParams();
			Dataset dataset = editor.createDataset(name, type, params);
			Chart chart = editor.createChart("Chart of " + name);
			dataset.getItems().add(chart);
			editor.executeCommand(new AddCommand(
					editor.getEditingDomain(),
					editor.getAnalysisModelObject().getDatasets().getDatasets(),
					dataset));
			editor.openChart(chart);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
