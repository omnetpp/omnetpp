package org.omnetpp.scave.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.model2.ScaveModelUtil.RunIdKind;

/**
 * Creates a dataset from the filter setting on the BrowseDataPage and 
 * creates a chart displaying the dataset. 
 */
public class CreateTempChartAction extends AbstractScaveAction {
	public CreateTempChartAction() {
		setText("Create chart...");
		setToolTipText("Create chart from current data");
	}

	@SuppressWarnings("unchecked")
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel == null || activePanel.getTable().getSelectionCount() == 0)
			return;
		
			String datasetName = "dataset";
			Dataset dataset =
				ScaveModelUtil.createDataset(
						datasetName,
						activePanel.getTable().getDataType(),
						activePanel.getTable().getSelectedItems(),
						RunIdKind.FILE_RUN);
			String chartName = "chart"; //FIXME generate proper name
			Chart chart = ScaveModelUtil.createChart(chartName);
			dataset.getItems().add(chart);
			
			editor.executeCommand(AddCommand.create(
					editor.getEditingDomain(),
					editor.getTempAnalysis().getDatasets(),
					ScaveModelPackage.eINSTANCE.getDatasets_Datasets(),
					dataset));
			editor.openChart(chart);
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel(); 
		return activePanel != null && activePanel.getTable().getSelectionCount() > 0;
	}
}
