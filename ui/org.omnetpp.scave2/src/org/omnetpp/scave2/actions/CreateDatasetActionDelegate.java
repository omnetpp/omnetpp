package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.editors.ui.FilterPanel;
import org.omnetpp.scave2.model.FilterParams;

public class CreateDatasetActionDelegate extends AbstractScaveActionDelegate {

	@Override
	protected void doRun(ScaveEditor editor,
			IStructuredSelection selection) {
		if (isEnabled(editor, selection)) {
			FilterPanel activePanel = editor.getBrowseDataPage().getActivePanel();
	
			InputDialog dialog = new InputDialog(editor.getSite().getShell(),
					"Create dataset", "Dataset name:", "", null);
			int result = dialog.open();
			if (result == Window.OK) {
				String name = dialog.getValue();
				String type = activePanel == editor.getBrowseDataPage().getScalarsPanel() ?
						"scalar" : "vector";
				FilterParams params = activePanel.getFilterParams();
				Dataset dataset = editor.createDataset(name, type, params);
				executeCommand(new AddCommand(
						editor.getEditingDomain(),
						editor.getAnalysisModelObject().getDatasets().getDatasets(),
						dataset));
				editor.openDataset(dataset);
			}
		}
	}

	@Override
	protected boolean isEnabled(ScaveEditor editor,
			IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}

}
