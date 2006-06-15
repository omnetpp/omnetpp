package org.omnetpp.scave2.actions;

import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
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
				editor.addNewDataset(name, type, params);
			}
		}
	}

	@Override
	protected boolean isEnabled(ScaveEditor editor,
			IStructuredSelection selection) {
		return editor.getBrowseDataPage().getActivePanel() != null;
	}

}
