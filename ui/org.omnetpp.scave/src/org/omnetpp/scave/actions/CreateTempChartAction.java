package org.omnetpp.scave.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary dataset from the selection on the BrowseDataPage and 
 * creates a temporary chart containing the items of the dataset.
 * It opens a chart page to display the chart. 
 */
public class CreateTempChartAction extends AbstractScaveAction {
	public CreateTempChartAction() {
		setText("Create temporary chart...");
		setToolTipText("Create temporary chart from selected result items");
	}

	@SuppressWarnings("unchecked")
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
		if (activePanel == null || activePanel.getTable().getSelectionCount() == 0)
			return;
		
			String datasetName = "dataset";
			String[] runidFields = new String[] { Filter.FIELD_FILENAME, Filter.FIELD_RUNNAME};
			Dataset dataset =
				ScaveModelUtil.createDataset(
						datasetName,
						activePanel.getTable().getDataType(),
						activePanel.getTable().getSelectedItems(),
						runidFields);
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
