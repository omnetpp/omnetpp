package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave2.editors.ui.CreateDatasetDialog;
import org.omnetpp.scave2.model.ScaveModelUtil;

/**
 * Creates a dataset from the filter setting on the BrowseDataPage and 
 * creates a chart displaying the dataset. 
 */
public class CreateChartAction extends AbstractScaveAction {
	public CreateChartAction() {
		setText("Create chart...");
		setToolTipText("Create chart from current data");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel == null)
			return;
		
		CreateDatasetDialog dialog = new CreateDatasetDialog(editor.getSite().getShell(), "Create chart");
		dialog.setFilterParams(activePanel.getFilterParams());
		dialog.setFilterHints(activePanel.getFilterHints());
		
		int result = dialog.open();
		if (result == Window.OK) {
			Dataset dataset = dialog.useFilter() ?
				ScaveModelUtil.createDataset(
						dialog.getDatasetName(),
						activePanel.getTable().getDataType(),
						dialog.getFilterParams()) :
				ScaveModelUtil.createDataset(
						dialog.getDatasetName(),
						activePanel.getTable().getDataType(),
						activePanel.getTable().getSelectedItems(),
						dialog.getRunIdKind());
			Chart chart = ScaveModelUtil.createChart("Chart of " + dialog.getDatasetName());
			dataset.getItems().add(chart);
			
			editor.executeCommand(AddCommand.create(
					editor.getEditingDomain(),
					editor.getAnalysis().getDatasets(),
					ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
					dataset));
			editor.openChart(chart);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
