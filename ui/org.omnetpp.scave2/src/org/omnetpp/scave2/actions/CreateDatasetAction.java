package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave2.model.FilterParams;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class CreateDatasetAction extends AbstractScaveAction {
	public CreateDatasetAction() {
		setText("Create dataset...");
		setToolTipText("Create dataset from current data");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();

		InputDialog dialog = new InputDialog(editor.getSite().getShell(),
				"Create dataset", "Dataset name:", "", null);
		int result = dialog.open();
		if (result == Window.OK) {
			String name = dialog.getValue();
			DatasetType type = activePanel == editor.getBrowseDataPage().getScalarsPanel() ?
					DatasetType.SCALAR_LITERAL : DatasetType.VECTOR_LITERAL; //XXX histogram
			FilterParams params = activePanel.getFilterParams();
			Dataset dataset = ScaveModelUtil.createDataset(name, type, params);
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
