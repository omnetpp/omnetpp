package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.DataTable;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave2.editors.ui.CreateDatasetDialog;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.ScaveModelUtil;
import org.omnetpp.scave2.model.ScaveModelUtil.RunIdKind;

public class CreateDatasetAction extends AbstractScaveAction {
	public CreateDatasetAction() {
		setText("Create dataset...");
		setToolTipText("Create dataset from current data");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();

		CreateDatasetDialog dialog = new CreateDatasetDialog(editor.getSite().getShell());
		dialog.setFilterParams(activePanel.getFilterParams());

		int result = dialog.open();
		if (result == Window.OK) {
			String name = dialog.getDatasetName();
			DatasetType type = 
				activePanel == editor.getBrowseDataPage().getScalarsPanel() ? DatasetType.SCALAR_LITERAL :
				activePanel == editor.getBrowseDataPage().getVectorsPanel() ? DatasetType.VECTOR_LITERAL :
					DatasetType.HISTOGRAM_LITERAL;
			Dataset dataset;
			
			if (dialog.useFilter()) {
				FilterParams params = dialog.getFilterParams();
				dataset = ScaveModelUtil.createDataset(name, type, params);
			} else {
				DataTable table = activePanel.getTable();
				ResultItem[] items = table.getSelectedItems();
				RunIdKind idKind = dialog.getRunIdKind();
				dataset = ScaveModelUtil.createDataset(name, type, items, idKind);
			}
			
			editor.executeCommand(new AddCommand(
					editor.getEditingDomain(),
					editor.getAnalysis().getDatasets().getDatasets(),
					dataset));
			editor.openDataset(dataset);
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}
}
